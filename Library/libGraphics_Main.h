
// libGraphics_Main.h -- to be written in a libBasic-abstract style.

#pragma once

#include <memory>
#include <algorithm>        // for std::sort()

#include "Maths.h"
#include "Geometric.h"
#include "SineTable.h"
#include "Array1D2D.h"
#include "Abstractions.h"   // for libBasic::AbstractTextOutputStream (for metafiles)
#include "SmallStringBuilder.h"
#include "Resources_Pattern1616.h"

extern const int32_t BezierConstants[15*4];





namespace libGraphics
{
	namespace System
	{
		namespace Raster
		{
			// Return the address of a pixel within an arbitrary-type frame buffer.
			template<typename PIXEL>
			inline PIXEL *template_CalcPixelAddress( PIXEL *destination, int32_t x, int32_t y, intptr_t destScanLineSizeBytes )
			{
				return reinterpret_cast<PIXEL *>( reinterpret_cast<intptr_t>(destination) + (y * destScanLineSizeBytes) ) + x;
			}

		}
	}
}




// - - - POINT PLOTTER -> BITMAP - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		namespace PointReceivers
		{

			template<typename PIXEL, typename SCALAR>
			class Plot
			{
			public:

				// This is a POINT_RECEIVER

				Plot();
				void SetUncheckedTarget( PIXEL *destination, intptr_t destinationBytesPerLine )        { _destination = destination; _destinationBytesPerLine = destinationBytesPerLine; }
				void SetUncheckedViewport( Rect<SCALAR> r )                                            { _viewport = r; }
				void SetColour( PIXEL colour )                                                         { _colour = colour; }
				Rect<SCALAR>  GetViewport() const                                                      { return _viewport; }
				void operator()( SCALAR x, SCALAR y ); // NB: This *IS* checked!

			private:

				PIXEL        *_destination;
				intptr_t      _destinationBytesPerLine;
				PIXEL         _colour;
				Rect<SCALAR>  _viewport;

			};

			template<typename PIXEL, typename SCALAR>
			Plot<PIXEL,SCALAR>::Plot()
					: _destination(nullptr)
					, _destinationBytesPerLine(0)
					, _colour(0)
					, _viewport(Rect<SCALAR>())
			{
			}

			template<typename PIXEL, typename SCALAR>
			void Plot<PIXEL,SCALAR>::operator()( SCALAR x, SCALAR y )
			{
				if( x >= _viewport.left && x < _viewport.right &&
					y >= _viewport.top  && y < _viewport.bottom )
				{
					assert( _destination != 0 );
					assert( _destinationBytesPerLine != 0 );
					*System::Raster::template_CalcPixelAddress( _destination, x, y, _destinationBytesPerLine ) = _colour;
				}
			}

		} /// end namespace

	} /// end namespace

} /// end namespace







namespace libGraphics
{
	namespace System
	{
		namespace Raster
		{
			// - - - BRUSHES - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			template<typename PIXEL>
			class SolidBrush
			{
			public:

				// A brush that represents a solid-colour fill.

				SolidBrush()                          : Colour(0) {}
				explicit SolidBrush( PIXEL Colour )   : Colour(Colour) {}

				PIXEL  Colour;
			};

			template<typename PIXEL, size_t MASK, size_t SHIFT>
			class AverageMixBrush
			{
			public:

				// A brush that represents averaging with a solid-colour.

				AverageMixBrush()                          : Colour(0) {}
				explicit AverageMixBrush( PIXEL Colour )   : Colour(Colour) {}

				PIXEL  Colour;
			};

			template<typename PIXEL>
			class PatternBrush
			{
			public:

				// A brush that paints a repeat 16*16 tiled pattern.
				// The pattern is anchored at the origin if the offsets are zero.

				PatternBrush();
				explicit PatternBrush( const uint16_t *pattern );
				PatternBrush( const uint16_t *pattern, PIXEL foreColour );
				PatternBrush( const uint16_t *pattern, PIXEL foreColour, PIXEL backColour );

				const uint16_t *Pattern;   // Base address of 16*16 pattern (An array: "uint16_t Pattern[16]")
				PIXEL   ForeColour;      // Colour for 1s
				PIXEL   BackColour;      // Colour for 0s
				int8_t  PatternOffsetX;  // Start offset into pattern (horizontal)
				int8_t  PatternOffsetY;  // Start offset into pattern (vertical)
				bool    ForeTransparent; // Is the foreground to be transparent?
				bool    BackTransparent; // Is the background to be transparent?
			};

			template<typename PIXEL>
			PatternBrush<PIXEL>::PatternBrush()
				: Pattern(g_Pattern1616_FilledDiamond) // The default pattern
	// TODO: Colour representation assumption in template for type 'PIXEL'
				, ForeColour(0x000000)      // '1's will be black
				, BackColour(0xFFFFFF)      // '0's will be white
				, PatternOffsetX(0)         // Start offset into pattern (horizontal)
				, PatternOffsetY(0)         // Start offset into pattern (vertical)
				, ForeTransparent(false)    // Foreground opaque
				, BackTransparent(false)    // Background opaque
			{
			}

			template<typename PIXEL>
			PatternBrush<PIXEL>::PatternBrush( const uint16_t *pattern )
				: Pattern(pattern)          // Base address of 16*16 pattern (An array: "uint16_t Pattern[16]")
	// TODO: Colour representation assumption in template for type 'PIXEL'
				, ForeColour(0x000000)      // '1's will be black
				, BackColour(0xFFFFFF)      // '0's will be white
				, PatternOffsetX(0)         // Start offset into pattern (horizontal)
				, PatternOffsetY(0)         // Start offset into pattern (vertical)
				, ForeTransparent(false)    // Foreground opaque
				, BackTransparent(false)    // Background opaque
			{
			}

			template<typename PIXEL>
			PatternBrush<PIXEL>::PatternBrush( const uint16_t *pattern, PIXEL foreColour )
				: Pattern(pattern)          // Base address of 16*16 pattern
				, ForeColour(foreColour)    // Colour for '1's
				, BackColour(0)             // Background will be transparent (see below)
				, PatternOffsetX(0)         // Start offset into pattern (horizontal)
				, PatternOffsetY(0)         // Start offset into pattern (vertical)
				, ForeTransparent(false)    // Foreground opaque
				, BackTransparent(true)     // Background transparent
			{
			}

			template<typename PIXEL>
			PatternBrush<PIXEL>::PatternBrush( const uint16_t *pattern, PIXEL foreColour, PIXEL backColour )
				: Pattern(pattern)          // Base address of 16*16 pattern
				, ForeColour(foreColour)    // Colour for '1's
				, BackColour(backColour)    // Colour for '0's
				, PatternOffsetX(0)         // Start offset into pattern (horizontal)
				, PatternOffsetY(0)         // Start offset into pattern (vertical)
				, ForeTransparent(false)    // Foreground opaque
				, BackTransparent(false)    // Backgruond transparent
			{
			}





			// - - - SINGLE-RASTER PAINTERS -> BITMAP - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			template<typename PIXEL, typename SCALAR>
			inline void PaintRasterInSolidBrush(  // TODO: unroll
				const SolidBrush<PIXEL> &solidBrush,
				SCALAR x,
				SCALAR y,
				PIXEL *destination,
				SCALAR widthPixels )
			{
				// Paint a raster with a solid brush.

				// x; // not used
				// y; // not used
				auto colour = solidBrush.Colour;
				auto endAddress = destination + widthPixels;
				while( destination < endAddress )
				{
					*destination = colour;
					++destination;
				}
			}


			template<typename PIXEL, typename SCALAR, size_t MASK, size_t SHIFT>
			inline void PaintRasterInAverageMixBrush(
				const AverageMixBrush<PIXEL,MASK,SHIFT> &averageMixBrush,
				SCALAR x,
				SCALAR y,
				PIXEL *destination,
				SCALAR widthPixels )
			{
				// Paint a raster with averaging brush.
				// x; // not used
				// y; // not used
				ArrayAverageFill( destination, averageMixBrush.Colour, PIXEL(MASK), PIXEL(SHIFT), widthPixels );
			}


			template<typename PIXEL, typename SCALAR>
			inline void PaintRasterInPatternBrush_Core(
				PIXEL    *destination,
				SCALAR    widthPixels,
				uint16_t  thisRowPattern,
				uint16_t  mask,
				PIXEL     foreColour,
				PIXEL     backColour,
				bool      foreTransparent,
				bool      backTransparent )
			{
				// Assistance function.  See template_PaintRasterInPatternBrush()

				while( widthPixels > 0 )
				{
					while( widthPixels > 0 && mask != 0 )
					{
						if( thisRowPattern & mask )
						{
							// 1
							if( ! foreTransparent )
							{
								*destination = foreColour;
							}
						}
						else
						{
							// 0
							if( ! backTransparent )
							{
								*destination = backColour;
							}
						}
						mask >>= 1;
						--widthPixels;
						++destination;
					}
					mask = 0x8000; // reset for the next repetition
				}
			}

			template<typename PIXEL, typename SCALAR>
			void PaintRasterInPatternBrush(
				const PatternBrush<PIXEL> &patternBrush,
				SCALAR x,
				SCALAR y,
				PIXEL *destination,
				SCALAR widthPixels )
			{
				// Paint a raster with a tile-patterned brush.

				uint16_t  thisRowPattern = patternBrush.Pattern[ (y + patternBrush.PatternOffsetY) & 15 ];
				uint16_t  mask = (0x8000 >> ((x + patternBrush.PatternOffsetX) & 15));

				if( ! patternBrush.ForeTransparent && ! patternBrush.BackTransparent )
				{
					// Call a specialisation for when neither are transparent.
					// The optimiser removes the transparency tests in the expansion,
					// and this should co-erce write combining.
					PaintRasterInPatternBrush_Core(
						destination, widthPixels, thisRowPattern, mask,
						patternBrush.ForeColour, patternBrush.BackColour, false, false );
				}
				else
				{
					// Call the general version for when one is transparent.
					PaintRasterInPatternBrush_Core(
						destination, widthPixels, thisRowPattern, mask,
						patternBrush.ForeColour, patternBrush.BackColour, patternBrush.ForeTransparent, patternBrush.BackTransparent );
				}
			}




			// - - - RASTER-IMAGE PAINTERS -> BITMAP - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			template<typename PIXEL, typename SCALAR, typename PAINTER, typename BRUSH>
			void PaintPolygonPointsArray(
				PIXEL     *destination,             // The address of the origin (0,0)
				intptr_t   destinationBytesPerLine, // Bytes per scan line on target bitmap
				const Point<SCALAR> *pixelsBuffer,  // The pixels buffer (sorted)
				size_t     numPoints,               // The number of elements in the PixelsBuffer array.
				PAINTER   &rasterPainter,           // A functor that does the painting.
				BRUSH     &brush )                  // A brush object for use by the Painter.
			{
				// Draw post-scan-converted polygon data on to a target using the Painter and Brush.
				// All rasters are assumed to lie within the target, and are at least 1 pixel wide.

				assert( ! (numPoints & 1) ); // NumPoints should be even
				if( numPoints & 1 ) return;

				auto pixelsBufferEnd = pixelsBuffer + numPoints;
				while( pixelsBuffer != pixelsBufferEnd )
				{
					// Fetch raster line edges and Y:
					assert( pixelsBuffer[0].y == pixelsBuffer[1].y ); // Expected to be on the same level!
					auto  left  = pixelsBuffer[0].x;
					auto  y     = pixelsBuffer[0].y; // they are both the same
					auto  right = pixelsBuffer[1].x;

					// Draw the line:
					auto address = System::Raster::template_CalcPixelAddress( destination, left, y, destinationBytesPerLine );
					auto widthPixels = right - left;
					assert( widthPixels > 0 ); // should be
					rasterPainter( brush, left, pixelsBuffer->y, address, widthPixels );

					pixelsBuffer += 2; // advance to next pair of points
				}
			}

			template<typename SCALAR>
			struct RasterLR
			{
				// A horizontal line between two points (L,Y) -> (R,Y)   (Y is supplied externally)

				// - An advantage of the RasterLR technique is that it can 'merge' overlapping
				//   rasters, so the ultimate renderer does not do overlapped work.

				SCALAR  Left;  // Leftmost pixel  | Inclusive     | (offset into bitmap from left side)
				SCALAR  Right; // Rightmost pixel | Non-inclusive | (offset into bitmap from left side)
			};

			template<typename PIXEL, typename SCALAR, typename PAINTER, typename BRUSH>
			void PaintLRArray(
				PIXEL     *destination,              // The address of the origin (0,0)
				intptr_t   destinationBytesPerLine,  // Bytes per scan line on target bitmap
				SCALAR     topY,                     // Number of rows into the bitmap at which to start.
				const RasterLR<SCALAR> *lrArray,     // Array of (LeftX,RightX) pairs, one per raster line, in order top->bottom.
				uint32_t   numEdges,                 // Number of entries in the array.
				PAINTER   &rasterPainter,            // A functor that does the painting.
				BRUSH     &brush )                   // A brush object for use by the Painter.
			{
				// Paint an image, consisting of horizontal rows, onto a bitmap.
				// Array of (LeftX,RightX) pairs specifies the start and end points on each
				// raster line of a contiguous group (measured from the left of the bitmap).

				// Advance to address of (0,y):
				reinterpret_cast<intptr_t &>(destination) += (topY * destinationBytesPerLine);

				// Paint rasters from this row downwards:
				auto lrArrayEnd = lrArray + numEdges;
				while( lrArray < lrArrayEnd )
				{
					auto destinationAddress = destination + lrArray->Left;
					auto widthPixels = lrArray->Right - lrArray->Left;
					if( widthPixels > 0 )
					{
						rasterPainter( brush, lrArray->Left, topY, destinationAddress, widthPixels );
					}
					reinterpret_cast<intptr_t &>(destination) += destinationBytesPerLine;
					++lrArray;
					++topY;
				}
			}

			template<typename PIXEL, typename SCALAR, typename PAINTER, typename BRUSH>
			void PaintFilledRectangle(
				PIXEL     *destination,             // The address of the origin (0,0)
				intptr_t   destinationBytesPerLine, // Bytes per scan line on target bitmap
				SCALAR     leftX,                   // Pixels in from the left to start.
				SCALAR     topY,                    // Number of rows into the bitmap at which to start.
				SCALAR     widthPixels,             // Width of area
				SCALAR     heightPixels,            // Height of area
				PAINTER   &rasterPainter,           // A functor that does the painting.
				BRUSH     &brush )                  // A brush object for use by the Painter.
			{
				// Paint a rectangle by calling the Action object for each raster line.
				// It is assumed that the parameters fit the target bitmap memory.

				// Advance to address of (Left,y):
				destination     = System::Raster::template_CalcPixelAddress( destination, leftX, topY, destinationBytesPerLine );
				auto endAddress = System::Raster::template_CalcPixelAddress( destination, 0, heightPixels, destinationBytesPerLine );

				// Paint rectangle of rasters from this row downwards:
				// Reminder: frame buffer may be upside down (-ve bytes per line)
				while( destination != endAddress )
				{
					rasterPainter( brush, leftX, topY, destination, widthPixels );
					reinterpret_cast<intptr_t &>(destination) += destinationBytesPerLine;
					++topY;
				}
			}


			// - - - "LEFT/RIGHTS-ARRAY" RASTER-IMAGE ROUTINES   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			template<typename SCALAR>
			void InitArray( RasterLR<SCALAR> *lrArray, size_t numArrayEntries, SCALAR widthPixels )
			{
				// Initialisation of a RasterLR array to "empty" (inside-out):

				auto arrayEnd = lrArray + numArrayEntries;
				while( lrArray < arrayEnd )
				{
					lrArray->Left  = widthPixels;
					lrArray->Right = 0;
					++lrArray;
				}
			}

			template<typename SCALAR>
			class LRCollector
			{
			public:

				// A class that assists writing to a RasterLR array.

				// Note: Array[0] corresponds to y=0
				// The vertical viewport is assumed to fit the target Array,
				// and is assumed to lie within the ultimate target horizontally.
				// Reminder: In the raster array, the right-hand side is NOT inclusive!

				LRCollector( RasterLR<SCALAR> *lrArray, size_t arraySize, Rect<SCALAR> viewport );
				void operator()( SCALAR l, SCALAR r, SCALAR y );  // the overload if you know the left & right side of the raster
				void operator()( SCALAR x, SCALAR y );            // the overload to use "raster discovery" via a min/max techinique

			private:

				RasterLR<SCALAR> *_array;

			public: // so clients can read the viewport:

				SCALAR vx0; // inclusive
				SCALAR vy0; // inclusive
				SCALAR vx1; // non-inclusive
				SCALAR vy1; // non-inclusive

			};

			template<typename SCALAR>
			LRCollector<SCALAR>::LRCollector( RasterLR<SCALAR> *lrArray, size_t arraySize, Rect<SCALAR> viewport )
				: _array(lrArray)
				, vx0(viewport.left)
				, vy0(viewport.top)
				, vx1(viewport.right)
				, vy1(viewport.bottom)
			{
				assert( vx0 <= vx1 );       // The viewport certainly cannot be inside-out, but it can be empty.
				assert( vy0 <= vy1 );       // The viewport certainly cannot be inside-out, but it can be empty.
				assert( vx0 >= 0 );         // We know this will have to be the case for the target
				// We can't validate vx1 because we don't know the max width of the ultimate target.
				assert( vy0 >= 0 );         // Otherwise the viewport allows writes before the start of the Array!
				assert( size_t(vy1) <= arraySize ); // Otherwise the viewport allows writes beyond the end of the Array!
			}

			template<typename SCALAR>
			void LRCollector<SCALAR>::operator()( SCALAR left, SCALAR right, SCALAR y )
			{
				// "Draw" a horizontal line into the RasterLR array.

				// 1. Check for outside viewport:
				if( y >= vy0 && y < vy1 && left < vx1 && right >= vx0 )
				{
					// 2. Clipping to viewport:
					left  = std::max( left,  vx0 );
					right = std::min( right, vx1 );

					// 3. Store the horizontal line:
					_array[y].Left  = left;
					_array[y].Right = right;
				}
			}

			template<typename SCALAR>
			void LRCollector<SCALAR>::operator()( SCALAR x, SCALAR y )
			{
				// "Draw" a point into a Raster LR array.

				// 1. Ignore if it's outside the viewport (vertically):
				if( y >= vy0 && y < vy1 )
				{
					// 2. Clipping to viewport (horizontally):
					x = std::max( x, vx0 );
					x = std::min( x, vx1 );

					// 3. Does this parameter stretch the current raster?
					// (We MUST do both tests every time -- for the "first time ever" case)
					if( x < _array[y].Left )  _array[y].Left  = x;
					if( x > _array[y].Right ) _array[y].Right = x;
				}
			}

		} /// end namespace

	} /// end namespace

} /// end namespace





// - - - SHAPES using RasterLR<> array - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		namespace ToRasters
		{

			template<typename SCALAR>
			void BresenhamFilledEllipse(
				SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1,     // The extents area for the ellipse.
				Raster::LRCollector<SCALAR> &rr)                  // The receiver for the output rasters.  This also knows the viewport & does clipping.
			{
				// Draw an ellipse into a RasterLR array.

				// Inside-out check:
				if(x0 >= x1 || y0 >= y1) return;

				// Check for totally outside viewport:
				if(x0 >= rr.vx1 || x1 <= rr.vx0 || y0 >= rr.vy1 || y1 <= rr.vy0) return;

				// Determine centre:
				auto centerX = (x0 + x1) / 2;
				auto centerY = (y0 + y1) / 2;

				// Determine radii:
				auto radiusX = x0<x1 ? x1-centerX : x0-centerX;
				auto radiusY = y0<y1 ? y1-centerY : y0-centerY;

				// If it's a TALL ellipse, swap radii so it becomes WIDE,
				// and flag for coordinate swapping at draw-time:
				bool isWideEllipse = true;
				if(radiusX < radiusY)
				{
					auto temp = radiusX;
					radiusX = radiusY;
					radiusY = temp;
					isWideEllipse = false;
				}

				// This plots a 'wide' ellipse.
				SCALAR d       = 3 - (2 * radiusX);
				SCALAR X       = 0;         // Circle X and y0 X
				SCALAR Y       = radiusX;   // Circle Y and Side X
				SCALAR topY    = radiusY;   // y0 Y
				SCALAR sideY   = 0;         // Side Y
				SCALAR topDec  = radiusX/2; // Limit=RadiusX, IncBy=RadiusY
				SCALAR sideDec = radiusX/2; // Limit=RadiusX, IncBy=RadiusY
				SCALAR delta1  = 6;
				SCALAR delta2  = -radiusX*4 + 10;

				while(X <= Y)
				{
					// Call the callback to draw horizontal lines, reflect all four quadrants:
					// Here, we send UNCLIPPED line coordinates:
					if( isWideEllipse )
					{
						// Wide
						rr( centerX - X, centerX + X, centerY + topY  );
						rr( centerX - Y, centerX + Y, centerY + sideY );
						rr( centerX - X, centerX + X, centerY - topY  );
						rr( centerX - Y, centerX + Y, centerY - sideY );
					}
					else
					{
						// Tall
						rr( centerX - topY,  centerX + topY,  centerY + X );
						rr( centerX - sideY, centerX + sideY, centerY + Y );
						rr( centerX - topY,  centerX + topY,  centerY - X );
						rr( centerX - sideY, centerX + sideY, centerY - Y );
					}

					// Conditionally increment SideY:
					sideDec += radiusY;
					if(sideDec > radiusX)
					{
						sideY++;
						sideDec -= radiusX;
					}

					// Bresenham's circle algorithm decisions:
					if(d >= 0)
					{
						d += delta2;
						delta1 += 4;
						delta2 += 8;
						X++;
						Y--;

						// Conditionally decrement TopY:
						topDec += radiusY;
						if(topDec > radiusX)
						{
							topY--;
							topDec -= radiusX;
						}

						continue;
					}

					d += delta1;
					delta1 += 4;
					delta2 += 4;
					X++;
				}
			}

		} /// end namespace

	} /// end namespace

} /// end namespace








// - - - MISC - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		template<typename SCALAR>
		inline void CondSwapEdges( SCALAR &v0, SCALAR &v1)
		{
		   if(v0 > v1)
		   {
			  auto temp = v0;
			  v0 = v1;
			  v1 = temp;
		   }
		}

		inline int64_t  LineDirectionChanged( int64_t delta1, int64_t delta2 )
		{
			return (delta1 ^ delta2) & 0x8000000000000000;
		}

		inline int32_t  LineDirectionChanged( int32_t delta1, int32_t delta2 )
		{
			return (delta1 ^ delta2) & 0x80000000;
		}

	}
}





// - - - OUTLINE PLOTTERS - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		namespace ToPoints
		{

			template<typename SCALAR, typename POINT_RECEIVER>
			bool BresenhamEllipse(
				Rect<SCALAR> viewport,
				SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1,
				POINT_RECEIVER &plotPoint)
			{
				// Plot ellipse outline using Bresenham

				// Swap
				CondSwapEdges( x0, x1 );
				CondSwapEdges( y0, y1 );

				// Check (fast reject)
				if( x0 >= viewport.right  || x1 <= viewport.left ||
					y0 >= viewport.bottom || y1 <= viewport.top ) return false; // didn't draw anything

				// Compute centre and radii
				auto centerX = (x0 + x1) / 2;
				auto centerY = (y0 + y1) / 2;
				auto radiusX = x0<x1 ? x1-centerX : x0-centerX;
				auto radiusY = y0<y1 ? y1-centerY : y0-centerY;

				bool isWideEllipse = true;

				if( radiusX < radiusY )
				{
					// It's a TALL ellipse. Swap Radii so it becomes WIDE
					// and flag for coordinate swapping:

					auto temp = radiusX;
					radiusX     = radiusY;
					radiusY     = temp;
					isWideEllipse = false;
				}

				// This plots a 'wide' ellipse.
				SCALAR d       = 3 - (2 * radiusX);
				SCALAR X       = 0;         // Circle X and y0 X
				SCALAR Y       = radiusX;   // Circle Y and Side X
				SCALAR topY    = radiusY;   // y0 Y
				SCALAR sideY   = 0;         // Side Y
				SCALAR topDec  = radiusX/2; // Limit=RadiusX, IncBy=RadiusY
				SCALAR sideDec = radiusX/2; // Limit=RadiusX, IncBy=RadiusY
				SCALAR delta1  = 6;
				SCALAR delta2  = -radiusX*4 + 10;

				while(X <= Y)
				{
					if(isWideEllipse)
					{
						// Draw wide ellipse
						PlotPoint( centerX + X, centerY + topY  );
						PlotPoint( centerX + Y, centerY + sideY );
						PlotPoint( centerX + X, centerY - topY  );
						PlotPoint( centerX + Y, centerY - sideY );
						PlotPoint( centerX - X, centerY + topY  );
						PlotPoint( centerX - Y, centerY + sideY );
						PlotPoint( centerX - X, centerY - topY  );
						PlotPoint( centerX - Y, centerY - sideY );
					}
					else
					{
						// Draw tall ellipse
						PlotPoint( centerX + topY,  centerY + X );
						PlotPoint( centerX + sideY, centerY + Y );
						PlotPoint( centerX + topY,  centerY - X );
						PlotPoint( centerX + sideY, centerY - Y );
						PlotPoint( centerX - topY,  centerY + X );
						PlotPoint( centerX - sideY, centerY + Y );
						PlotPoint( centerX - topY,  centerY - X );
						PlotPoint( centerX - sideY, centerY - Y );
					}

					// Conditionally increment SideY:
					sideDec += radiusY;
					if(sideDec > radiusX)
					{
						sideY++;
						sideDec -= radiusX;
					}

					// Decisions:
					if(d >= 0)
					{
						d += delta2;
						delta1 += 4;
						delta2 += 8;
						X++;
						Y--;

						// Conditionally decrement TopY:
						topDec += radiusY;
						if(topDec > radiusX)
						{
							topY--;
							topDec -= radiusX;
						}

						continue;
					}

					d += delta1;
					delta1 += 4;
					delta2 += 4;
					X++;
				}

				return true; // drew something
			}



			template<typename SCALAR, typename POINT_RECEIVER>
			void Line(
				SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1,
				POINT_RECEIVER &plotPoint)
			{
				// Draw line (no fast rejection against a viewport)

				//    \ b | c /
				//    a \ | / d
				//    ----+----   (x0,y0) at +
				//    h / | \ e
				//    / g | f \     .

				if(y1 <= y0)
				{
					// a b c d
					if(x1 < x0)
					{
						// a b
						auto absdx = x0-x1;
						auto absdy = y0-y1;
						if(absdx > absdy)
						{
							// a
							SCALAR d = absdx / 2;
							while(x0 >= x1)
							{
								plotPoint(x0--, y0);
								d+=absdy; if(d<absdx) continue; y0--; d-=absdx;
							}
						}
						else
						{
							// b
							auto d = absdy / 2;
							while(y0 >= y1)
							{
								plotPoint(x0, y0--);
								d+=absdx; if(d<absdy) continue; x0--; d-=absdy;
							}
						}
					}
					else
					{
						// c d
						auto absdx = x1-x0;
						auto absdy = y0-y1;
						if(absdx > absdy)
						{
							// d
							auto d = absdx / 2;
							while(x0 <= x1)
							{
								plotPoint(x0++, y0);
								d+=absdy; if(d<absdx) continue; y0--; d-=absdx;
							}
						}
						else
						{
							// c
							auto d = absdy / 2;
							while(y0 >= y1)
							{
								plotPoint(x0, y0--);
								d+=absdx; if(d<absdy) continue; x0++; d-=absdy;
							}
						}
					}
				}

				//    \ b | c /
				//    a \ | / d
				//    ----+----   (x0,y0) at +
				//    h / | \ e
				//    / g | f \    .

				else
				{
					// e f g h
					if(x1<x0)
					{
						// g h
						auto absdx = x0-x1;
						auto absdy = y1-y0;
						if(absdx > absdy)
						{
							// h
							auto d = absdx / 2;
							while(x0 >= x1)
							{
								plotPoint(x0--, y0);
								d+=absdy; if(d<absdx) continue; y0++; d-=absdx;
							}
						}
						else
						{
							// g
							auto d = absdy / 2;
							while(y0 <= y1)
							{
								plotPoint(x0, y0++);
								d+=absdx; if(d<absdy) continue; x0--; d-=absdy;
							}
						}
					}
					else
					{
						// e f
						auto absdx = x1-x0;
						auto absdy = y1-y0;
						if(absdx > absdy)
						{
							// e
							auto d = absdx / 2;
							while(x0 <= x1)
							{
								plotPoint(x0++, y0);
								d+=absdy; if(d<absdx) continue; y0++; d-=absdx;
							}
						}
						else
						{
							// f
							auto d = absdy / 2;
							while(y0 <= y1)
							{
								plotPoint(x0, y0++);
								d+=absdx; if(d<absdy) continue; x0++; d-=absdy;
							}
						}
					}
				}
			}




/* TODO: Is this a duplicate?
			template<typename SCALAR, typename POINT_RECEIVER>
			bool Line(
				Rect<SCALAR> Viewport,
				SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1,
				POINT_RECEIVER &PlotPoint)
			{
				// Draw a line (fast rejection against viewport)

				// Swap the coordinates so we can perform
				// fast-rejection against the viewport:

				auto CopyOfX0 = x0;
				auto CopyOfX1 = x1;
				auto CopyOfY0 = y0;
				auto CopyOfY1 = y1;

				// Swap
				CondSwapEdges(x0, x1);
				CondSwapEdges(y0, y1);

				// Check
				if( x0 >= Viewport.right || x1 <= Viewport.left ||
					y0 >= Viewport.bottom || y1 <= Viewport.top ) return false; // didn't draw anything

				// Draw the line:
				template_LineToPoints<SCALAR,POINT_RECEIVER>( CopyOfX0, CopyOfY0, CopyOfX1, CopyOfY1, PlotPoint );
				return true; // drew something
			}
			*/




			template<typename SCALAR, typename POINT_RECEIVER>
			void BresenhamLineForScanConversion(
				SCALAR x,
				SCALAR y,
				SCALAR endX,
				SCALAR endY,
				POINT_RECEIVER &ppf )
			{
				// BRESENHAM LINE for SCAN CONVERSION

				// Computes the points on a line, but only as they cross scan lines.
				// This is an adapted Bresenham line algorithm for polygon scan conversion.
				// This supports a viewport region vertically, but not horizontally.

				// This routine should never be passed a horizontal line:
				if( y == endY ) return;

				// Compute deltas:
				auto deltaX    = endX - x;
				auto deltaY    = endY - y;
				auto sgnDeltaX = Maths::Sgn(deltaX);
				auto sgnDeltaY = Maths::Sgn(deltaY);
				auto absDeltaX = Maths::Abs(deltaX);
				auto absDeltaY = Maths::Abs(deltaY);

				if( absDeltaX > absDeltaY )
				{
					// Line is more horizontal than vertical.

					auto decision = absDeltaX; // necessary to plot 1st point always
					assert(y != endY);
					while( y != endY )
					{
						decision += absDeltaY;
						if(decision >= absDeltaX)
						{
							decision -= absDeltaX;
							ppf(x,y);
							y += sgnDeltaY;
						}
						x += sgnDeltaX;
					}
				}
				else
				{
					// Else, line is more vertical than horizontal:

					assert( absDeltaX <= absDeltaY );

					auto decision = absDeltaY / 2;
					assert(y != endY); // means at least 1 point will always be drawn
					while(y != endY)
					{
						ppf(x,y);
						y += sgnDeltaY;
						decision += absDeltaX;
						if(decision >= absDeltaY)
						{
							x += sgnDeltaX;
							decision -= absDeltaY;
						}
					}
				}
			}





			template<typename SCALAR, typename POINT_RECEIVER>
			void PlotArrayOfPoints( Point<SCALAR> *begin, Point<SCALAR> *end, POINT_RECEIVER &pr )
			{
				// Plot an array of points to a POINT_RECEIVER

				while( begin < end )
				{
					pr( begin->x, begin->y );
					++begin;
				}
			}

		} /// end namespace

	} /// end namespace

} /// end namespace







// - - - LINE_RECEIVER -> POINT_RECEIVER Functors - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		namespace LineReceivers
		{
			template<typename SCALAR, typename POINT_RECEIVER>
			class LineDraw
			{
			public:

				// To be used as a LINE_RECEIVER and connect to a POINT_RECEIVER
				// Regular line draw with viewport.

				LineDraw( POINT_RECEIVER &pp, Rect<SCALAR> viewport )
                    : _pp(pp)
                    , _view(viewport)
                {
                }

				inline void operator()( SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1 )
				{
					ToPoints::Line<SCALAR,POINT_RECEIVER>( _view, x0,y0, x1,y1, _pp );
				}

			private:

				POINT_RECEIVER  &_pp;
				Rect<SCALAR>     _view;

			};


			template<typename SCALAR, typename POINT_RECEIVER>
			class LineDrawNoRejectCheck
			{
			public:

				// To be used as a LINE_RECEIVER and connect to a POINT_RECEIVER
				// Regular line draw, no viewport.

				LineDrawNoRejectCheck( POINT_RECEIVER &pp )
                    : _pp(pp)
                {
                }

				inline void operator()( SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1 )
				{
					ToPoints::Line<SCALAR,POINT_RECEIVER>( x0,y0, x1,y1, _pp );
				}

			private:

				POINT_RECEIVER &_pp;

			};


			template<typename SCALAR>
			class LineDrawToLRArray
			{
			public:

				// This LINE_RECEIVER draws lines into a LRCollector<SCALAR> container.

				LineDrawToLRArray( Raster::RasterLR<SCALAR> *lrArray, size_t arraySize, Rect<SCALAR> viewport )
                    : _lrCollector( lrArray, arraySize, viewport )
                {
                }

				inline void operator()( SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1 )
				{
					ToPoints::BresenhamLineForScanConversion<SCALAR>( x0,y0,x1,y1, _lrCollector );
				}

			private:
				Raster::LRCollector<SCALAR>  _lrCollector;
			};



			template<typename SCALAR>
			class ScanConversionPointCounter
			{
			public:

				// LINE_RECEIVER for SCAN CONVERSION points counting
				//
				// This LINE_RECEIVER estimates the number of Point<SCALAR>s required
				// to scan-convert a shape.  Pass your shapes to this first, allocate the
				// array, then use the 'class ScanConverter'.
				//
				// This estimate may be slightly larger than is required, but never smaller.
				//
				// This is a LINE_RECEIVER functor.

				ScanConversionPointCounter( Rect<SCALAR> viewport )
					: _viewport(viewport)
					, _count(0)
				{
				}

				// inline void SetExtents( Rect<SCALAR> Extents ) { } // do nothing

				inline void operator()( SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1 ) // LINE_RECEIVER
				{
					x0; // not used
					x1; // not used

					// Horizontal lines play no part in scan conversion:
					if( y0 != y1 )
					{
						// Swap:
						auto  top    = min(y0,y1);
						auto  bottom = max(y0,y1);

						// Is it totally invisible (vertically)?
						if( top >= _viewport.bottom ) return;
						if( bottom <= _viewport.top ) return;

						// Include this in the count:
						_count += (bottom - top) + 1;
					}
				}

				size_t  size() const { return _count; }

				inline Rect<SCALAR> GetViewportRect() const   { return _viewport; }

			private:

				Rect<SCALAR> _viewport;
				size_t       _count;

			};



			template<typename SCALAR>
			class ScanConverter
			{
			public:

				// LINE_RECEIVER for SCAN CONVERSION -> Point<SCALAR> array

				// This LINE_RECEIVER scan-converts one or more input shapes.
				// - A shape is deemed to end when a line is seen that joins back to the start point.
				// - Any lines submitted after that start an additional shape.
				// If more than one shape is submitted, the shape overlaps form holes.
				// Points are collected into a user-supplied array.  The array
				// discards points once full.  If the array is full to capacity
				// on return, it should be discarded, since there is no "DidItOverflow()".
				// Once done, call SortAndGetSize() to check and finalise the points array.
				//
				// IMPORTANT:
				// - Each shape submitted MUST be closed (final line joins back to start point).
				// - You must submit a shape as a sequence of lines where the next line starts
				//   where the previous one ends.
				// Failing to do so results in unpaired points, and a rejection of the composite shape.
				//
				// NOTE:
				// The 'template_XxxxxxToLines()' functions are compatible with this.
				//
				// This is a suitable LINE_RECEIVER functor.
				//

				ScanConverter()
					: _pArrayStart(nullptr)
					, _pSubPolyStart(nullptr)
					, _pArray(nullptr)
					, _pArrayEnd(nullptr)
					, _firstLineDiff(0) /// irrelevant initialisation
					, _previousLineDiff(0) /// irrelevant initialisation
					, _mostRecentLineDirectionChanged(0)
					, _mrx1(0)
					, _mry1(0)
					, _firstX(0)
					, _firstY(0)
					, _firstPointRecordedOnSubPoly(false)
				{
				}

				void Reset( Point<SCALAR> *pointArray, size_t arraySize )
				{
					_pArrayStart   = pointArray;
					_pArray        = pointArray;
					_pSubPolyStart = pointArray;
					_pArrayEnd     = pointArray + arraySize;
				}

				void SetViewport( Rect<SCALAR> &viewport )
				{
					_viewport = viewport;
				}

				void operator()( SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1 ) // LINE_RECEIVER
				{
					if( ! _firstPointRecordedOnSubPoly )
					{
						_firstX = x0;
						_firstY = y0;
						_firstPointRecordedOnSubPoly = true;
					}

					// Ignore horizontals -- they never contribute.
					if( y0 != y1 )
					{
						// The polygon scan converter's "Add line" state-machine.

						auto thisLineDiff = y1 - y0;
						bool firstLine = (_pArray == _pSubPolyStart);
						if( firstLine )
						{
							// This is the first line on a new sub-polygon.
							#ifdef ATLTRACE_THE_POLY_FILL
							ATLTRACE( "Scan convert: NEW SUB POLY\n" );
							#endif
							_firstLineDiff = thisLineDiff;
						}
						else if( LineDirectionChanged( thisLineDiff, _previousLineDiff ) )
						{
							// We have doubled back.  Supply missing end point of previous line:
							#ifdef ATLTRACE_THE_POLY_FILL
							ATLTRACE( "Scan convert: Doubling back, adding [%d,%d]\n", x0,y0 );
							#endif
							operator()( x0,y0 );
						}

						#ifdef ATLTRACE_THE_POLY_FILL
						ATLTRACE( "Scan convert: Line [%d,%d] - (%d,%d)\n", x0,y0, x1,y1 );
						#endif
						_previousLineDiff = thisLineDiff; // for next time

						// Is it totally invisible (vertically)?

						// TODO: This speed optimisation turned out to be faulty with >= and <=
						//         so these have been slackened to > and <.  If still faulty, just
						//         draw the line anyway:

						auto topY    = std::min(y0,y1);
						auto bottomY = std::max(y0,y1);
						if( topY > _viewport.bottom || bottomY < _viewport.top )
						{
							// It's invisible vertically, so none of the operator() calls will contribute points
						}
						else
						{
							// Draw the line.  This includes ONE point on each raster in range [y0..y1)
							ToPoints::BresenhamLineForScanConversion<SCALAR>( x0,y0,x1,y1, *this );
						}

						// Record all this for a (possible) future examination by the code below,
						// since a horizontal may close the polygon.
						_mostRecentLineDirectionChanged = LineDirectionChanged( _firstLineDiff, thisLineDiff );
						_mrx1 = x1;
						_mry1 = y1;
					}

					// Did this line close the sub-polygon?
					if( x1 == _firstX && y1 == _firstY )
					{
						if( _mostRecentLineDirectionChanged )
						{
							_mostRecentLineDirectionChanged = 0;
							#ifdef ATLTRACE_THE_POLY_FILL
							ATLTRACE( "Scan convert: Sub-polygon closed in different direction to first line.  Adding extra point [%d,%d]\n", m_mrx1,m_mry1 );
							#endif
							operator()( _mrx1, _mry1 );
						}
						_pSubPolyStart = _pArray; // for the next sub-poly
						_firstPointRecordedOnSubPoly = false; // for next time
					}
				}

				inline void operator()( SCALAR x, SCALAR y ) // POINTS_RECEIVER (internal use only)
				{
					if( _pArray < _pArrayEnd )
					{
						if( y >= _viewport.top && y < _viewport.bottom )
						{
							*_pArray = Point<SCALAR>(x,y);
							++_pArray;
						}
					}
				}

				size_t  SortAndGetSize();

				inline Rect<SCALAR> GetViewportRect() const { return _viewport; }

			private:

				Rect<SCALAR>   _viewport;
				Point<SCALAR> *_pArrayStart;
				Point<SCALAR> *_pSubPolyStart;
				Point<SCALAR> *_pArray;
				Point<SCALAR> *_pArrayEnd;
				SCALAR         _firstLineDiff;
				SCALAR         _previousLineDiff;

				SCALAR         _mostRecentLineDirectionChanged;
				SCALAR         _mrx1;
				SCALAR         _mry1;

				SCALAR         _firstX;
				SCALAR         _firstY;
				bool           _firstPointRecordedOnSubPoly;

			};


			template<typename SCALAR>
			inline bool PolygonPointSortPredicate( Point<SCALAR> p1, Point<SCALAR> p2 )
			{
				// Predicate for std::sort() for polygon points.
				return (p1.y < p2.y) || (p1.y == p2.y && p1.x < p2.x);
			}


			template<typename SCALAR>
			size_t  ScanConverter<SCALAR>::SortAndGetSize()
			{
				// Finalisation of the Points<SCALAR> array:
				// - Sort the points ready for raster painting.
				// - Clip rasters horizontally.
				// - Discard points that don't pair up.

				// Check for out of buffer space.
				// There must be 1 free at least afterwards (this was an easy way to implement out of space).
				if( _pArray == _pArrayEnd ) return 0;

				// Sort the points:
				std::sort( _pArrayStart, _pArray, PolygonPointSortPredicate<SCALAR> );

				// Apply horizontal clipping to the rasters described by the points buffer.
				// This may entail losing some entries entirely.
				// We also lose zero-width items.

				auto source    = _pArrayStart;
				auto sourceEnd = _pArray - 1; // -1 to account for reading in pairs
				auto dest      = source; // overlay result.

				while( source < sourceEnd )
				{
					// Look for a pair of points on the same Y (makes a raster):
					auto  y = source[0].y;
					if( y == source[1].y )
					{
						// Tests to see if this raster is completely
						// of the left/right sides of the viewport:
						auto   left  = source[0].x;
						auto   right = source[1].x;

						right += 1; // to add an extra pixel to treat the source data 'right' as inclusive in the range.

						if( left >= right || left >= _viewport.right || right <= _viewport.left )
						{
							// Skip the line:
							// - It's zero or negative width, or:
							// - It's completely off one of the sides.
						}
						else
						{
							// Ensure the line is clipped to the viewport:
							left  = std::max( left,  _viewport.left );
							right = std::min( right, _viewport.right );

							// Keep this line, if it is still non-zero width:
							if( left < right )
							{
								dest[0].x = left;
								dest[0].y = y;
								dest[1].x = right;
								dest[1].y = y;
								dest += 2;
							}
						}

						source += 2; // advance to next pair of points
					}
					else
					{
						// We get here in the "unpairable" case.
						#ifdef ATLTRACE_THE_POLY_FILL
						ATLTRACE( "Renderer: Unpaired point {%d,%d} with {%d,%d}\n", Source[0].x, Source[0].y, Source[1].x, Source[1].y );
						#endif
						return 0; // fail the request outright, the scan conversion should never output unpaired points.
					}
				}

				// Update to what we kept:
				_pArray = dest;

				// Return the number of points in the final array:
				size_t countOfPoints = _pArray - _pArrayStart;
				#ifdef ATLTRACE_THE_POLY_FILL
				ATLTRACE( "Renderer: All points paired up OK\n" );
				#endif
				return countOfPoints;
			}

		} /// end namespace

	} /// end namespace

} /// end namespace





// - - - SHAPES -> LINE_RECEIVER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		namespace ToLines
		{
			template<typename SCALAR>
			Rect<SCALAR>  RectangleExtents(
				SCALAR x0, SCALAR y0,
				SCALAR x1, SCALAR y1 )
			{
				// Get (rectified) extents rectangle of given rectangle.
				return GetBoundingRectangle(
					Point<SCALAR>(x0,y0),
					Point<SCALAR>(x1,y1) );
			}

			template<typename SCALAR, typename LINE_RECEIVER>
			bool Rectangle(
				Rect<SCALAR> viewport,
				SCALAR x0, SCALAR y0,
				SCALAR x1, SCALAR y1,
				LINE_RECEIVER &drawLine )
			{
				// Draw outline rectangle.

				// Determine extents, and do fast rejection check against viewport:
				if( Intersects( RectangleExtents(x0,y0,x1,y1), viewport ) )
				{
					drawLine( x0,y0,x1,y0 ); // top side
					drawLine( x1,y0,x1,y1 ); // right side
					drawLine( x1,y1,x0,y1 ); // bottom side
					drawLine( x0,y1,x0,y0 ); // left side
					return true; // drew something.
				}
				return false; // did not draw anything.
			}

			template<typename SCALAR>
			Rect<SCALAR> TriangleExtents(
				SCALAR x0, SCALAR y0,
				SCALAR x1, SCALAR y1,
				SCALAR x2, SCALAR y2 )
			{
				// Get (rectified) extents rectangle of given triangle.
				auto extents = Rect<SCALAR>( x0,y0,x0,y0 );
				extents = Union( extents, Point<SCALAR>(x1,y1) );
				extents = Union( extents, Point<SCALAR>(x2,y2) );
				return extents;
			}

			template<typename SCALAR, typename LINE_RECEIVER>
			bool Triangle(
				Rect<SCALAR> viewport,
				SCALAR x0, SCALAR y0,
				SCALAR x1, SCALAR y1,
				SCALAR x2, SCALAR y2,
				LINE_RECEIVER &drawLine )
			{
				// Draw a triangle

				// Determine extents, and do fast rejection check against viewport:
				if( Intersects( TriangleExtents( x0,y0, x1,y1, x2,y2 ), viewport ) )
				{
					// Draw it:
					drawLine( x0,y0, x1,y1 );
					drawLine( x1,y1, x2,y2 );
					drawLine( x2,y2, x0,y0 );
					return true; // drew something.
				}
				return false; // did not draw anything.
			}

			template<typename SCALAR>
			Rect<SCALAR> BezierExtents(
				SCALAR BX0, SCALAR BY0,
				SCALAR BX1, SCALAR BY1,
				SCALAR BX2, SCALAR BY2,
				SCALAR BX3, SCALAR BY3 )
			{
				// Get (rectified) extents rectangle of given bezier curve convex hull.
				auto extents = Rect<SCALAR>(BX0,BY0,BX0,BY0);
				extents = Union( extents, Point<SCALAR>(BX1,BY1) );
				extents = Union( extents, Point<SCALAR>(BX2,BY2) );
				extents = Union( extents, Point<SCALAR>(BX3,BY3) );
				return extents;
			}

			template<typename SCALAR, typename LINE_RECEIVER>
			bool Bezier(
				Rect<SCALAR> viewport,
				SCALAR bX0, SCALAR bY0,
				SCALAR bX1, SCALAR bY1,
				SCALAR bX2, SCALAR bY2,
				SCALAR bX3, SCALAR bY3,
				LINE_RECEIVER &drawLine )
			{
				// Draw bezier curve to lines

				// Determine extents, and do fast rejection check against viewport:
				if( Intersects( BezierExtents( bX0,bY0, bX1,bY1, bX2,bY2, bX3,bY3 ), viewport ) )
				{
					// Draw:
					auto   sX = bX0;
					auto   sY = bY0;
					auto   p = &BezierConstants[0];
					while( p != &BezierConstants[15*4] )
					{
						SCALAR xE = (bX0*p[0] + bX1*p[1] + bX2*p[2] + bX3*p[3]) >> 12;
						SCALAR yE = (bY0*p[0] + bY1*p[1] + bY2*p[2] + bY3*p[3]) >> 12;
						drawLine( sX,sY, xE,yE );
						p += 4;
						sX = xE;
						sY = yE;
					}
					drawLine( sX,sY, bX3,bY3 );
					return true; // drew something
				}
				return false; // did not draw anything.
			}

			template<typename SCALAR>
			Rect<SCALAR> PolygonExtents( const Point<SCALAR> *endPointsArray, size_t arraySize )
			{
				// Get (rectified) extents rectangle of given polygon points.
				if( arraySize >= 1 )
				{
					// Determine extents:
					auto p = endPointsArray;
					auto e = endPointsArray + arraySize;
					auto extents = Rect<SCALAR>( p->x, p->y, p->x, p->y );
					++p;
					while( p < e )
					{
						extents = Union( extents, Point<SCALAR>( p->x, p->y ) );
						++p;
					}
					return extents; // did something
				}
				return Rect<SCALAR>(); // invalid poly
			}

			template<typename SCALAR, typename LINE_RECEIVER>
			bool Polygon(
				Rect<SCALAR> viewport,
				const Point<SCALAR> *endPointsArray,
				size_t arraySize,
				bool closeIt,
				LINE_RECEIVER  &drawLine )
			{
				// Draw polygon to lines

				auto extents = PolygonExtents( endPointsArray, arraySize );
				if( HasArea(extents) )
				{
					if( Intersects( extents, viewport ) )
					{
						// Draw the poly array;
						auto p = endPointsArray;
						auto e = endPointsArray + (arraySize - 1);
						while( p < e )
						{
							drawLine( p[0].x, p[0].y, p[1].x, p[1].y );
							++p;
						}

						if( closeIt )
						{
							// Draw closing line back to the start point:
							drawLine( p[-1].x, p[-1].y, endPointsArray->x, endPointsArray->y );
						}

						return true; // drew something
					}
				}

				return false; // did not draw
			}

			template<typename SCALAR>
			struct ArcInfo
			{
				Point<SCALAR> Centre;
				Point<SCALAR> StartOnArc;
				Point<SCALAR> EndOnArc;
				SCALAR Radiusx;
				SCALAR Radiusy;
			};

			template<typename SCALAR>
			Point<SCALAR>  PointOnArc( ArcInfo<SCALAR> &theArc, uint32_t theta )
			{
				// Return position of a point on the arc at angle 'theta' degrees, range (0-359).
				return Point<SCALAR>(
					theArc.Centre.x + ((theArc.Radiusx * FixedPointSine(theta)) / 65536),
					theArc.Centre.y - ((theArc.Radiusy * FixedPointCosine(theta)) / 65536)  );
			}

			template<typename SCALAR>
			bool GetArcInfo(
				SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1,
				int32_t startAngleDegrees,
				int32_t endAngleDegrees,
				ArcInfo<SCALAR> *out_arcResult, Rect<SCALAR> *out_extents )
			{
				// Validate angles:
				if( startAngleDegrees < 0 || startAngleDegrees > 359 ||
					endAngleDegrees   < 0 || endAngleDegrees   > 360 )  // we allow 360 for end
				{
					return false; // did not draw it
				}

				// Swap if inside out:
				CondSwapEdges( x0,x1 );
				CondSwapEdges( y0,y1 );

				// Return extents:
				if( out_extents )
				{
					*out_extents = Rect<SCALAR>(x0,y0,x1,y1);
				}

				// Reserve ArcInfo record:
				ArcInfo<SCALAR> ar;

				// Centre:
				ar.Centre.x = (x1+x0)/2;
				ar.Centre.y = (y1+y0)/2;

				// Calculate radii:
				ar.Radiusx = (x1-x0)/2;
				ar.Radiusy = (y1-y0)/2;

				// StartOnArc:
				ar.StartOnArc = PointOnArc( ar, startAngleDegrees );

				// EndOnArc:
				if( endAngleDegrees < 360 )
				{
					ar.EndOnArc = PointOnArc( ar, endAngleDegrees );
				}
				else
				{
					// EndAngleDegrees == 360 (Special case: I'm not doing modulo "% 360" indexing just for this )
					ar.EndOnArc.x = ar.Centre.x;
					ar.EndOnArc.y = ar.Centre.y - ar.Radiusy;
				}

				if( out_arcResult )
				{
					*out_arcResult = ar; // send this back to the caller.
				}

				return true;
			}

			template<typename SCALAR, typename LINE_RECEIVER>
			bool Arc(
				Rect<SCALAR> viewport,
				SCALAR x0, SCALAR y0, SCALAR x1, SCALAR y1,
				int32_t startAngleDegrees,
				int32_t endAngleDegrees,
				int32_t sideFlags,
				LINE_RECEIVER  &drawLine )
			{
				// Draw arc / pie slice / secant to lines

				ArcInfo<SCALAR> ar;
				Rect<SCALAR> extents;
				if( GetArcInfo( x0,y0,x1,y1, startAngleDegrees, endAngleDegrees, &ar, &extents ) )
				{
					// Fast rejection check the extents against the viewport:
					if( Intersects( extents, viewport ) )
					{
						// Line from centre out to the start (if required):
						if( (sideFlags & 5) == 1 )
						{
							drawLine( ar.Centre.x, ar.Centre.y, ar.StartOnArc.x, ar.StartOnArc.y );
						}

						// Draw all points on the arc:
						auto prevPoint = ar.StartOnArc;

						// Rectify angular range, if necessary:
						if( startAngleDegrees > endAngleDegrees )
						{
							endAngleDegrees += 360;
						}

						// Draw arc through range:
						auto theta = uint32_t(startAngleDegrees);
						while( theta <= uint32_t(endAngleDegrees) )
						{
							uint32_t index = (theta < 360) ? theta : (theta - 360);
							auto p = PointOnArc( ar, index );
							drawLine( prevPoint.x, prevPoint.y, p.x, p.y );
							prevPoint = p;
							++theta;
						}

						// Line from end to centre (if required):
						if( (sideFlags & 6) == 2 )
						{
							drawLine( ar.EndOnArc.x, ar.EndOnArc.y, ar.Centre.x, ar.Centre.y );
						}

						// Secant flag test (overrides bits 1 and 0):
						if( sideFlags & 4 )
						{
							drawLine( ar.EndOnArc.x, ar.EndOnArc.y, ar.StartOnArc.x, ar.StartOnArc.y );
						}

						return true; // drew it
					}
				}

				return false; // didn't draw anything.
			}


		}	 /// end namespace

	}	 /// end namespace

} /// end namespace
























// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------








// - - - Pens and Brushes - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace Pens
	{
		#define ABSTRACT_PEN_VIRTUALS \
			virtual void ToMetafileText( libBasic::AbstractTextOutputStream * ); \
			virtual uint32_t KludgeGetColour();

		class AbstractPen
		{
		public:
			virtual void ToMetafileText( libBasic::AbstractTextOutputStream * ) = 0;
			virtual uint32_t KludgeGetColour() = 0;
		};

		class Solid: public AbstractPen
		{
		public:
			Solid()                      : Thickness(0), Colour(0) {}
			Solid( uint32_t colour )     : Thickness(1), Colour(colour) {}
			ABSTRACT_PEN_VIRTUALS;
			int32_t   Thickness; // not used yet.
			uint32_t  Colour;
		};
	}

	namespace Brushes
	{
		#define ABSTRACT_BRUSH_VIRTUALS \
			virtual void PaintRasterRectangle( uint32_t *Destination, intptr_t BytesPerScanLine, int32_t left, int32_t top, int32_t WidthPixels, int32_t HeightPixels ); \
			virtual void PaintRasterLR( uint32_t *Destination, intptr_t BytesPerScanLine, int32_t ViewportTop, System::Raster::RasterLR<int32_t> *LRArray, int32_t HeightPixels ); \
			virtual void PaintPolygonRasters( uint32_t *Destination, intptr_t BytesPerScanLine, Point<int32_t> *PointsArray, size_t NumPoints ); \
			virtual void ToMetafileText( libBasic::AbstractTextOutputStream * );


		class AbstractBrush
		{
		public:
			virtual void PaintRasterRectangle( uint32_t *Destination, intptr_t BytesPerScanLine, int32_t left, int32_t top, int32_t WidthPixels, int32_t HeightPixels ) = 0;
			virtual void PaintRasterLR( uint32_t *Destination, intptr_t BytesPerScanLine, int32_t ViewportTop, System::Raster::RasterLR<int32_t> *LRArray, int32_t HeightPixels ) = 0;
			virtual void PaintPolygonRasters( uint32_t *Destination, intptr_t BytesPerScanLine, Point<int32_t> *PointsArray, size_t NumPoints ) = 0;
			virtual void ToMetafileText( libBasic::AbstractTextOutputStream * ) = 0;
		};

		class Solid: public AbstractBrush
		{
		public:
			Solid() {}
			explicit Solid( uint32_t colour ) : Settings(colour) {}
			ABSTRACT_BRUSH_VIRTUALS;
			System::Raster::SolidBrush<uint32_t> Settings;
		};

		class Patterned: public AbstractBrush
		{
		public:
			Patterned() {}
			explicit Patterned( const uint16_t *pattern )                                     : Settings( pattern ) {}
			Patterned( const uint16_t *pattern, uint32_t foreColour )                         : Settings( pattern, foreColour ) {}
			Patterned( const uint16_t *pattern, uint32_t foreColour, uint32_t backColour )    : Settings( pattern, foreColour, backColour ) {}
			ABSTRACT_BRUSH_VIRTUALS;
			System::Raster::PatternBrush<uint32_t> Settings;
		};

		#define AMIX_TYPES_TUPLE     uint32_t(0xFEFEFE),uint32_t(1)
		class AverageMixed: public AbstractBrush
		{
		public:
			AverageMixed() {}
			explicit AverageMixed( uint32_t colour )   : Settings(colour) {}
			ABSTRACT_BRUSH_VIRTUALS;
			System::Raster::AverageMixBrush<uint32_t,AMIX_TYPES_TUPLE> Settings;
		};
		#undef AMIX_TYPES_TUPLE

		#undef ABSTRACT_BRUSH_VIRTUALS
	}
}





// - - - Bitmaps - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		template<typename SCALAR, typename PIXEL>
		struct template_Bitmap
		{
			// A structure representing a bitmap, residing in user-defined memory.
			template_Bitmap()     : TopLeft(nullptr), BytesPerScanLine(0), WidthPixels(0), HeightPixels(0) {}
			PIXEL    *TopLeft;
			intptr_t  BytesPerScanLine;
			SCALAR    WidthPixels;
			SCALAR    HeightPixels;
		};
	}

	namespace Bitmaps
	{
		#define ABSTRACT_BITMAP_VIRTUALS \
			virtual void ToMetafileText( libBasic::AbstractTextOutputStream * ); \
			virtual void DrawOpaqueTo32bpp( System::template_Bitmap<int32_t,uint32_t> &TargetBM, Rect<int32_t> Viewport, Rect<int32_t> AreaOnTarget, Rect<int32_t> AreaOnSource, uint32_t ForeColour, uint32_t BackColour ); \
			virtual void DrawTransparentTo32bpp( System::template_Bitmap<int32_t,uint32_t> &TargetBM, Rect<int32_t> Viewport, Rect<int32_t> AreaOnTarget, Rect<int32_t> AreaOnSource, uint32_t ForeColour ); \
			virtual Rect<int32_t> GetExtents();

		class AbstractBitmap
		{
		public:
			virtual void ToMetafileText( libBasic::AbstractTextOutputStream * ) = 0;
			virtual void DrawOpaqueTo32bpp(      System::template_Bitmap<int32_t,uint32_t> &targetBM, Rect<int32_t> viewport, Rect<int32_t> areaOnTarget, Rect<int32_t> areaOnSource, uint32_t foreColour, uint32_t backColour ) = 0;
			virtual void DrawTransparentTo32bpp( System::template_Bitmap<int32_t,uint32_t> &targetBM, Rect<int32_t> viewport, Rect<int32_t> areaOnTarget, Rect<int32_t> areaOnSource, uint32_t foreColour ) = 0;
			virtual Rect<int32_t> GetExtents() = 0;
		};

		class Colour: public AbstractBitmap, public System::template_Bitmap<int32_t,uint32_t>
		{
		public:
			// The "standard" bitmap, 32-bpp.
			Colour();
			Colour( uint32_t *topLeft, int32_t widthPixels, int32_t heightPixels, intptr_t bytesPerScanLine );
			ABSTRACT_BITMAP_VIRTUALS;
		};

		class Mono: public AbstractBitmap, public System::template_Bitmap<int32_t,uint8_t>
		{
		public:
			// Mono bitmap (interpretation 1-bpp).
			// Bit 7 is the left pixel always.  Hint: Use a viewport, and decrement X coordinate to clip.
			Mono();
			Mono( uint8_t *topLeft, int32_t widthPixels, int32_t heightPixels, intptr_t bytesPerScanLine );
			ABSTRACT_BITMAP_VIRTUALS;
		};
	}
}






// - - - (Stretchblt) - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace Bitmaps
	{
		void ScaleColourOpaque(
			System::template_Bitmap<int32_t,uint32_t> &bmpTarget,	const Rect<int32_t> &areaOnTarget,
			System::template_Bitmap<int32_t,uint32_t> &bmpSource,	const Rect<int32_t> &areaOnSource,
			const Rect<int32_t> &viewport );

		void ScaleColourTransparent(
			System::template_Bitmap<int32_t,uint32_t> &bmpTarget,	const Rect<int32_t> &areaOnTarget,
			System::template_Bitmap<int32_t,uint32_t> &bmpSource,	const Rect<int32_t> &areaOnSource,
			const Rect<int32_t> &viewport,
			uint32_t transparencyColour );

		void ScaleMonoOpaque(
			System::template_Bitmap<int32_t,uint32_t> &bmpTarget,   const Rect<int32_t> &areaOnTarget,
			System::template_Bitmap<int32_t,uint8_t> &bmpSource,    const Rect<int32_t> &areaOnSource,
			const Rect<int32_t> &viewport,
			uint32_t  foreColour,
			uint32_t  backColour );

		void ScaleMonoTransparent(
			System::template_Bitmap<int32_t,uint32_t> &bmpTarget,   const Rect<int32_t> &areaOnTarget,
			System::template_Bitmap<int32_t,uint8_t>  &bmpSource,   const Rect<int32_t> &areaOnSource,
			const Rect<int32_t> &viewport,
			uint32_t foreColour );

	}  /// end namespace

}  /// end namespace








namespace libGraphics
{
	struct Scaling
	{
		// Scaling structure.
		// Allows independent scaling horizontally and vertically.
		Scaling()                                                                                 : MultiplierX(1), DivisorX(1), MultiplierY(1), DivisorY(1) {}
		Scaling( int32_t multiplierX, int32_t multiplierY )                                       : MultiplierX(multiplierX), DivisorX(1), MultiplierY(multiplierY), DivisorY(1) {}
		Scaling( int32_t multiplierX, int32_t divisorX, int32_t multiplierY, int32_t divisorY )   : MultiplierX(multiplierX), DivisorX(divisorX), MultiplierY(multiplierY), DivisorY(divisorY) {}

		int32_t  MultiplierX;
		int32_t  DivisorX;
		int32_t  MultiplierY;
		int32_t  DivisorY;
	};
}




// - - - Abstract classes - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		namespace LineReceivers
		{
			class AbstractLineReceiver
			{
			public:
				virtual void operator()( int32_t x0, int32_t y0, int32_t x1, int32_t y1 ) = 0;
				virtual Rect<int32_t> GetViewportRect() = 0;
			};
		}
	}

	namespace Fonts
	{
		namespace Internal
		{
			struct DrawInfo; /// in GraphicsFonts.h
		}

		class AbstractFont
		{
		public:
			// Provides format-independent access to character images of a bitmapped font.
			/// The same 'DrawInfo' instance is always passed to these functions.
			/// This interface allows flexibility, as you can have some fields
			/// in 'DrawInfo' that are initialised ONCE, and others that need to
			/// change per-character.
			virtual bool OnFirstInit( Internal::DrawInfo *out_drawInfo ) = 0;
			virtual bool OnGetChar( uint32_t CharCode, Internal::DrawInfo *out_drawInfo ) = 0;
		};

		class AbstractFontServer
		{
		public:
			// Provides access to fonts (usage counted).
			virtual std::shared_ptr<AbstractFont>  AddRefFont( const char *fontName, uint32_t pointSizeTenths ) = 0;
			virtual bool ReleaseFont( std::shared_ptr<AbstractFont> pFont ) = 0; // TODO: Have a LRU expiry instead?
		};
	}

	namespace Devices
	{
		class AbstractDevice
		{
		public:
			// This is a drawing interface.
			virtual void SetViewport( Rect<int32_t> r ) = 0;  /// Viewport cannot be set while adding polygon points.
			virtual void SelectPen( std::shared_ptr<Pens::AbstractPen> ) = 0;
			virtual void SelectBrush( std::shared_ptr<Brushes::AbstractBrush> ) = 0;
			virtual void SelectBitmap( std::shared_ptr<Bitmaps::AbstractBitmap> ) = 0;
			virtual void SelectFont( const char *fontName, uint32_t pointSizeTenths ) = 0;
			virtual void SetForegroundColour( uint32_t foreColour ) = 0;
			virtual void SetBackgroundColour( uint32_t backColour ) = 0;
			virtual Rect<int32_t> GetViewport() = 0;
			virtual void StartPoly() = 0;
			virtual void EndPoly() = 0;
			virtual void MoveTo( int32_t x,int32_t y ) = 0;
			virtual void LineTo( int32_t x,int32_t y ) = 0;
			virtual void Arc(    Rect<int32_t> r, int32_t startAngle, int32_t endAngle ) = 0;
			virtual void Secant( Rect<int32_t> r, int32_t startAngle, int32_t endAngle ) = 0;
			virtual void Pie(    Rect<int32_t> r, int32_t startAngle, int32_t endAngle ) = 0;
			virtual void Ellipse( Rect<int32_t> r ) = 0;
			virtual void Rectangle( Rect<int32_t> r ) = 0;
			virtual void Bezier( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2,int32_t x3,int32_t y3 ) = 0;
			virtual void Triangle( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2 ) = 0;
			virtual void DrawBitmap( Rect<int32_t> areaOnTarget, Rect<int32_t> areaOnSource, uint32_t flags ) = 0;  // TODO: What are the flags really?
			virtual void Text( int32_t x, int32_t y, Scaling *pScaling, const char *text, size_t charCount ) = 0;
				/// Reminder: pScaling can be NULL to indicate "no font scaling" (use 1:1 pixel size)
			virtual std::shared_ptr<Fonts::AbstractFont> GetFont() const = 0;
		};

		/// Note: ABSTRACT_DEVICE_VIRTUALS does not include stuff done by class BaseDevice:
		#define ABSTRACT_DEVICE_VIRTUALS \
			virtual void MoveTo( int32_t x,int32_t y ) override; \
			virtual void LineTo( int32_t x,int32_t y ) override; \
			virtual void Arc( Rect<int32_t> r, int32_t StartAngle, int32_t EndAngle ) override; \
			virtual void Secant( Rect<int32_t> r, int32_t StartAngle, int32_t EndAngle ) override; \
			virtual void Pie( Rect<int32_t> r, int32_t StartAngle, int32_t EndAngle ) override; \
			virtual void Ellipse( Rect<int32_t> r ) override; \
			virtual void Rectangle( Rect<int32_t> r ) override; \
			virtual void Bezier( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2,int32_t x3,int32_t y3 ) override; \
			virtual void Triangle( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2 ) override; \
			virtual void DrawBitmap( Rect<int32_t> AreaOnTarget, Rect<int32_t> AreaOnSource, uint32_t Flags ) override;

		class BaseDevice: public AbstractDevice
		{
		public:
			// The base activities that all derived devices will need to do.
			BaseDevice();
			virtual ~BaseDevice();
			virtual void SetForegroundColour( uint32_t foreColour ) override;
			virtual void SetBackgroundColour( uint32_t backColour ) override;
			virtual void SetViewport( Rect<int32_t> r ) override;  /// Viewport cannot be set while adding polygon points.
			virtual void SelectPen( std::shared_ptr<Pens::AbstractPen> ) override;
			virtual void SelectBrush( std::shared_ptr<Brushes::AbstractBrush> ) override;
			virtual void SelectBitmap( std::shared_ptr<Bitmaps::AbstractBitmap> ) override;
			virtual void SelectFont( const char *fontName, uint32_t pointSizeTenths ) override;
			virtual void Text( int32_t x, int32_t y, Scaling *pScaling, const char *text, size_t charCount ) override; /// shared routine (does bitmaps)
			virtual Rect<int32_t> GetViewport() override;
			virtual void StartPoly() override;
			virtual void EndPoly() override;
			inline bool DoingPolygon() const                                             { return _polygonModeCounter > 0; }
			virtual std::shared_ptr<Fonts::AbstractFont> GetFont() const override;
			void SetFontServer( std::shared_ptr<Fonts::AbstractFontServer> fontServer )  { _pFontServer = fontServer; }
			std::shared_ptr<Fonts::AbstractFontServer> GetFontServer() const             { return _pFontServer; }
		protected:
			uint32_t       _foreColour;
			uint32_t       _backColour;
			Rect<int32_t>  _viewport;      /// Invariant: Viewport must never be inside-out, and must always lie in the bounds of the target.
			std::shared_ptr<Pens::AbstractPen>        _pPen;
			std::shared_ptr<Brushes::AbstractBrush>   _pBrush;
			std::shared_ptr<Bitmaps::AbstractBitmap>  _pBitmap;
			std::shared_ptr<Fonts::AbstractFont>      _pFont;
		protected:
			inline char PenAvailable()     const { return _pPen    != nullptr; }
			inline char BrushAvailable()   const { return _pBrush  != nullptr; }
			inline char BitmapAvailable()  const { return _pBitmap != nullptr; }
			inline char FontAvailable()    const { return _pFont   != nullptr; }
		private:
			int32_t  _polygonModeCounter; /// nesting counter (>0 == point collecting for filled polygon)
			void ReleaseCurrentFont(); /// assistance for destructor
			std::shared_ptr<Fonts::AbstractFontServer> _pFontServer;   /// null if not set
		};
	}

	namespace Fonts
	{
		void PaintByBitmaps(
				Devices::AbstractDevice &dc,
				AbstractFont &font,
				int32_t x,
				int32_t y,
				const Scaling *pScaling, // null if not required
				const char *text,
				size_t charCount );
			// Paint text in given font onto given DC as individual bitmaps.
			// Use AbstractDevice::Text() in preference to this.
			// OptWidth and OptHeight can both be -1 to disable scaling (ie: 1:1)
			// Otherwise, these represent the dimensions of the extents box to scale
			// into (see Measure() function, below).

		// TODO: must investigate why passing in (x,y) ????
		void Measure( int32_t x, int32_t y, AbstractFont &font, const char *text, size_t charCount, Rect<int32_t> *out_pExtents );
			// Determine text extents, in given font with text at position (x,y)
	}
}







// - - - (Line Receivers) - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		namespace LineReceivers
		{
			class ForBitmapOutlines: public AbstractLineReceiver
			{
			public:
				// LINE_RECEIVER for drawing shape outlines on a bitmap (1 pixel solid pen)
				void SetTargetBitmap( Bitmaps::Colour &bm )                                             { _pointPlotter.SetUncheckedTarget( bm.TopLeft, bm.BytesPerScanLine ); }
				void SetUncheckedViewport( Rect<int32_t> r )                                            { _pointPlotter.SetUncheckedViewport(r); }
				void SetColour( uint32_t Colour )                                                       { _pointPlotter.SetColour(Colour); }
				virtual void operator()( int32_t x0, int32_t y0, int32_t x1, int32_t y1 )               { System::ToPoints::Line( x0,y0,x1,y1, _pointPlotter ); }
				virtual Rect<int32_t> GetViewportRect()                                                 { return _pointPlotter.GetViewport(); }
			private:
				PointReceivers::Plot<uint32_t,int32_t> _pointPlotter;
			};

			class ForBitmapScanConvPointCollecting: public AbstractLineReceiver
			{
			public:
				// LINE_RECEIVER for collecting the points in an array when polygon scan converting.
				inline void Reset( Point<int32_t> *pPts, size_t capacity )                   { _sconv.Reset( pPts, capacity );  }
				inline void SetViewport( Rect<int32_t> &viewport )                           { _sconv.SetViewport( viewport );  }
				virtual void operator()( int32_t x0, int32_t y0, int32_t x1, int32_t y1 )    { _sconv( x0,y0,x1,y1 );           }
				virtual Rect<int32_t> GetViewportRect()                                      { return _sconv.GetViewportRect(); }
				size_t SortAndGetSize()                                                      { return _sconv.SortAndGetSize();  }
			private:
				ScanConverter<int32_t> _sconv;
			};

		} /// end namespace

	}  /// end namespace

} /// end namespace









// - - - (Concrete DCs : Drawing to bitmaps, Metafile recorder, Measuring, Rescale/Translation ) - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace Devices
	{
		class BitmapDevice: public BaseDevice
		{
		public:

			// For drawing onto a user-defined bitmap.

			explicit BitmapDevice( Bitmaps::Colour &Target ); /// WARNING: A copy is taken of the bitmap specifics!

			// Priming:
			void SetLRArray( System::Raster::RasterLR<int32_t> *pArray, size_t capacity );
			void SetPointsArray( Point<int32_t> *pArray, size_t capacity );

			// Query functions
			inline Rect<int32_t> GetExtentsRect() const     { return Rect<int32_t>( 0, 0, _bitmap.WidthPixels, _bitmap.HeightPixels ); }

			// Implementing the standard device API:
			ABSTRACT_DEVICE_VIRTUALS;

			// We also override these from BaseDevice:
			virtual void StartPoly() override;
			virtual void EndPoly() override;
			virtual void SetViewport( Rect<int32_t> r ) override;
			virtual void SelectPen( std::shared_ptr<Pens::AbstractPen> ) override;

			// The "Direct" API, for direct-to-bitmap performance (note: non-virtual).
			// These are BRUSH-ONLY functions (no outlines with pen).
			// The Ellipse and Triangle require SetLRArray(), or else nothing shows.
			void DirectRectangle( int32_t x0,int32_t y0,int32_t x1,int32_t y1 );
			void DirectEllipse( int32_t x0,int32_t y0,int32_t x1,int32_t y1 );
			void DirectTriangle( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2 );

		private:
			Bitmaps::Colour     _bitmap;                     /// The target bitmap.
			System::Raster::RasterLR<int32_t>  *_pLRArray;   /// If allocated, it always has _bitmap.Height elements.
			Point<int32_t>     *_pPointsArray;               /// The caller allocates an array for poly scan conversion.
			size_t              _pointsArrayCapacity;        /// Capacity of _pPointsArray
			System::LineReceivers::ForBitmapOutlines                 _outlinerLineRecv;
			System::LineReceivers::ForBitmapScanConvPointCollecting  _scanCvtLineRecv;
			int32_t             _cursorX;
			int32_t             _cursorY;
		private:
			void PaintLRArrayToBitmap( Rect<int32_t> viewport );
			void PaintPointsArrayToBitmap( Point<int32_t> *pointsArray, size_t numPoints );
			System::LineReceivers::AbstractLineReceiver *GetLineReceiverForCurrentMode();
		};

		class MetafileRecorderDevice: public BaseDevice
		{
		public:

			// For recording drawing operations to a libBasic::AbstractTextOutputStream
			// in text format (a "metafile").

			explicit MetafileRecorderDevice( libBasic::AbstractTextOutputStream * );

			/// Implementing the standard API:
			ABSTRACT_DEVICE_VIRTUALS;
			virtual void StartPoly() override; /// override base
			virtual void EndPoly() override;   /// override base

			/// And we also override these to output the metafile records after calling the base:
			virtual void SetForegroundColour( uint32_t foreColour ) override;
			virtual void SetBackgroundColour( uint32_t backColour ) override;
			virtual void SetViewport( Rect<int32_t> r ) override;
			virtual void SelectPen( std::shared_ptr<Pens::AbstractPen> ) override;
			virtual void SelectBrush( std::shared_ptr<Brushes::AbstractBrush> ) override;
			virtual void SelectBitmap( std::shared_ptr<Bitmaps::AbstractBitmap> ) override;
			virtual void SelectFont( const char *fontName, uint32_t PointSizeTenths ) override;
			virtual void Text( int32_t x, int32_t y, Scaling *pScaling, const char *text, size_t charCount ) override; /// does NOT call base

		private:
			void Done( SmallStringBuilder &tmpstr );
			libBasic::AbstractTextOutputStream  *_outputTextStream;
		};

		class ExtentsMeasurementDevice: public BaseDevice
		{
		public:

			// For measuring the extents of items drawn since construction.
			// (This is not for any actual drawing!)

			// TODO: implement everything that isn't implemented yet.

			ExtentsMeasurementDevice();
			ABSTRACT_DEVICE_VIRTUALS;

			// Returns 'true' if 1 or more items drawn, and the extents of those items.
			// Returns 'false' if no items drawn, and rect (0,0,0,0).
			bool GetMeasuredExtents( Rect<int32_t> *out_extents ) const;

			// Reset the measurer
			void Clear()                      { _firstTime = true; }

		private:
			void ContributeRect( Rect<int32_t> r );
			void ContributePoint( int32_t x, int32_t y );
		private:
			Rect<int32_t>  _measuredExtents;
			bool           _firstTime;
		};

		class Rescaler: public AbstractDevice
		{
		public:
			// Re-scaling and translation
			Rescaler();

			/// Priming
			void SetTarget( AbstractDevice *pTarget );
			void SetRescale( int32_t Multiplier, int32_t Divisor );
			void SetRescale( const Scaling &sc );
			void SetTranslation( int32_t dx, int32_t dy );

			/// AbstractDevice implementation:
			virtual void SetViewport( Rect<int32_t> r ) override;
			virtual void SelectPen( std::shared_ptr<Pens::AbstractPen> ) override;
			virtual void SelectBrush( std::shared_ptr<Brushes::AbstractBrush> ) override;
			virtual void SelectBitmap( std::shared_ptr<Bitmaps::AbstractBitmap> ) override;
			virtual void SelectFont( const char *fontName, uint32_t pointSizeTenths ) override;
			virtual void SetForegroundColour( uint32_t foreColour ) override;
			virtual void SetBackgroundColour( uint32_t backColour ) override;
			virtual Rect<int32_t> GetViewport() override;
			virtual void StartPoly() override;
			virtual void EndPoly() override;
			virtual void MoveTo( int32_t x,int32_t y ) override;
			virtual void LineTo( int32_t x,int32_t y ) override;
			virtual void Arc(    Rect<int32_t> r, int32_t startAngle, int32_t endAngle ) override;
			virtual void Secant( Rect<int32_t> r, int32_t startAngle, int32_t endAngle ) override;
			virtual void Pie(    Rect<int32_t> r, int32_t startAngle, int32_t endAngle ) override;
			virtual void Ellipse( Rect<int32_t> r ) override;
			virtual void Rectangle( Rect<int32_t> r ) override;
			virtual void Bezier( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2,int32_t x3,int32_t y3 ) override;
			virtual void Triangle( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2 ) override;
			virtual void DrawBitmap( Rect<int32_t> areaOnTarget, Rect<int32_t> areaOnSource, uint32_t flags ) override;
			virtual void Text( int32_t x, int32_t y, Scaling *pScaling, const char *text, size_t charCount ) override;
			virtual std::shared_ptr<Fonts::AbstractFont> GetFont() const override;

		private:

			// These are really 32-bits, pre-sign-extended to 64-bits:
			int64_t _multiplierX;
			int64_t _divisorX;
			int64_t _multiplierY;
			int64_t _divisorY;
			int64_t _dx;
			int64_t _dy;

			AbstractDevice *_pTarget;

			Rect<int32_t>  _preScaledViewport;

		private:

			inline int32_t Scale( int32_t n, int64_t mul, int64_t div, int64_t delta )
			{
				return int32_t(   ((int64_t(n) * mul) / div) + delta   );
			}

			inline int32_t ScaleX( int32_t x )   { return Scale( x, _multiplierX, _divisorX, _dx ); }
			inline int32_t ScaleY( int32_t y )   { return Scale( y, _multiplierY, _divisorY, _dy ); }

			inline Rect<int32_t> ScaleRect( Rect<int32_t> r )
			{
				return Rect<int32_t>(
					ScaleX(r.left), ScaleY(r.top), ScaleX(r.right), ScaleY(r.bottom) );
			}

		};

	}  /// end namespace

} /// end namespace












// - - - Bitmap functions - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace Bitmaps
	{
		template<typename SCALAR, typename PIXEL>
		inline Rect<SCALAR>  BitmapExtents( const System::template_Bitmap<SCALAR,PIXEL> &bm )
		{
			// Get extents of bitmap
			return Rect<SCALAR>( 0, 0, bm.WidthPixels, bm.HeightPixels );
		}

		template<typename SCALAR, typename PIXEL>
		inline System::template_Bitmap<SCALAR,PIXEL>  Intersect( const System::template_Bitmap<SCALAR,PIXEL> &b, Rect<SCALAR> r )
		{
			// Return a Bitmap representing the intersection of 'r' and bitmap 'b'.
			r = Intersection( r, BitmapExtents(b) );
			if( HasArea(r) )
			{
				System::template_Bitmap<SCALAR,PIXEL> Result;
				Result.TopLeft          = System::Raster::template_CalcPixelAddress( b.TopLeft, r.left, r.top, b.BytesPerScanLine );
				Result.BytesPerScanLine = b.BytesPerScanLine;
				Result.WidthPixels      = Width(r);
				Result.HeightPixels     = Height(r);
				return Result;
			}
			return System::template_Bitmap<SCALAR,PIXEL>(); // no intersection
		}

	}

} /// end namespace






// - - - (Metafile player) - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace Meta
	{
		class Player
		{
		public:

			// A metafile player.
			// Parses metafile text and sens graphics instructions to Target Device.

			Player();

			/// Priming
			void SetSourceText( const char *sourceText );
			void SetTarget( Devices::AbstractDevice * );
			void SetMasterClippingRectangle( Rect<int32_t> r );
				/// Only "viewport" records get intersected with this.
				/// Subtlety: If you metafile has no "viewport" records, this is ignored!
			void SetBitmapBuffer( void *bufferAddress, size_t bufferSize ); /// optional

			/// Operations
			bool PlayAll();
			bool PlaySingleStep();

			/// Query state
			const char *GetPos() const                         { return _pos; }
			Devices::AbstractDevice *GetTarget() const         { return _pTarget; }
			Rect<int32_t> GetMasterClippingRectangle() const   { return _masterViewport; }

		private:

			const char  *_pos; /// Parser position
			Devices::AbstractDevice  *_pTarget; /// Target device to draw to.

			/// "Master" viewport for clipping the metafile:
			Rect<int32_t> _masterViewport;

			/// For now we'll have these literally:
			std::shared_ptr<Brushes::Solid>         _solidBrush;
			std::shared_ptr<Brushes::AverageMixed>  _avgMixBrush;
			std::shared_ptr<Brushes::Patterned>     _pattBrush;
			std::shared_ptr<Pens::Solid>            _solidPen;
			uint16_t  _pattBrushPattern[16];

			/// Bitmap buffer is optional.  This is used in preference to heap, if possible:
			void    *_pBitmapBuffer;
			size_t   _bitmapBufferSizeBytes;

			/// Stores for currently selected bitmap:
			std::shared_ptr<Bitmaps::Mono>         _monoBitmap;
			std::shared_ptr<Bitmaps::Colour>       _colourBitmap;

		};

	} /// end namespace

} /// end namespace
























#undef ABSTRACT_DEVICE_VIRTUALS
#undef ABSTRACT_SHAPE_VIRTUALS
#undef ABSTRACT_OUTLINE_VIRTUALS
#undef ABSTRACT_PEN_VIRTUALS
#undef ABSTRACT_BITMAP_VIRTUALS
#undef ABSTRACT_BRUSH_VIRTUALS
