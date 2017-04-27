//
// Showcasing a palette using stretched-bitmap painting.
//


#include <fstream>
#include <string>
#include <iostream>
#include "libGraphics_Main.h"
#include "libGraphics_Fonts.h"
#include "ColoursEnum.h"
#include "Resources_FixedFont.h"
#include "AssistanceForExamples.h"



const int DemoBitmapWidth = 640;
const int DemoBitmapHeight = 480;


//
// Here is a statically allocated 32-bpp bitmap.
// It's just a strip of colours:
//

uint32_t g_ColourStripData[16] = 
{
  0xFF000000,   // black
  0xFF333333,   // grey 33
  0xFF575757,   // grey 57
  0xFF888888,   // grey 88
  0xFFCCCCCC,   // grey
  0xFFFFFFFF,   // white
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
};



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





//
// DrawFontDemo
//

void DrawFontDemo( libGraphics::Devices::AbstractDevice &theDevice )
{
	// Set the colour for the text:
	theDevice.SetForegroundColour( libBasic::Colours::White );
	
	// Select a font, and write a string.
	// The nullptr means no scaling is provided, so font will render 1:1 definition-to-pixels.
	theDevice.SelectFont( "System",80 );
	theDevice.Text( 320,100, nullptr, "This is a test of System font.", 30 );
	
	// Select another colour and show how to write text scaled by a ratio:

	theDevice.SetForegroundColour( libBasic::Colours::Red );
	auto scaleRatio = libGraphics::Scaling( 4, 2 );
	theDevice.Text( 100,250, &scaleRatio, "Wide text!", 10 );

	auto scaleRatio2 = libGraphics::Scaling( 2, 4 );
	theDevice.SetForegroundColour( libBasic::Colours::Black );   // Shadow
	theDevice.Text( 301,351, &scaleRatio2, "Tall text!", 10 );
	theDevice.SetForegroundColour( libBasic::Colours::Yellow );  // Text
	theDevice.Text( 300,350, &scaleRatio2, "Tall text!", 10 );
	
	// Select a different font:
	theDevice.SetForegroundColour( libBasic::Colours::Black );
	theDevice.SelectFont( "Lynx",80 );
	theDevice.Text( 300,200, nullptr, "This is the Camputers Lynx's font.", 34 );
}


 
int main()
{
	return WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "output.data", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
			DrawPalette( g_ColourStripData, 16, theDevice, DemoBitmapWidth, DemoBitmapHeight );
			DrawFontDemo( theDevice );
		}) ? 0 : 1;
}
