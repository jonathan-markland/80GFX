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


// To be written in a libBasic-abstract style.

#pragma once

#include <stdint.h>
#include <memory>
#include <vector>


namespace lib80GFX
{
	namespace Fonts
	{
		namespace Internal
		{
			namespace GemFontFlags
			{
				enum Enum // Flags for GEM DESKTOP font header
				{
					IsDefaultSystemFont       = 1,  // bit 0 set if default system font
					UseHorizontalOffsetTables = 2,  // bit 1 set if horizontal offset tables should be used
					ByteSwap                  = 4,  // bit 2 byte-swap flag (see "Font Data")
					IsMonospaced              = 8,  // bit 3 set if mono-spaced font
					Compressed                = 32, // bit 5 is compressed
				};
			};

			struct GemFont
			{
				// GEM DESKTOP Font (bitmapped)

				uint16_t  FaceIdentifier;         //  0 -  1 : face identifier (see the Set Text Face function)
				uint16_t  SizeInPoints;           //  2 -  3 : font size in points
				char    FaceName[32];           //  4 - 35 : face name (see the inquire Face Name and Index function)

				uint16_t  LowestADE;              // 36 - 37 : lowest ADE value in the face
				uint16_t  HighestADE;             // 38 - 39 : highest ADE value in the face

				uint16_t  TopLineDistance;        // 40 - 41 : top line distance, see Note[1]
				uint16_t  AscentLineDistance;     // 42 - 43 : ascent line distance, see Note[1]
				uint16_t  HalfLineDistance;       // 44 - 45 : half line distance, see Note[1]
				uint16_t  DescentLineDistance;    // 46 - 47 : descent line distance, see Note[1]
				uint16_t  BottomLineDistance;     // 48 - 49 : bottom line distance, see Note[1]

				uint16_t  WidthOfWidestCharacter; // 50 - 51 : width of the widest character in the font
				uint16_t  WidthOfWidestCell;      // 52 - 53 : width of the widest character cell in the face

				int16_t   LeftOffset;             // 54 - 55 : left offset (see the Inquire Current Face Information function)
				int16_t   RightOffset;            // 56 - 57 : right offset (see the Inquire Current Face Information function)

				uint16_t  ThickeningSizePixels;   // 58 - 59 : thickening: the number of pixels by which to widen thickened characters
				uint16_t  UnderlineSizePixels;    // 60 - 61 : underline size: the width (in pixels) of the underline
				uint16_t  LighteningMask;         // 62 - 63 : lightening mask:   the mask used to drop pixels out when lightening; usually 5555H
				uint16_t  SkewingMask;            // 64 - 65 : skewing mask: the mask that is rotated to determine when to perform additional rotation on the character to perform skewing; usually 5555H

				uint16_t  Flags;                  // 66 - 67 : flags, see enum above

				uint32_t  PtrHorizOffsetTable;    // 68 - 71 : file-offset to the horizontal offset table
				uint32_t  PtrCharOffsetTable;     // 72 - 75 : file-offset to the character offset table

				uint32_t  PtrFontData;            // 76 - 79 : file-offset to the font data
				uint16_t  FormWidth;              // 80 - 81 : form width (see "Font Data")
				uint16_t  FormHeight;             // 82 - 83 : form height (see "Font Data")
				uint32_t  __unused_NextFont;      // 84 - 87 : Reserved (was address of next font, not 64-bit compliant)
			};
		}
	}
}


// ** FIX: Not sure what's going to happen with these (where kept, namespace name):
namespace lib80GFX
{
	namespace Fonts
	{
		namespace Internal
		{
			namespace Gem
			{
				enum { MAX_FORM_WIDTH  = 8192 }; // just making this up
				enum { MAX_FORM_HEIGHT = 256  }; // just making this up
				bool ValidateGemFontTable( uint32_t Ptr, uint32_t Size, uint64_t FileSize );
				int32_t Validate( const void *FileImage, uint64_t FileSize );
				void Decompress( void *Source, void *Dest, uint32_t FormWidth, uint32_t FormHeight );
			}
		}
	}
}

/*
- Note[1] : Distances are measured relative to the
  character baseline and are always a positive
  value (magnitude rather than offset).
*/





/*****************************************************************************

Appendix G - Font Format
_____________________________________________________________________________

INTRODUCTION

	The system fonts and external fonts used in GEM
	VDI are composed of four parts: the font data,
	a font header, a character offset table, and a
	horizontal offset table.

_____________________________________________________________________________

FONT DATA

	The font data is organized as a single raster
	area. The area's height equals the font height
	and its width equals the sum of the character
	widths.

	The top scan line of the first character in the
	font is aligned to a byte boundary. The top
	scan line of the second character is abutted to
	the first character and is not necessarily
	byte-aligned. That is, the end of any
	character and the beginning of the following
	character often occur within the same byte; no
	byte alignment occurs within the font form.

	Bit padding occurs only at the end of a scan
	line. Each scan line in the font form begins
	on a word boundary. The number of bytes from
	the beginning of one scan line to the beginning
	of the next is called the form width. The
	number of scan lines required to draw any
	character is called the form height.

	A flag within the font header indicates the
	orientation of bytes within a word in the font
	data. If the flag is cleared, the font data is
	in a format such that the low byte of a word
	occurs in memory before the high byte
	(Intel16 format). If the flag is set, the high
	byte precedes the low byte in memory.

_____________________________________________________________________________

FONT HEADER

	The font header contains information that
	describes global aspects of the font. For
	example, the name of the face, the font size,
	the minimum and maximum characters in the font,
	and any other data that applies to every
	character of the font are global aspects of
	that font. The format of the font header is
	shown in Table G-1.
_____________________________________________________________________________

CHARACTER OFFSET TABLE

	The character offset table is used to index
	into the font data and to determine the width of
	specific characters in the font. It is indexed
	by relative character value (the ADE value of
	the desired character, minus the lowest ADE
	value in the font) and yields the offset from
	the base of the font data to the beginning of
	the character definition. The difference
	between the offset to a character and the
	offset to the following character gives the
	width of the character. Note that the
	character offset table includes one more entry
	than the number of characters in the font so
	that a width may be obtained for the final
	character in the font.

	Note: The character offset table is required
	even for mono-spaced fonts.
_____________________________________________________________________________

HORIZONTAL OFFSET TABLE

	The horizontal offset table is indexed by
	relative character value and yields any
	additional positive or negative spacing
	necessary before outputting the character. The
	horizontal offset table often does not exist.
	Whether it exists or not is indicated by the
	horizontal offset table bit in the flags word
	of the font header.
_____________________________________________________________________________

End of Appendix G

*****************************************************************************/









namespace lib80GFX
{
	namespace Fonts
	{
		namespace Internal
		{
			struct DrawInfo
			{
				DrawInfo()
					: pTargetDevice( nullptr )
					, WidthPixels(0)
					, HeightPixels(0)
					, DeltaFromLeftSide(0)
					, DeltaFromBaseLineY(0)
					, PostIncrementX(0)
				{
				}

				// Set by the system before any calls.
				// The client MUST select bitmaps:
				Devices::AbstractDevice *pTargetDevice;

				// v Required after AbstractFont::OnGetChar():
				int32_t   WidthPixels;
				int32_t   HeightPixels;
				int32_t   DeltaFromLeftSide;
				int32_t   DeltaFromBaseLineY;
				int32_t   PostIncrementX;
				Rect<int32_t>  AreaOnSource;
				// ^
			};
		}



		class GemFont: public AbstractFont
		{
		public:

			// DR GEM Desktop bitmapped font.

			GemFont( std::shared_ptr<Internal::GemFont> gemFontFileImage );
			virtual bool OnFirstInit( Internal::DrawInfo *out_DrawInfo ) override;
			virtual bool OnGetChar( uint32_t CharCode, Internal::DrawInfo *out_DrawInfo ) override;

		private:
			std::shared_ptr<Internal::GemFont> _gemFontFileImage;
			uint16_t *_characterTable;
		};





		class Fixed8byNFont: public AbstractFont
		{
		public:

			// 8*n pixels monospaced bitmap font.
			// The font data is intended to be stored in statically allocated memory.
			// There is NO associated persistence format with these fonts.

			Fixed8byNFont( int32_t pointSizeTenths, uint8_t *pFont, uint32_t scanLines, uint32_t lowestChar, uint32_t highestChar, int32_t baseLineOffset );
			virtual bool OnFirstInit( Internal::DrawInfo *out_drawInfo ) override;
			virtual bool OnGetChar( uint32_t charCode, Internal::DrawInfo *out_drawInfo ) override;
		private:
			uint8_t   *_fontData;          // Usually in a statically allocated resource.
			uint32_t   _scanLines;
			uint32_t   _lowestChar;
			uint32_t   _highestChar;
			int32_t    _baseLineOffset;
		};








        class TgaFont;

        namespace Internal   // TODO: reinstate this namespace
        {
            class TgaFontScanner;

            class TgaFontGlyph
            {
            public:

                TgaFontGlyph();

                int32_t GetGlyphWidthPixels()       const { return GlyphWidthPixels; }
                int32_t GetGlyphHeightPixels()      const { return GlyphHeightPixels; }
                int32_t GetOffsetFromLeft()         const { return OffsetFromLeft; }
                int32_t GetOffsetFromBaseLine()     const { return OffsetFromBaseLine; }
                int32_t GetTotalCursorDelta()       const { return TotalCursorDelta; }

            private:

                friend TgaFont;
                friend TgaFontScanner;

                std::shared_ptr<Bitmaps::Colour> ColourBitmap;
                std::vector<uint32_t> GlyphData32;
                    // RGB0 triples in full colour fonts.

                std::shared_ptr<Bitmaps::Mono> MonoBitmap;
                std::vector<uint8_t> GlyphData8;
                    // 1-bpp representation for recoloured fonts.

                int32_t OffsetFromLeft;
                    // Offset to add to the X position before
                    // rendering the character.

                int32_t OffsetFromBaseLine;
                    // If Y represents the vertical position of the base
                    // line on which the string is written, this value
                    // is the amount to SUBTRACT from Y before rendering
                    // the character.

                int32_t GlyphWidthPixels;
                    // Width (in pixels) of the character grid.

                int32_t GlyphHeightPixels;
                    // Height (in pixels) of the character grid.

                int32_t TotalCursorDelta;
                    // Amount by which X should be incremented to step
                    // to the right position for rendering the next character.
                    // NB: Do NOT also include 'OffsetFromLeft'!

            };

        } /// end namespace





		class TgaFont: public AbstractFont
		{
		public:

            // Fonts defined by a TGA file.
            //
            // - 32bpp TGA files are supported only.  Hint: GIMP saves these.
            //
            // Image dimensions
            // The image file should be very wide, with all the characters in one row.
            // The image height should be trimmed to that necessary.
            //
            // File content
            // The first three vertical columns are reserved and have special meanings:
            //
            // Column X=0
            // The leftmost column is the colour definition column, and is not part of any glyph.
            // - Pixel (0,0) provides the colour for the columns that terminate character glyphs.
            // - Pixel (0,1) must be black RGB 0,0,0 to indicate recoloured font, or yellow (255,255,0) to indicate full colour.
            // - Pixel (0,2) provides the transparency colour within character glyphs.
            // - The colours at (0,0) and (0,2) must be DIFFERENT.
            // - The rest of this column must be the same colour as (0,0)
            //
            // Column X=1
            // This column must be entirely the same colour as (0,0) except for a spot of
            // a different colour that indicates the font base line position.
            //
            // Column X=2
            // This column must be entirely the same colour as (0,0) and is reserved.
            //
            // Column X=3
            // This column is the first column of the first character.
            //
            // Character glyph content
            // Character matrixes are rectangular, and can be irregular widths.
            // The character data just starts at the next available column, there is no leader.
            // Characters are *terminated* by a column entirely in the terminator colour (0,0).
            // The final character must be terminated.
            //
            // Colour interpretation
            // - For 24bpp and 32bpp images, the RGB values in the character glyph are literally
            //   used in the rendering.  Except pixels that match (0,1) are rendered transparent.
            // - For 8bpp images, pixels matching the colour at (0,1) are rendered transparent,
            //   and all other colour values are replaced with the currently selected foreground
            //   colour, when the font is rendered.
            // - It is not an error to use the terminator colour in the glyph, but obviously an
            //   entire column in the terminator colour will be interpreted as a terminator!
            //
            // Character set interpretation
            // - If the file contains 224+ terminators, then the font is assumed to define
            //   characters starting from 0 onwards.
            // - If the file contains 37 terminators, then the font is assumed to define
            //   characters: SPACE A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 0 1 2 3 4 5 6 7 8 9
            // - If the file contains < 224 characters, the first character is taken to define
            //   the space character (32) onwards.
            // - Any other number is an invalid file.

            TgaFont();
            ~TgaFont();

            bool CreateFontFromTgaFileImage( const uint8_t *sourceTgaBitmapFile, uint32_t sourceTgaBitmapFileSize );
                // The following is an interface function used
                // by the Font Manager only, and tells the TgaFont
                // to try to establish itself from the given 8-bpp TGA file.
                // Note: This can only be called ONCE.

			virtual bool OnFirstInit( Internal::DrawInfo *out_DrawInfo ) override;
			virtual bool OnGetChar( uint32_t CharCode, Internal::DrawInfo *out_DrawInfo ) override;

            const Internal::TgaFontGlyph *GetGlyph( uint32_t characterCode ) const;
                // The following is used by clients wishing to directly
                // render this bitmapped font.
                // Returns nullptr if the characterCode is outside the range of the font.

        private:

            bool _recolouredFont;
                // True if the font is recoloured at rendering time.  The pixels are either the foreground colour, or transparent.
                // False if the font is full-colour (32-bpp RGBX).

            uint32_t _firstChar;
            uint32_t _lastChar;
                // The number of the first / last character in the font.

            int32_t _baseLineOffset;
                // This is to support quick-rejection calculations
                // by allowing bounding box to be computed.
                // If Y represents the vertical position of the base
                // line on which the string is written: Take Y,
                // subtract this value, and this gives the topmost Y
                // that will be affected by rendering any string in
                // this font.

            int32_t _largestHeightOffset;
                // This is to support quick-rejection calculations
                // by allowing bounding box to be computed.
                // If Y represents the vertical position of the base
                // line on which the string is written: Take Y,
                // subtract BaseLineOffset, then add LargestHeightOffset.
                // This gives the bottommost Y value that will be
                // affected by rendering any string in this font.

            std::vector<Internal::TgaFontGlyph> _glyphImages;
                // All extracted glyphs.
                // Indexing is based at _firstChar, extending through to _lastChar.

		};


	}
}
