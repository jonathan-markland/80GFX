//
// 80GFX - Totally retro 1980s graphics library for C++ with no dependencies.
// Copyright (C) 2017  Jonathan Markland
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//		jynx_emulator {at} yahoo {dot} com
//


// [ ] Use of exception raising - to be considered.

#include "lib80GFX_Main.h"
#include "lib80GFX_Fonts.h"






// ** FIX: Not sure what's going to happen with these (where kept, namespace name):
namespace lib80GFX
{
	namespace Fonts
	{
		namespace Internal
		{
			namespace Gem
			{
				// [ ] TODO: Exception raising.

				bool ValidateTableFitsWithinFileImage( uint32_t tableOffset, uint32_t tableSize, uint64_t fileSize )
				{
					if( tableOffset == 0 ) return true; // allow not-present
					if( tableOffset < sizeof(Internal::GemFont) ) return false; // can't start within header
					if( (((uint64_t) tableOffset) + tableSize) > fileSize ) return false; // out of range of the file data
					return true;
				}



				int32_t Validate( const void *fileImage, uint64_t fileSize )
				{
					auto pFont = (const Internal::GemFont *) fileImage;

					// Basic:
					if( pFont == 0 || fileSize < sizeof(Internal::GemFont) ) return -999;

					auto &f = *pFont; // convenience

					if( f.SizeInPoints < 1 || f.SizeInPoints > 200 ) return -1;
					uint32_t i=0;
					for( ; i<32; i++ )
					{
						if( f.FaceName[i] == 0 ) break;
					}
					if( i == 32 ) return -2; // nul terminator not found
					if( f.LowestADE >= f.HighestADE ) return -3;

					if( f.FormWidth > MAX_FORM_WIDTH || f.FormWidth & 1 )
					{
						return -7;
					}
					if(	f.FormHeight > MAX_FORM_HEIGHT )
					{
						return -8;
					}

					if(	f.TopLineDistance > f.FormHeight ||
						f.AscentLineDistance > f.FormHeight ||
						f.HalfLineDistance > f.FormHeight ||
						f.DescentLineDistance > f.FormHeight ||
						f.BottomLineDistance > f.FormHeight )
					{
						return -4;
					}

					uint32_t  numChars = (f.HighestADE - f.LowestADE) + 1; // treating HighestADE as inclusive

					if( ! ValidateTableFitsWithinFileImage( f.PtrCharOffsetTable,  numChars*2, fileSize ) ) return -5;
					if( ! ValidateTableFitsWithinFileImage( f.PtrHorizOffsetTable, numChars*2, fileSize ) ) return -6;

					bool isCompressed = (f.Flags & GemFontFlags::Compressed) != 0;
					if( ! isCompressed )
					{
						if( ! ValidateTableFitsWithinFileImage( f.PtrFontData, uint32_t(f.FormWidth) * uint32_t(f.FormHeight), fileSize ) ) return -9;
					}
					else
					{
						if( fileSize < 152 ) return -10; // not enough room for the offset
						// We can't validate the huffman data
					}

					return 1; // success
				}



				// ** FIX: Unify with the le16() etc macros?
				inline void Inc( void *&ptr, intptr_t delta )    { ((intptr_t &) ptr) += delta; }
				inline void Dec( void *&ptr, intptr_t delta )    { ((intptr_t &) ptr) -= delta; }
				inline void StoreByte( void *ptr, uint8_t n )    { *((uint8_t  *) ptr) = n;  }
				inline void StoreWord( void *ptr, uint16_t n )   { *((uint16_t *) ptr) = n;  }
				inline uint8_t FetchByte( void *ptr )            { return *((uint8_t *) ptr); }
				inline uint16_t FetchWord( void *ptr )           { return *((uint16_t *) ptr); }



				void Decompress( void *sourceData, void *destData, uint32_t formWidth, uint32_t formHeight )
				{
					// Now for the tricky bit - decoding the data.
					//
					// The decoding scheme is this:
					// Starting with a string of zeros, then alternating ones and zeros read string
					// lengths encoded as:
					// ZERO strings:
					//  length of string   Encoding
					//      1-8	    1xyz	xyz=n-1 in binary
					//      9-16	    01xyz       xyz=n-9 in binary (1xyz=n-1)
					//     17-32	    001wxyz    wxyz=n-17 in binary (1wxyz=n-1)
					//  etc to:
					//     64K-1	    0000 0000 0000 0111 1111 1111 1111 0
					// BUT 64K-1 no alternation:
					// 		    0000 0000 0000 0111 1111 1111 1111 1
					// This last is used to break up long strings so that we can use 16 bit counts
					//
					// ONE strings:
					//  length of string   Encoding
					//      1		     0
					//      2		     10
					//      3		     110
					//  etc where the 0 flags the end of the string
					// NOTE that there is no theoretical limit to the lengths of strings encountered
					//
					// Lastly, convert each line except the top one to the XOR of itself and the
					// line above.

					uint32_t formSize = formWidth * formHeight;
					uint16_t bp = 0; // Note: I added this initialisation on principle (non essential)
					uint32_t originalFormSize = formSize;
					auto originalDest = destData;
					uint32_t eax = 0; // Note: I added this initialisation on principle (non essential)

					++formSize;     // Must start with count+1

					uint32_t mask = 0;  // Select current bit which is the bit before the first word, as we have an imagined zero there (See HUFFMAN.A86)
					uint8_t  currBit = 1;

					void *destEnd = destData; // the end-pointer
					Inc( destEnd, formSize );

					uint8_t currentByte = 0;

					for(;;) // read 0s
					{
						uint16_t countM1 = 0xFFFF; // set (count-1)=-1

						// Count a string of zero bits which leads zeros string:
						do
						{
							mask >>= 1;
							if( mask == 0 ) // Any bits left unscanned in src word?
							{
								// Get next word of source while scanning leading zeros
								eax = FetchWord( sourceData );
								Inc( sourceData, 2 );
								mask = 0x8000;  // and set 16 bits still to be scanned
							}
							++countM1;
						}
						while ( (eax & mask) == 0 ); // Any more zeroes?

						// At this point CountM1 = count - 1

						bp = 1; // Preload the 1 bit implicit for n>8. Is string length going to be > 8 ?
						if( countM1 == 0 )
						{
							// No.
							// So there were 1 to 8 zeros and we must get 3 bit zero_count
							countM1 = 1; // Pretend we need 3 bits (we do!)
							bp = 0; // but we don't want the leading 1
						}

						countM1 += 2; // We need 2 bits more than CountM1

						do
						{
							mask >>= 1;
							if( mask == 0 )
							{
								// Get next word of source while scanning zeros
								eax = FetchWord( sourceData );
								Inc( sourceData, 2 );
								mask = 0x8000;  // And set 16 bits still to be scanned
							}

							// Get next bit into BP:
							bp <<= 1;
							if( eax & mask ) bp |= 1;
							--countM1;  // Count bits
						}
						while( countM1 != 0 );

						++bp;     // (count-1)+1
						countM1 = bp;  // Get it back in CountM1

						uint16_t savedCountM1 = countM1; // Save count of zeros in case 64K-1
						if( bp == 0 ) --countM1; // Treat 64K-1 case: Decrement count because actually it was 64K-1, not 64K as stated in the file.

						// Do whole bytes, if any:
						while( countM1 > 7 )
						{
							StoreByte( destData, currentByte );
							Inc( destData, 1 ) ; // add byte count to pointer
							currentByte = 0;
							if( destData >= destEnd ) goto huffDone;
							countM1 -= 8;
						}

						// Do remaining bits:
						while( countM1 )
						{
							if( currBit & 1 )
							{
								StoreByte( destData, currentByte );
								Inc( destData, 1 ); // Yes.
								currentByte = 0;
								if( destData >= destEnd ) goto huffDone;
							}
							currBit >>= 1;
							if( currBit == 0 ) currBit = 0x80;
							--countM1;
						}

						countM1 = savedCountM1; // Get back zero count.
						if( countM1 != 0 )
						{
							// Non-Zero? (i.e. zero meant 64K) no more zeroes to read:

							do // read 1s
							{
								mask >>= 1;
								if( mask == 0 )
								{
									// Get next word of source while scanning ones
									eax = FetchWord( sourceData );
									Inc( sourceData, 2 );
									mask = 0x8000;  // And set 16 bits still to be scanned
								}
								currentByte |= currBit; // Set bit in font

								if( currBit & 1 )
								{
									StoreByte( destData, currentByte );
									Inc( destData, 1 );
									currentByte = 0;
									if( destData >= destEnd ) goto huffDone;
								}
								currBit >>= 1;
								if( currBit == 0 ) currBit = 0x80;
							}
							while( eax & mask );
						}

					} // read 0s loop continues

				huffDone:

					// Undo the XOR:
					// Read from the upper row, write to the lower row:

					{
						uint32_t widthOfOneLine = formWidth;

						auto upperRow = originalDest;
						auto lowerRow = originalDest;
						Inc( lowerRow, widthOfOneLine );

						auto upperRowEnd = upperRow;
						Inc( upperRowEnd, originalFormSize - widthOfOneLine );
						while( upperRow != upperRowEnd )
						{
							StoreWord( lowerRow, FetchWord(upperRow) ^ FetchWord(lowerRow) );
							Inc( lowerRow, 2 );
							Inc( upperRow, 2 );
						}
					}
				}


			} /// end namespace

		} /// end namespace

	} /// end namespace

} /// end namespace












// TODO: Terminology "separator" should be "terminator"

namespace lib80GFX
{
	namespace Fonts
	{
        namespace Internal
        {
            class TgaFontScanner
            {
            public:

                TgaFontScanner(
                    const uint8_t *rowPtr,
                    int32_t  rowDelta,
                    int32_t  imageWidthPixels,
                    int32_t  imageHeightPixels,
                    uint32_t bitsPerPixel );

                uint32_t PixelAt( int32_t x, int32_t y ) const;

                uint32_t GetSeparatorColour() const;
                uint32_t GetTransparencyColour() const;
                bool IsRecolouredFont() const;

                bool IsSeparatorColumn(   int32_t x ) const;
                bool IsTransparentColumn( int32_t x ) const;

                int32_t SeekTopMost   ( int32_t x, uint32_t expectedBackgroundColour ) const;
                int32_t SeekBottomMost( int32_t x ) const;

                int32_t ScanCharacter( int32_t &x, TgaFontGlyph &thisGlyph, int32_t baseLineOffset );

            private:

                bool IsColumn( const int32_t x, const uint32_t colour ) const;
                const uint8_t *PixelAddress( int32_t x, int32_t y ) const;  // We do not assume the address within the TGA file is aligned.

                const uint8_t *_imageData;

                int32_t  _imageDelta;
                int32_t  _imageWidth;
                int32_t  _imageHeight;
                uint32_t _separatorColour;
                uint32_t _transparencyColour;
                bool     _recolouredFont;
                uint32_t _bitsPerPixel;

            };



            TgaFontScanner::TgaFontScanner(
                const uint8_t *imageData,
                int32_t        imageDelta,
                int32_t        imageWidth,
                int32_t        imageHeight,
                uint32_t       bitsPerPixel )
            {
                _imageData          = imageData;
                _imageDelta         = imageDelta;
                _imageWidth         = imageWidth;
                _imageHeight        = imageHeight;
                _bitsPerPixel       = bitsPerPixel;

                _recolouredFont     = IsRecolouredFont();

                if( imageHeight >= 3 )
                {
                    _separatorColour    = GetSeparatorColour();
                    _transparencyColour = GetTransparencyColour();
                }
                else
                {
                    assert(false);  // Should not get here.
                    _separatorColour    = 0;
                    _transparencyColour = 0;
                }
            }



            const uint8_t *TgaFontScanner::PixelAddress( int32_t x, int32_t y ) const
            {
                assert( x >= 0 && x < _imageWidth && y >= 0 && y < _imageHeight );
                if( _bitsPerPixel == 8 )
                {
                    return _imageData + (y * _imageDelta) + x;
                }
                else if( _bitsPerPixel == 24 )
                {
                    return _imageData + (y * _imageDelta) + x * 3;
                }
                else if( _bitsPerPixel == 32 )
                {
                    return _imageData + (y * _imageDelta) + x * 4;
                }
                else return nullptr;
            }



            uint32_t TgaFontScanner::PixelAt( int32_t x, int32_t y ) const
            {
                // We do not assume the address within the TGA file is aligned.
                auto p = PixelAddress(x,y);
                if( _bitsPerPixel == 8 )
                {
                    return *p;  // we don't return the RGB, we can get away with just returning the index of the colour.
                }
                else if( _bitsPerPixel == 24 || _bitsPerPixel == 32 )
                {
                    return (p[2] << 16) | (p[1] << 8) | p[0];
                }
                return 0x000000;
            }



            bool TgaFontScanner::IsRecolouredFont() const
            {
                return _bitsPerPixel == 8;
            }



            uint32_t TgaFontScanner::GetSeparatorColour() const
            {
                return PixelAt(0,0);
            }



            uint32_t TgaFontScanner::GetTransparencyColour() const
            {
                return PixelAt(0,2);
            }



            bool TgaFontScanner::IsColumn( int32_t x, uint32_t colour ) const
            {
                // Scans a column (at position 'x') of the source TGA bitmap to
                // find whether the column contains pixels of the same colour 'colour'.
                //
                // Returns 1 if so, 0 if not.

                assert( x >= 0 && x < _imageWidth );

                auto h = _imageHeight;

                for( int32_t y = 0; y < h; ++y )
                {
                    if( PixelAt( x, y ) != colour ) return false;
                }

                return true;
            }



            bool TgaFontScanner::IsSeparatorColumn( int32_t x ) const
            {
                return IsColumn( x, _separatorColour );
            }



            bool TgaFontScanner::IsTransparentColumn( int32_t x ) const
            {
                return IsColumn( x, _transparencyColour );
            }



            int32_t TgaFontScanner::SeekTopMost( int32_t x, uint32_t expectedBackgroundColour ) const
            {
               auto h = _imageHeight;
               for( int32_t y = 0; y < h; ++y )
               {
                  if( PixelAt(x, y) != expectedBackgroundColour ) return y;
               }
               return -1;
            }



            int32_t TgaFontScanner::SeekBottomMost( int32_t x ) const
            {
                // returns -1 if not found
                // else returns offset (from top) of bottommost occurrence

                int32_t y = _imageHeight;
                while( y != 0 )
                {
                    --y;
                    if( PixelAt(x, y) != _transparencyColour ) return y;
                }
                return -1;
            }




            int32_t TgaFontScanner::ScanCharacter(
               int32_t &x,
               TgaFontGlyph &thisGlyph,
               int32_t baseLineOffset)
            {
                // On entry, x is the position of the leftmost column in the character.
                //
                // If the character is successfully scanned, data will be returned
                // at 'thisGlyph', and 1 is returned.
                //
                // If the character could not be scanned, 0 is returned (error -- abort)

                assert( x >= 3 );
                assert( x < _imageWidth );

                //
                // Determine TotalCursorDelta:
                //

                {
                    int32_t  a = x;
                    while( ! IsSeparatorColumn(a) )
                    {
                        ++a;
                    }
                    thisGlyph.TotalCursorDelta = a - x;
                }

                //
                // Determine OffsetFromLeft:
                //

                {
                    int32_t  a = x;
                    auto     e = x + thisGlyph.TotalCursorDelta;
                    while( a < e )
                    {
                        if( ! IsTransparentColumn(a) ) break;
                        ++a;
                    }
                    thisGlyph.OffsetFromLeft = a - x;
                }

                if( thisGlyph.OffsetFromLeft == thisGlyph.TotalCursorDelta )
                {
                    //
                    // This character is empty (don't store any data,
                    // just note down the cursor delta):
                    //

                    thisGlyph.OffsetFromLeft     = 0;
                    thisGlyph.OffsetFromBaseLine = 0;
                    thisGlyph.GlyphWidthPixels   = thisGlyph.TotalCursorDelta;
                    thisGlyph.GlyphHeightPixels  = 0;
                    x += thisGlyph.TotalCursorDelta + 1; // NB: +1 to advance past white boundary
                    return 1;
                }

                //
                // Determine glyph width by finding right-most transparent columns:
                //
                // NB: We KNOW the character has some data in it (hence this
                // will terminate before running off the left-side!)
                //

                thisGlyph.GlyphWidthPixels = thisGlyph.TotalCursorDelta - thisGlyph.OffsetFromLeft;

                {
                    int32_t  a = x + thisGlyph.TotalCursorDelta - 1;
                    while( IsTransparentColumn( a ) )
                    {
                        --a;
                        --thisGlyph.GlyphWidthPixels;
                    }
                }

                //
                // Determine OffsetFromBaseLine:
                //
                // We already know the offset of the base line,
                // and we do this calc by determining the offset
                // from the top, and then convert this to be relative
                // to the base line.
                //
                // NB: We KNOW the character has some data in it!
                //

                {
                    int32_t  xStart = x + thisGlyph.OffsetFromLeft;
                    int32_t  xEnd   = x + thisGlyph.OffsetFromLeft + thisGlyph.GlyphWidthPixels; // NB: not inclusive!
                    assert( xStart != xEnd );

                    int32_t  topY    = _imageHeight; // bound to decrease! (we know the character has some data!)
                    int32_t  bottomY = 0;            // bound to increase! (we know the character has some data!)

                    while( xStart != xEnd )
                    {
                        auto o = SeekTopMost( xStart, _transparencyColour );
                        auto h = SeekBottomMost( xStart );
                        if( o != -1  &&  o < topY )    topY    = o;
                        if( h != -1  &&  h > bottomY ) bottomY = h;
                        ++xStart;
                    }

                    //
                    // Determine the height & base line offset
                    // from the extents determined above:
                    //

                    thisGlyph.GlyphHeightPixels  = (bottomY - topY) + 1;
                    thisGlyph.OffsetFromBaseLine = baseLineOffset - topY;

                    //
                    // Extract the glyph:
                    //

                    auto px    = x + thisGlyph.OffsetFromLeft;
                    auto pxEnd = px + thisGlyph.GlyphWidthPixels;

                    auto py    = topY;
                    auto pyEnd = topY + thisGlyph.GlyphHeightPixels;

                    while( py < pyEnd )
                    {
                        px = x + thisGlyph.OffsetFromLeft;

                        if( _recolouredFont )
                        {
                            uint8_t mask = 0x80;
                            uint32_t monoAccumulator = 0;
                            while( px < pxEnd )
                            {
                                auto thisPixelColour = PixelAt( px, py );
                                if( thisPixelColour != _transparencyColour) monoAccumulator |= mask;
                                mask >>= 1;
                                if( mask == 0 )
                                {
                                    thisGlyph.GlyphData8.push_back( monoAccumulator );
                                    mask = 0x80;
                                    monoAccumulator = 0;
                                }
                                ++px;
                            }

                            if( mask != 0x80 )
                            {
                                thisGlyph.GlyphData8.push_back( monoAccumulator );
                            }
                        }
                        else // Not recoloured font.  Font is full colour with colour-key transparency.
                        {
                            while( px < pxEnd )
                            {
                                thisGlyph.GlyphData32.push_back( PixelAt( px, py ) );
                                ++px;
                            }
                        }

                        ++py;
                    }
                }

                //
                // Success! Update caller's x value and return:
                //

                x += thisGlyph.TotalCursorDelta + 1; // NB: +1 to advance past separator boundary
                return 1;
            }

        } /// end namespace

    } /// end namespace

} /// end namespace









namespace lib80GFX
{
	namespace Fonts
	{
        namespace Internal
        {
            TgaFontGlyph::TgaFontGlyph()
                : OffsetFromLeft(0)
                , OffsetFromBaseLine(0)
                , GlyphWidthPixels(0)
                , GlyphHeightPixels(0)
                , TotalCursorDelta(0)
            {
            }

        } /// end namespace

    } /// end namespace

} /// end namespace























namespace lib80GFX
{
	namespace Fonts
	{
		Fixed8byNFont::Fixed8byNFont( int32_t pointSizeTenths, const uint8_t *fontData, uint32_t scanLines, uint32_t lowestChar, uint32_t highestChar, int32_t baseLineOffset )
		{
			_fontData    = fontData;
			_scanLines   = scanLines;
			_lowestChar  = lowestChar;
			_highestChar = highestChar;
			_baseLineOffset = baseLineOffset;
		}

		bool Fixed8byNFont::OnFirstInit( Internal::DrawInfo *out_drawInfo )
		{
			int32_t numChars = (_highestChar - _lowestChar) + 1;
            auto monoBitmap = std::make_shared<Bitmaps::Mono>( const_cast<uint8_t *>(_fontData), 8, _scanLines * numChars, 1 );
			out_drawInfo->DeltaFromBaseLineY   = _baseLineOffset;
			out_drawInfo->PostIncrementX       = 8;
			out_drawInfo->WidthPixels          = 8;
			out_drawInfo->HeightPixels         = _scanLines;
			out_drawInfo->AreaOnSource.left    = 0;
			out_drawInfo->AreaOnSource.right   = 8;
			out_drawInfo->pTargetDevice->SelectBitmap( monoBitmap ); // Do this one at start.
			return true;
		}

		bool Fixed8byNFont::OnGetChar( uint32_t charCode, Internal::DrawInfo *out_drawInfo )
		{
			if( charCode >= _lowestChar && charCode <= _highestChar )
			{
				charCode -= _lowestChar;
				int32_t y = charCode * _scanLines;
				out_drawInfo->AreaOnSource.top    = y;
				out_drawInfo->AreaOnSource.bottom = y + _scanLines;
				return true;
			}
			return false; // no bitmap for this CharCode
		}

    } // end namespace

} // end namespace






namespace lib80GFX
{
	namespace Fonts
	{
		GemFont::GemFont( std::shared_ptr<Internal::GemFont> gemFontFileImage )
		{
			_gemFontFileImage = nullptr;
			_characterTable   = nullptr;
			if( gemFontFileImage != nullptr )
			{
				if( gemFontFileImage->Flags & Internal::GemFontFlags::Compressed ) return; // caller must decompress first
				_gemFontFileImage = gemFontFileImage;
				_characterTable = reinterpret_cast<uint16_t *>(
					reinterpret_cast<uint8_t *>(gemFontFileImage.get()) + gemFontFileImage->PtrCharOffsetTable );
			}
		}

		bool GemFont::OnFirstInit( Internal::DrawInfo *out_drawInfo )
		{
            auto monoBitmap = std::make_shared<Bitmaps::Mono>(
                reinterpret_cast<uint8_t *>(_gemFontFileImage.get()) + _gemFontFileImage->PtrFontData,
                _gemFontFileImage->FormWidth * 8,
                _gemFontFileImage->FormHeight,
                _gemFontFileImage->FormWidth);
			out_drawInfo->HeightPixels           = _gemFontFileImage->FormHeight;
			out_drawInfo->DeltaFromBaseLineY     = _gemFontFileImage->AscentLineDistance + _gemFontFileImage->BottomLineDistance; // ** FIX: Not happy about what the adjustment should be here
			out_drawInfo->pTargetDevice->SelectBitmap( monoBitmap ); // Do this ONCE for GEM fonts.
			out_drawInfo->AreaOnSource.top       = 0;
			out_drawInfo->AreaOnSource.bottom    = _gemFontFileImage->FormHeight;
			return true;
		}

		bool GemFont::OnGetChar( uint32_t charCode, Internal::DrawInfo *out_drawInfo )
		{
			// ** FIX: This is probably not 100% compliant with the DR interpretation.
			//         For instance, we ignore the horizontal offsets table.

			if( charCode <= _gemFontFileImage->HighestADE &&
				charCode >= _gemFontFileImage->LowestADE )
			{
				charCode -= _gemFontFileImage->LowestADE;
				int32_t  x = _characterTable[charCode];
				int32_t  w = _characterTable[charCode+1] - x;
				out_drawInfo->PostIncrementX     = w;
				out_drawInfo->WidthPixels        = w;
				out_drawInfo->AreaOnSource.left  = x;
				out_drawInfo->AreaOnSource.right = x + w;
				return true;
			}

			return false; // no bitmap for this character code
		}

    } // end namespace

} // end namespace







namespace lib80GFX
{
	namespace Fonts
	{
        TgaFont::TgaFont()
        {
           _firstChar           = 0;
           _lastChar            = 0;
           _baseLineOffset      = 0;
           _largestHeightOffset = 0;
        }



        TgaFont::~TgaFont()
        {
        }



        bool TgaFont::OnFirstInit( Internal::DrawInfo *out_drawInfo )
        {
            // Initialise fields common to all characters.
            out_drawInfo->AreaOnSource.left = 0;
            out_drawInfo->AreaOnSource.top  = 0;
			return true;
        }



        bool TgaFont::OnGetChar( uint32_t charCode, Internal::DrawInfo *out_drawInfo )
        {
            // Initialise fields specific to this character.
            auto thisGlyph = GetGlyph( charCode );
            if( thisGlyph != nullptr )
            {
                out_drawInfo->DeltaFromBaseLineY  = thisGlyph->GetOffsetFromBaseLine();
                out_drawInfo->DeltaFromLeftSide   = thisGlyph->GetOffsetFromLeft();
                out_drawInfo->PostIncrementX      = thisGlyph->GetTotalCursorDelta();
                out_drawInfo->WidthPixels         = thisGlyph->GetGlyphWidthPixels();
                out_drawInfo->HeightPixels        = thisGlyph->GetGlyphHeightPixels();
                out_drawInfo->AreaOnSource.right  = thisGlyph->GetGlyphWidthPixels();
				out_drawInfo->AreaOnSource.bottom = thisGlyph->GetGlyphHeightPixels();
				if( _recolouredFont )
				{
                    out_drawInfo->pTargetDevice->SelectBitmap( thisGlyph->MonoBitmap );
                }
                else
                {
                    out_drawInfo->pTargetDevice->SelectBitmap( thisGlyph->ColourBitmap );
                }
				return true;
			}

			return false; // no bitmap for this charCode
        }



        bool TgaFont::CreateFontFromTgaFileImage(
           const uint8_t *sourceTgaBitmapFile,
           uint32_t       sourceTgaBitmapFileSize)
        {
            // This is the main entry point for the TGA-to-font
            // conversion routine. Load a 256-colour TGA file into
            // a memory buffer, and pass it to this routine.

            assert( _glyphImages.empty() ); // should never be called more than once

            //
            // TGA file image size-check:
            // NB: TGA header is 18 bytes long.
            //

            if( sourceTgaBitmapFileSize < 18 ) return false;

            //
            // Identify file content:
            //

            auto fileData = sourceTgaBitmapFile;

            uint32_t  identificationFieldLength = fileData[0];
            uint32_t  colourMapType             = fileData[1];
            uint32_t  imageTypeCode             = fileData[2];
            // uint32_t  colourMapBaseIndex        = uint32_t(fileData[ 4] << 8) + fileData[3]; // not needed here
            uint32_t  colourMapEntries          = uint32_t(fileData[ 6] << 8) + fileData[5];
            uint32_t  colourMapEntrySizeBits    = fileData[7];
            int32_t   imageWidth                = uint32_t(fileData[13] << 8) + fileData[12];
            int32_t   imageHeight               = uint32_t(fileData[15] << 8) + fileData[14];
            int32_t   bitsPerPixel              = fileData[16];
            uint32_t  imageDescriptor           = fileData[17];
            uint32_t  imageNeedsFlipping        = (imageDescriptor & 0x20) == 0;

            //
            // Validate it's the right sort of bitmap:
            //

            uint32_t requiredFileSize = 18;
            int32_t  rowDelta = 0;
            auto rowPtr = fileData + 18 + identificationFieldLength;

            if( imageWidth <= 3 || imageHeight <= 3 ) return false;

            if( bitsPerPixel == 8 )
            {
                if( colourMapType != 1 || imageTypeCode != 1 ) return false;
                if( colourMapEntrySizeBits != 24 && colourMapEntrySizeBits != 32) return false;
                auto colourMapEntrySizeBytes = colourMapEntrySizeBits / 8;
                auto colourTableSize = (colourMapEntries * colourMapEntrySizeBytes);
                requiredFileSize += identificationFieldLength + colourTableSize + (imageWidth * imageHeight);
                rowDelta = imageWidth;
                rowPtr += colourTableSize;
                if( imageNeedsFlipping )
                {
                    rowPtr += (imageWidth * (imageHeight - 1));
                }
            }
            else if( bitsPerPixel == 24 )
            {
                if( colourMapType != 0 || imageTypeCode != 2 ) return false;
                requiredFileSize += identificationFieldLength + (imageWidth * imageHeight * 3);
                rowDelta = imageWidth * 3;
                if( imageNeedsFlipping )
                {
                    rowPtr += (imageWidth * (imageHeight - 1) * 3);
                }
            }
            else if( bitsPerPixel == 32 )
            {
                if( colourMapType != 0 || imageTypeCode != 2 ) return false;
                requiredFileSize += identificationFieldLength + (imageWidth * imageHeight * 4);
                rowDelta = imageWidth * 4;
                if( imageNeedsFlipping )
                {
                    rowPtr += (imageWidth * (imageHeight - 1) * 4);
                }
            }
            else return false;

            //
            // Validate the length of the TGA file image:
            //

            if( sourceTgaBitmapFileSize < requiredFileSize ) return false;

            if( imageNeedsFlipping )
            {
                rowDelta = -imageWidth;
            }

            //
            // Prepare the font scanner:
            //

            auto fs = Internal::TgaFontScanner( rowPtr, rowDelta, imageWidth, imageHeight, bitsPerPixel );
            _recolouredFont = fs.IsRecolouredFont();

            //
            // Check the colours are unique:
            //

            auto separatorColour = fs.GetSeparatorColour();
            auto transparencyColour = fs.GetTransparencyColour();

            if( separatorColour == transparencyColour ) return false;

            //
            // Ensure the bitmap has a white column at the right-hand end:
            // (This ensures it is 'terminated'):
            //

            if( ! fs.IsSeparatorColumn(imageWidth - 1) ) return false;

            //
            // Determine base line offset:
            //

            _baseLineOffset = fs.SeekTopMost( 1, separatorColour );
            if( _baseLineOffset == -1 ) return false;

            //
            // Determine _largestHeightOffset:
            //

            _largestHeightOffset = 0;
            {
                for( int32_t x = 3; x < imageWidth; x++ )
                {
                    auto h = fs.SeekBottomMost( x );
                    if( h != -1 )
                    {
                        if( h > _largestHeightOffset )
                        {
                            _largestHeightOffset = h;

                            //
                            // Just a speed optimisation here: We break if we
                            // find a character that occupies the bottommost pixel
                            // row. (No point in checking the rest!)
                            //

                            if( _largestHeightOffset == (imageHeight - 1) ) break;
                        }
                    }
                }
            }

            //
            // Character grabbing loop:
            //

            int32_t  x = 3;
            while( x < imageWidth )
            {
                Internal::TgaFontGlyph  thisGlyph;
                if( ! fs.ScanCharacter( x, thisGlyph, _baseLineOffset ) )
                {
                    return false; // failed to scan
                }
                _glyphImages.push_back( thisGlyph );
            }

            //
            // Create bitmaps without fear of vector re-allocation spoiling addresses:
            //

            for( auto &thisGlyph : _glyphImages )
            {
                //
                // Create an appropriate bitmap object for selecting into DC:
                //

                if( _recolouredFont )
                {
                    thisGlyph.MonoBitmap = std::make_shared<Bitmaps::Mono>(
                        &(*thisGlyph.GlyphData8.begin()),
                        thisGlyph.GlyphWidthPixels,
                        thisGlyph.GlyphHeightPixels,
                        (thisGlyph.GlyphWidthPixels + 7) / 8 );
                }
                else
                {
                    thisGlyph.ColourBitmap = std::make_shared<Bitmaps::Colour>(
                        &(*thisGlyph.GlyphData32.begin()),
                        thisGlyph.GlyphWidthPixels,
                        thisGlyph.GlyphHeightPixels,
                        thisGlyph.GlyphWidthPixels * 4 );
                }
            }

            //
            // Determine the range of characters from the total:
            //

            auto charCount = _glyphImages.size();

            if( charCount == 0 )
            {
                return false;
            }
            else if( charCount == 37 )
            {
               _firstChar = 0;   // The lookup routine handles this case.
               _lastChar  = 36;  // The lookup routine handles this case.
            }
            else if( charCount <= 224 )
            {
               _firstChar = 32;
               _lastChar  = 32 + (charCount - 1);
            }
            else
            {
               _firstChar = 0;
               _lastChar  = charCount - 1;
            }

            //
            // Success
            //

            return true;
        }



        const Internal::TgaFontGlyph *TgaFont::GetGlyph( uint32_t ch ) const
        {
            if( _glyphImages.size() == 37 )
            {
                // GAMING FONT

                if( ch == ' ' )
                {
                    return &_glyphImages[ 0 ];
                }
                else if( ch >= 'A' && ch <= 'Z' )
                {
                    return &_glyphImages[ ch - ('A' - 1) ];
                }
                else if( ch >= '0' && ch <= '9' )
                {
                    return &_glyphImages[ (ch - '0') + 27 ];
                }
            }
            else if( _glyphImages.size() < 224 )
            {
                // REGULAR FONT

                if( ch >= 32 )
                {
                    auto zch = ch - 32;
                    if( zch < _glyphImages.size() )
                    {
                        return &_glyphImages[ zch ];
                    }
                }
            }
            else if( ch < _glyphImages.size() )
            {
                return &_glyphImages[ ch ];
            }

            return nullptr;
        }

    } /// end namespace

} /// end namespace
















namespace lib80GFX
{
	namespace Fonts
	{
		inline int32_t  Scale( int32_t n, int32_t mul, int32_t div )
		{
			return int32_t( (int64_t(n) * mul) / div );
		}




		void PaintByBitmaps(
			Devices::AbstractDevice &dc,
			AbstractFont &abstractFont,
			int32_t x,
			int32_t y,
			const Scaling *scalingInfo, // nullptr if not required
			const char *textString,
			size_t textStringLength )
		{
			// Paint font to target DC

			if( textString == nullptr || textStringLength == 0 ) return;

			// Give the AbstractFont opportunity to fill in those fields
			// that will remain the same for all characters in this font:
			Internal::DrawInfo info;
			info.pTargetDevice = &dc;
			if( ! abstractFont.OnFirstInit( &info ) ) return;

			// Scaling?
			Scaling  sc;  // local copy used when scaling active.

			// On entry 'y' is the *base line* position.
			if( scalingInfo != nullptr )
			{
                // We're scaling:
				sc = *scalingInfo; // take local copy (optimisation)
				if( sc.DivisorX < 1 || sc.DivisorY < 1 ) return;
			}

			// Loop through the supplied string and draw all characters:
			int32_t unscaledOfsX = 0;
			auto stringEnd = textString + textStringLength;
			while( textString != stringEnd )
			{
				uint32_t ch = *reinterpret_cast<const uint8_t*>(textString);

				// Ask the AbstractFont to fill in the fields for character 'ch':

				if( abstractFont.OnGetChar( ch, &info ) )
				{
					if( scalingInfo == nullptr )
					{
						// Optimise for 1:1 (common case)
						int32_t targetX = x + info.DeltaFromLeftSide;
						int32_t targetY = y - info.DeltaFromBaseLineY;
						auto areaOnTarget = Rect<int32_t>( targetX, targetY, targetX+info.WidthPixels, targetY+info.HeightPixels );
						dc.DrawBitmap( areaOnTarget, info.AreaOnSource, 1 );
						x += info.PostIncrementX;
					}
					else
					{
						// Scale each character into position.
						// But we maintain the cursor position in 1:1 coordinate scheme!
						int32_t targetX = x + Scale( unscaledOfsX + info.DeltaFromLeftSide, sc.MultiplierX, sc.DivisorX );
                        int32_t targetY = y - Scale( info.DeltaFromBaseLineY+1, sc.MultiplierY, sc.DivisorY ) - 1;
						int32_t sw  = Scale( info.WidthPixels,  sc.MultiplierX, sc.DivisorX );
						int32_t sh  = Scale( info.HeightPixels, sc.MultiplierY, sc.DivisorY );
						auto areaOnTarget = Rect<int32_t>( targetX, targetY, targetX+sw, targetY+sh );
						dc.DrawBitmap( areaOnTarget, info.AreaOnSource, 1 );
						unscaledOfsX += info.PostIncrementX;
					}
				}

				// Note: if OnGetChar() returned false, we do not assume to use the 'info' state
				// for this character.

				++textString;
			}
		}





		void Measure(
			int32_t x,
			int32_t y,
			AbstractFont &font,
			const char *textString,
			size_t textStringLength,
			Rect<int32_t> *out_pExtents )
		{
			// Measure a text string in a given font.
			// (0,0) is the origin for the leftmost character and the base line.
			// The extents are returned stretching to the right from this point,
			// the top will be negative and the bottom positive.

			Devices::ExtentsMeasurementDevice  dc;
			PaintByBitmaps( dc, font, x, y, 0, textString, textStringLength ); /// choosing to leave this as a direct call
			dc.GetMeasuredExtents( out_pExtents );
		}

	} /// end namespace

} /// end namespace














