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

//
// AssistanceChartDrawing.cpp
//

#include <stdint.h>
#include <memory>
#include <vector>
#include "80GFX_libGraphics_Main.h"
#include "80GFX_libGraphics_Fonts.h"
#include "80GFX_ColoursEnum.h"
#include "AssistanceChartDrawing.h"



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    DATASET OPERATIONS    TODO: Is there std lib stuff to do this nowadays?
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<typename SCALAR>
SCALAR  Sum( const std::vector<SCALAR> &vec )
{
	SCALAR  datasetSumValue = 0;
	for( auto thisValue : vec )
	{
		datasetSumValue += thisValue;
	}
	return datasetSumValue;
}



template<typename SCALAR>
SCALAR  Max( const std::vector<SCALAR> &vec )
{
	SCALAR  maximumValue = 0;
	for( auto thisValue : vec )
	{
		maximumValue = std::max( thisValue, maximumValue );
	}
	return maximumValue;
}





void DrawBackground(
	libGraphics::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight )
{
	auto blackBrush = std::make_shared<libGraphics::Brushes::Solid>( 0xFF000000 );
	
	theDevice.SelectBrush( blackBrush );
	theDevice.StartPoly();
		theDevice.Rectangle( Rect<int32_t>(0,0,projectionWidth,projectionHeight) );
	theDevice.EndPoly();
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    CHARTS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



uint32_t  g_ChartDrawingPalette[16]=
{
  0xFF33EEFF,   // yellow
  0xFF3392FF,   // orange
  0xFF2323AD,   // red
  0xFFC02681,   // magenta
  0xFFD74B2A,   // blue
  0xFFFFAF9D,   // light blue
  0xFF7AC581,   // light green
  0xFF14691D,   // green
  0xFF194A81,   // brown
  0xFFF3CDFF,   // pink
  0xFF000000,   // black
  0xFF333333,   // grey 33
  0xFF575757,   // grey 57
  0xFF888888,   // grey 88
  0xFFCCCCCC,   // grey
  0xFFFFFFFF,   // white
};





void DrawBarChart( 
	libGraphics::Devices::AbstractDevice &theDevice, 
	const VectorOfInt32 *dataSet, int32_t projectionWidth, int32_t projectionHeight )
{
	DrawBackground( theDevice, projectionWidth, projectionHeight );
	
	auto lx = projectionWidth / 10;
	auto ly = projectionHeight / 10;

	// Create pens:
	auto axisPen    = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::White );
	auto outlinePen = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::Blue );

	// Create brush:
	auto patternedBrush = std::make_shared<libGraphics::Brushes::Patterned>( 
		g_Pattern1616_RoughWeave, libBasic::Colours::Black, libBasic::Colours::Black );

	// Draw AXES
	theDevice.SelectPen( axisPen );
	theDevice.MoveTo( lx,ly );
	theDevice.LineTo( lx,ly*9 );
	theDevice.LineTo( lx*9,ly*9 );

	// Draw series
	auto numberOfBars = int32_t( dataSet->size() );
	if( numberOfBars > 0 )
	{
		auto barSpacing = (lx*8) / numberOfBars;
		auto barWidth   = (barSpacing * 12) / 16;
		auto barIndent  = (barSpacing *  2) / 16;
		auto barBottom  = (ly*9);
		auto tallestBar = Max(*dataSet);
		if( tallestBar > 0 )
		{
			theDevice.SelectPen( axisPen );

			for( int32_t i = 0; i < numberOfBars; i++ )
			{
				auto v = dataSet->at(i);
				if( v > 0 )
				{
					auto barLeft   = lx + (i * barSpacing) + barIndent;
					auto barHeight = ((ly * 8) * v) / tallestBar;
					auto barTop    = barBottom - barHeight;
					patternedBrush->Settings.ForeColour = g_ChartDrawingPalette[i & 15];
					theDevice.SelectBrush( patternedBrush );
					theDevice.StartPoly();
					theDevice.Rectangle( Rect<int32_t>( barLeft, barTop, barLeft + barWidth, barBottom ) );  // The fill (in brush)
					theDevice.EndPoly();                                                                     
					theDevice.Rectangle( Rect<int32_t>( barLeft, barTop, barLeft + barWidth, barBottom ) );  // The outline (in pen)
				}
			}
		}
	}
}




void DrawPieChart( 
	libGraphics::Devices::AbstractDevice &theDevice, 
	const VectorOfInt32 *dataSet, int32_t projectionWidth, int32_t projectionHeight )
{
	DrawBackground( theDevice, projectionWidth, projectionHeight );
	
	auto lx = projectionWidth / 10;
	auto ly = projectionHeight / 10;

	// Create brush:
	auto patternedBrush = std::make_shared<libGraphics::Brushes::Patterned>(
		g_Pattern1616_Balls, libBasic::Colours::Black, libBasic::Colours::Black );

	//auto outlineBrush = std::make_shared<libGraphics::Brushes::Patterned>(
	//	g_Pattern1616_RoughWeave, libBasic::Colours::White, libBasic::Colours::Blue );
	
	auto outlineBrush = std::make_shared<libGraphics::Brushes::Solid>( libBasic::Colours::White );
		
	// Create pens:
	// auto outlinePen = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::White );
	auto outlinePen = std::make_shared<libGraphics::Pens::ThickPen>( outlineBrush, 10 );

	// Draw series
	theDevice.SelectPen( outlinePen );
	auto  cx = lx * 5;
	auto  cy = ly * 5;
	auto   r = std::min(lx,ly) * 3;
	auto  numberOfSlices = int32_t( dataSet->size() );
	if( numberOfSlices > 0 )
	{
		auto dataTotal = Sum( *dataSet );
		if( dataTotal > 0 )
		{
			auto pieExtentsRect = Rect<int32_t>( cx-r, cy-r, cx+r, cy+r );
			auto sumSoFar  = 0;
			auto prevAngle = 0;
			for( int32_t i = 0; i < numberOfSlices; i++ )
			{
				sumSoFar += dataSet->at(i);
				auto startAngle = prevAngle;
				auto endAngle   = (360 * sumSoFar) / dataTotal;
				prevAngle = endAngle;
				patternedBrush->Settings.ForeColour = g_ChartDrawingPalette[ i & 15 ];
				theDevice.SelectBrush( patternedBrush );
				theDevice.StartPoly();
				theDevice.Pie( pieExtentsRect, startAngle, endAngle );  // The fill (in brush)
				theDevice.EndPoly();
				theDevice.Pie( pieExtentsRect, startAngle, endAngle );  // The outline (in pen)
			}
		}
	}
}






void DrawFilledPolygonWithHoles( 
	libGraphics::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight )
{
	DrawBackground( theDevice, projectionWidth, projectionHeight );
	
	//
	// Composing a filled polygon with holes from basic shapes:
	//
	//     - Rectangle
	//     - Circle (ellipse)
	//     - Pie slice
	//

	auto lx = projectionWidth / 10;
	auto ly = projectionHeight / 10;

	// Create brush:
	auto theBrush = std::make_shared<libGraphics::Brushes::Solid>( 0xFFF3CDFF );

	// Draw series
	auto  cx = lx * 5;
	auto  cy = ly * 5;
	auto  r  = std::min(lx, ly);
	auto  r2 = r * 2;
	auto  r3 = r * 3;
	auto  r4 = r * 4;

	// Calculate shape positioning:
	auto ellipseExtentsRect  = Rect<int32_t>( cx-r2, cy-r2, cx+r2, cy+r2 );
	auto pieExtentsRect      = Rect<int32_t>( cx-r3, cy-r3, cx+r3, cy+r3 );
	auto ellipse2ExtentsRect = Rect<int32_t>( cx-r4, cy-r4, cx+r4, cy+r4 );
	auto halfRectExtentsRect = Rect<int32_t>( cx-r4-10, cy-r4-10,    cx, cy+r4+10 );

	theDevice.SelectBrush( theBrush );

	// Construct the polygon:
	theDevice.StartPoly();
	
		// Note:  A pie slice is naturally a *closed* outline:
		theDevice.Pie( pieExtentsRect, 25, 85 );

		// Note:  A ellipse is naturally a *closed* outline:
		theDevice.Ellipse( ellipseExtentsRect );   // Really a circle

		// Note:  A ellipse is naturally a *closed* outline:
		theDevice.Ellipse( ellipse2ExtentsRect );  // Really a circle

		// Note:  A rectangle is naturally a *closed* outline:
		theDevice.Rectangle( halfRectExtentsRect );

	theDevice.EndPoly(); // <-- This line executes the final drawing.

	// NB: If nothing appears when you draw the polygon, the points buffer ran out of space.
	//     In the demo program, the lambda that calls this (WithNewBitmapDo) reserves this
	//     using the constant PolyScanArrayElementCount.
		// TODO: This problem needs sorting out by design!
}






void DrawFilledPolygonWithHoles2( 
	libGraphics::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight )
{
	DrawBackground( theDevice, projectionWidth, projectionHeight );
	
	auto lx = projectionWidth / 10;
	auto ly = projectionHeight / 10;

	// Create brush:
	auto theBrush = std::make_shared<libGraphics::Brushes::Solid>( 0xFFF3CDFF );

	// Draw series
	auto  cx = lx * 5;
	auto  cy = ly * 5;
	auto  r  = std::min(lx, ly);
	auto  r2 = r * 2;
	auto  r3 = r * 3;
	auto  r4 = r * 4;

	// Calculate shape positioning:
	auto ellipseExtentsRect  = Rect<int32_t>( cx-r3, cy-r2, cx+r3, cy+r2 );
	auto pieExtentsRect      = Rect<int32_t>( cx-r3, cy-r3, cx+r3, cy+r3 );
	auto outerExtentsRect    = Rect<int32_t>( cx-r4-10, cy-r4-10,  cx+r4+10, cy+r4+10 );

	theDevice.SelectBrush( theBrush );

	// Construct the polygon:
	theDevice.StartPoly();
	
		// Note:  A secant is naturally a *closed* outline:
		theDevice.Secant( pieExtentsRect, 345, 175 );  // NB:Defined in clockwise direction.

		// Note:  A ellipse is naturally a *closed* outline:
		theDevice.Ellipse( ellipseExtentsRect );   // Really an ellipse!

		// Note:  A triangle is naturally a *closed* outline:
		theDevice.Triangle( cx-r4,cy-r4, cx-r3,cy+r3, cx+r4,cy+r4 );

		// Note:  A rectangle is naturally a *closed* outline:
		theDevice.Rectangle( outerExtentsRect );

	theDevice.EndPoly(); // <-- This line executes the final drawing.

	// NB: If nothing appears when you draw the polygon, the points buffer ran out of space.
	//     In the demo program, the lambda that calls this (WithNewBitmapDo) reserves this
	//     using the constant PolyScanArrayElementCount.
		// TODO: This problem needs sorting out by design!
}








void DrawBrushesDemo( 
	libGraphics::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight )
{
	// Demonstrate the brushes

	auto lx = projectionWidth / 10;
	auto ly = projectionHeight / 10;
	auto  r  = std::min(lx, ly);

	auto DrawCircle = [&](int32_t cx, int32_t cy)
	{
		auto x = cx * lx;
		auto y = cy * ly;
		auto ellipseExtentsRect = Rect<int32_t>( x-r, y-r, x+r, y+r );
		theDevice.StartPoly();
		theDevice.Ellipse( ellipseExtentsRect );
		theDevice.EndPoly();
	};

	// Create brushes:

	auto blackBrush   = std::make_shared<libGraphics::Brushes::Solid>( 0xFF000000 );
	auto yellowBrush  = std::make_shared<libGraphics::Brushes::Solid>( 0xFF00FFFF );
	auto redBrush     = std::make_shared<libGraphics::Brushes::Solid>( 0xFF0000FF );
	
	auto cyanMixBrush = std::make_shared<libGraphics::Brushes::AverageMixed>( 0xFFFFFF00 );
	auto blueMixBrush = std::make_shared<libGraphics::Brushes::AverageMixed>( 0xFFFF0000 );
	
	auto redChannelBrush   = std::make_shared<libGraphics::Brushes::AndXor>( 0xFFFFFF00, 0x000000FF );
	auto blueChannelBrush  = std::make_shared<libGraphics::Brushes::AndXor>( 0xFF00FFFF, 0x00FF0000 );
	auto greenChannelBrush = std::make_shared<libGraphics::Brushes::AndXor>( 0xFFFF00FF, 0x0000FF00 );
	
	auto diamondBrush = std::make_shared<libGraphics::Brushes::Patterned>(
		g_Pattern1616_FilledDiamond, 0xFFFFFFFF, 0xFF000000 );

	// TODO: We could do a "tile brush" which rasterised by copying from a given 
	//       bitmap and wrapping at the edges.
		
	// Draw circles:
	
	// Solid brush:

	theDevice.SelectBrush( yellowBrush );
	DrawCircle( 2,2 );

	theDevice.SelectBrush( redBrush );
	DrawCircle( 5,2 );

	// Patterned brush:
	
	theDevice.SelectBrush( diamondBrush );
	DrawCircle( 8,2 );

	// AverageMixed brush demo: 
	
	theDevice.SelectBrush( cyanMixBrush );
	DrawCircle( 2,5 );

	theDevice.SelectBrush( yellowBrush );
	DrawCircle( 5,5 );

	theDevice.SelectBrush( cyanMixBrush ); // overlaps
	DrawCircle( 6,5 );

	theDevice.SelectBrush( blueMixBrush ); // overlaps
	DrawCircle( 5,6 );

	// Draw a white background rectangle for the following demos:
	
	theDevice.SelectBrush( blackBrush );
	theDevice.StartPoly();
	theDevice.Rectangle( Rect<int32_t>( 0*lx, 7*ly, 10*lx, 10*ly ) );
	theDevice.EndPoly();

	// AndXor brush demo (This writes the R,G,B channels separately)
	
	theDevice.SelectBrush( redChannelBrush );
	DrawCircle( 2,8 );
	theDevice.SelectBrush( greenChannelBrush );
	DrawCircle( 3,8 );
	theDevice.SelectBrush( blueChannelBrush );
	DrawCircle( 4,8 );
	
}




void private_DrawLineOutAndBack(
	libGraphics::Devices::AbstractDevice &theDevice, 
	int32_t x, int32_t y,
	int32_t dx, int32_t dy, bool drawBack )
{
	auto redPen  = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::Red );
	auto bluePen = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::Blue );

	auto ox = x + dx;
	auto oy = y + dy;
	
	if( dx > 0 ) --dx; // inclusive required
	else if( dx < 0 ) ++dx; // inclusive required
	
	if( dy > 0 ) --dy; // inclusive required
	else if( dy < 0 ) ++dy; // inclusive required
	
	theDevice.SelectPen( bluePen );
	theDevice.MoveTo( ox,oy );
	theDevice.LineTo( ox+dx,oy+dy );

	if( drawBack )
	{
		theDevice.SelectPen( redPen );
		theDevice.MoveTo( ox+dx,oy+dy );
		theDevice.LineTo( ox,oy );
	}
}



void private_DrawFanOutAndBack(
	libGraphics::Devices::AbstractDevice &theDevice, 
	int32_t x, int32_t y,
	int32_t dx, int32_t dy, bool drawBack )
{
	private_DrawLineOutAndBack( theDevice, x,y,  dx, dy, drawBack );
	private_DrawLineOutAndBack( theDevice, x,y,  dx,-dy, drawBack );
	private_DrawLineOutAndBack( theDevice, x,y, -dx, dy, drawBack );
	private_DrawLineOutAndBack( theDevice, x,y, -dx,-dy, drawBack );
	
	private_DrawLineOutAndBack( theDevice, x,y,  dy, dx, drawBack );
	private_DrawLineOutAndBack( theDevice, x,y,  dy,-dx, drawBack );
	private_DrawLineOutAndBack( theDevice, x,y, -dy, dx, drawBack );
	private_DrawLineOutAndBack( theDevice, x,y, -dy,-dx, drawBack );
}



void DrawOverLinesTest(
	libGraphics::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight )
{
	private_DrawFanOutAndBack( theDevice, 100,100, 11,6, true );
	private_DrawFanOutAndBack( theDevice, 200,100, 10,5, true );
	
	private_DrawFanOutAndBack( theDevice, 300,100, 8,8,  false );
	private_DrawFanOutAndBack( theDevice, 350,100, 0,8,  false );
	private_DrawFanOutAndBack( theDevice, 400,100, 8,0,  false );
	
	private_DrawFanOutAndBack( theDevice, 450,100, 3,3,  false );
	private_DrawFanOutAndBack( theDevice, 500,100, 0,3,  false );
	private_DrawFanOutAndBack( theDevice, 550,100, 3,0,  false );
}


	
	
	
void DrawWithTheDirectGraphicsFunctions(
	libGraphics::Devices::BitmapDevice &theBitmapDevice,
	int32_t demoBitmapWidth,
	int32_t demoBitmapHeight )
{
	DrawBackground( theBitmapDevice, demoBitmapWidth, demoBitmapHeight );
	
	// Note the colour values are 32-bpp, so you should specify RGBA where A=255
	// which will let you see the output in GIMP.  Obviously the "A" component is
	// not processed by this library, only stored.  Alpha just isn't retro enough.
	
	auto lx = demoBitmapWidth / 10;
	auto ly = demoBitmapHeight / 10;
	
	// Create pens:
	
	auto whitePen = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::White );
	auto bluePen  = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::Blue );
	
	// Create brushes:
	
	auto patternedBrush = std::make_shared<libGraphics::Brushes::Patterned>( 
		g_Pattern1616_Balls, libBasic::Colours::Yellow, libBasic::Colours::Green );

	auto solidBrush = std::make_shared<libGraphics::Brushes::Solid>( 
		libBasic::Colours::Blue );
		
	// Demo the "direct" functions, although these are for BitmapDevice only:

	theBitmapDevice.SelectBrush( solidBrush );
	theBitmapDevice.DirectRectangle( lx,ly, lx*9,ly*9 );
	
	theBitmapDevice.SelectBrush( patternedBrush );
	theBitmapDevice.DirectRectangle( lx*2,ly*2, lx*3,ly*3 );

	theBitmapDevice.SelectBrush( patternedBrush );
	theBitmapDevice.DirectEllipse( lx*5,ly*2, lx*8,ly*4 );
	theBitmapDevice.DirectTriangle( lx*2,ly*4, lx*2,ly*8, lx*4, ly*5 );
}


	
	
	
//
// DrawPalette
//
// Here's an example of drawing a 32-bpp bitmap onto a target device.
//

void DrawPalette( uint32_t *colourStrip, uint32_t numColours, libGraphics::Devices::AbstractDevice &theDevice, int32_t widthOfDisplay, int32_t heightOfDisplay )
{
	int32_t  lx = widthOfDisplay / 10;
	int32_t  ly = heightOfDisplay / 10;

	// Create a bitmap object that refers to the colourStrip memory space:
	auto theBitmap = std::make_shared<libGraphics::Bitmaps::Colour>(
		colourStrip, numColours, 1, numColours * 4 );

	// Select the bitmap and draw it:
	auto targetRect = Rect<int32_t>( lx, ly, lx*9, ly*9 );
	auto sourceRect = Rect<int32_t>( 0, 0, theBitmap->WidthPixels, theBitmap->HeightPixels );
	theDevice.SelectBitmap( theBitmap );
	theDevice.DrawBitmap( targetRect, sourceRect, 0 );
	theDevice.SelectBitmap( nullptr );
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    FONT DEMO
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



//
// DrawHorizontalPalette
//
// Here's an example of drawing a 32-bpp bitmap onto a target device.
//

void DrawHorizontalPalette( 
	libGraphics::Devices::AbstractDevice &theDevice,
	uint32_t *colourStrip, 
	uint32_t numColours, 
	int32_t widthOfDisplay, 
	int32_t heightOfDisplay )
{
	int32_t  lx = widthOfDisplay / 10;
	int32_t  ly = heightOfDisplay / 10;

	// Create a bitmap object that refers to the colourStrip memory space:
	auto theBitmap = std::make_shared<libGraphics::Bitmaps::Colour>(
		colourStrip, 1, numColours, 4 );

	// Select the bitmap and draw it:
	auto targetRect = Rect<int32_t>( lx, ly, lx*9, ly*9 );
	auto sourceRect = Rect<int32_t>( 0, 0, theBitmap->WidthPixels, theBitmap->HeightPixels );
	theDevice.SelectBitmap( theBitmap );
	theDevice.DrawBitmap( targetRect, sourceRect, 0 );
	theDevice.SelectBitmap( nullptr );
}





//
// DrawFontDemo
//
// This example shows how to select fonts, and render them.
//
// This also illustrates the scaling feature.  I can't remember
// why I put this unusual feature in, but it's there, anyway!
//

void DrawFontDemo( libGraphics::Devices::AbstractDevice &theDevice )
{
	DrawBackground( theDevice, 640, 480 ); // TODO: Sort out constants
	
	// Set the colour for the text using SetForegroundColour():

	theDevice.SetForegroundColour( libBasic::Colours::White );
	
	// Select a font, and write a string.
	// The nullptr means no scaling is provided, so font will render 1:1 definition-to-pixels:
	
	theDevice.SelectFont( "System",80 );
	theDevice.Text( 100,88, nullptr, "This is a test of System font.", 30 );
	
	// Select another colour and show how to write text scaled by a ratio:

	theDevice.SetForegroundColour( libBasic::Colours::Red );
	auto scaleRatio = libGraphics::Scaling( 4, 2 );  // TODO: It's a bit odd design to specify a scale ratio at the last moment?  Surely that's part of the select?
	theDevice.Text( 100,175, &scaleRatio, "Wide text!", 10 );

	// Large text with shadow effect:
	
	auto scaleRatio2 = libGraphics::Scaling( 6, 7 );
	theDevice.SetForegroundColour( libBasic::Colours::Black );   // Shadow
	theDevice.Text( 201,351, &scaleRatio2, "Tall text!", 10 );
	theDevice.SetForegroundColour( libBasic::Colours::Yellow );  // Text
	theDevice.Text( 200,350, &scaleRatio2, "Tall text!", 10 );
	
	// Select a different font:
	
	theDevice.SetForegroundColour( libBasic::Colours::Black );
	theDevice.SelectFont( "Lynx",80 );
	theDevice.Text( 100,210, nullptr, "This is the Camputers Lynx's font.", 34 );
}


 


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    POLYGON DEMOS - LETTER OUTLINES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 

 
void DrawCapitalLetterA(
	libGraphics::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight )
{
	auto whiteBrush = std::make_shared<libGraphics::Brushes::Solid>( 0xFFFFFFFF );
	auto blackBrush = std::make_shared<libGraphics::Brushes::Solid>( 0xFF000000 );

	theDevice.SelectBrush( whiteBrush );
	theDevice.StartPoly(); // TODO: Rename StartFilledPoly() ?
		theDevice.Rectangle( Rect<int32_t>(0,0,640,480) );
	theDevice.EndPoly(); // TODO: Rename EndFilledPoly() ?  // <-- This line executes the final drawing.
	
	theDevice.SelectBrush( blackBrush );
	theDevice.StartPoly();
	
		// Reminder: Your polygons will not draw if you don't close them.
		
		// The following has two figures:

		// Outer outline of the 'A':
		theDevice.MoveTo(286,52);
		theDevice.LineTo(348,52);
		theDevice.LineTo(491,430); 
		theDevice.LineTo(438,430); 
		theDevice.LineTo(404,335); 
		theDevice.LineTo(232,335); 
		theDevice.LineTo(196,431); 
		theDevice.LineTo(143,431);
		theDevice.LineTo(286,52); 
		
		// Inner triangle:
		theDevice.MoveTo(316,102); 
		theDevice.LineTo(386,291); 
		theDevice.LineTo(247,291); 
		theDevice.LineTo(316,102);

	theDevice.EndPoly(); // <-- This line executes the final drawing.
}



void DrawOmega(
	libGraphics::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight )
{
	auto whiteBrush = std::make_shared<libGraphics::Brushes::Solid>( 0xFFFFFFFF );
	auto blackBrush = std::make_shared<libGraphics::Brushes::Solid>( 0xFF000000 );
	auto redBrush   = std::make_shared<libGraphics::Brushes::Solid>( libBasic::Colours::Red );
	
	theDevice.SelectBrush( whiteBrush );
	theDevice.StartPoly(); // TODO: Rename StartFilledPoly() ?
		theDevice.Rectangle( Rect<int32_t>(0,0,640,480) );
	theDevice.EndPoly(); // TODO: Rename EndFilledPoly() ?  // <-- This line executes the final drawing.
	
	// TODO: Thick pen implementation uses a brush, but will not work well 
	//       if the AverageMixed brush is selected, because of nib overlaps.

	// TODO: Thick pen rendering is hackish:  It draws a filled circle at every point!
	//       On a modern machine this will do the business, but it isn't clever.  Would
	//       be great to have an algorithm to thicken an outline by returning a polygon
	//       of the thickening itself?

	// TODO: Failing the above, the thick pen could at least be faster by calculating
	//       the circle nib outline ONCE, then using translation to paint it.
	
	// TODO: Support different nib shapes for the pen?
	
	auto outlinePen = std::make_shared<libGraphics::Pens::ThickPen>( redBrush, 5 );
	theDevice.SelectPen( outlinePen );

	auto DrawOmegaOutline = [&]()
	{
		// NOTE: Not perfectly symmetrical because I read the coords by hovering
		// the mouse in GIMP, and making approximate notes rather than doing 
		// anything clever.
		
		// TODO: BezierTo() is required, which MUST set the internal 
		//       cursor position!  Then get rid of all the duplicate points!
		
		// TODO: The Arc() function does not fit well into outline polys for
		//       similar reasons, but also the difficulty of calculating the
		//       positioning(!).  Maybe support 90 degree circular arcs?

		// Reminder: Your polygons will not draw if you don't close them.
		
		theDevice.MoveTo(476,331); // 1
		theDevice.LineTo(466,370); // 2
		theDevice.Bezier(466,370   ,464,378, 462,382, 431,382); // 3
		theDevice.MoveTo(431,382); // TODO: extraneous
		theDevice.LineTo(388,383); // 4
		theDevice.Bezier(388,383   ,395,356, 407,336, 431,299); // 5
		theDevice.Bezier(431,299   ,460,259, 486,223, 486,177); // 6
		theDevice.Bezier(486,177   ,486,105, 412, 45, 320, 45); // 7
		theDevice.Bezier(320, 45   ,224, 45, 154,105, 153,176); // 8
		theDevice.Bezier(153,176   ,153,222, 179,261, 204,298); // 9
		theDevice.Bezier(204,298   ,233,338, 244,360, 249,381); // 10
		theDevice.MoveTo(249,381); // TODO: extraneous
		theDevice.LineTo(207,381); // 11
		theDevice.Bezier(207,381   ,177,382, 175,378, 171,371); // 12
		theDevice.Bezier(171,371   ,169,364, 165,345, 162,332); // 13
		theDevice.MoveTo(162,332); // TODO: extraneous
		theDevice.LineTo(147,330); 
		theDevice.LineTo(166,416); 
		theDevice.LineTo(250,416); 
		theDevice.Bezier(250,416   ,263,417, 266,417, 267,406); 
		theDevice.Bezier(267,406   ,266,369, 249,325, 233,284); 
		theDevice.Bezier(233,284   ,220,250, 208,214, 208,177); 
		theDevice.Bezier(208,177   ,208, 97, 265, 58, 318, 56); 
		theDevice.Bezier(318, 56   ,374, 56, 430, 98, 431,176); 
		theDevice.Bezier(431,176   ,431,216, 416,251, 405,283); 
		theDevice.Bezier(405,283   ,388,323, 371,368, 371,406); 
		theDevice.Bezier(371,406   ,371,417, 374,417, 387,416); 
		theDevice.MoveTo(387,416); // TODO: extraneous
		theDevice.LineTo(473,418); 
		theDevice.LineTo(491,333); 
		theDevice.LineTo(476,331);
	};
	
	theDevice.SelectBrush( blackBrush );
	theDevice.StartPoly();
	DrawOmegaOutline();
	theDevice.EndPoly(); // <-- This line executes the final drawing.  (Fill only!)
	
	DrawOmegaOutline();  // <-- This draws the outline in the pen
}



