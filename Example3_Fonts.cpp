//
// Showcasing a palette using stretched-bitmap painting.
//


#include <fstream>
#include <string>
#include <iostream>
#include "libGraphics_Main.h"
#include "ColoursEnum.h"




const int DemoBitmapWidth = 640;
const int DemoBitmapHeight = 480;





bool SaveMemoryToFile(const std::string &filename, const void *data, size_t const bytes)
{
	std::ofstream b_stream( filename.c_str(), std::fstream::out | std::fstream::binary );
	if (b_stream)
	{
		b_stream.write((const char *) data, bytes);
		return b_stream.good();
	}
	return false;
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    FONT SERVER  -- TODO:  I have not made a font server provision in the library itself, it is separate!
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


class TestFontServer: public libGraphics::Fonts::AbstractFontServer
{
public:
	static void Init();
	virtual std::shared_ptr<libGraphics::Fonts::AbstractFont> AddRefFont( const char *fontName, uint32_t pointSizeTenths ) override;
	virtual bool ReleaseFont( std::shared_ptr<libGraphics::Fonts::AbstractFont> font ) override;
};



// The font server will only serve these fonts:
std::shared_ptr<libGraphics::Fonts::Fixed8byNFont>  g_pSystemFont;
std::shared_ptr<libGraphics::Fonts::Fixed8byNFont>  g_pCamLynxFont;




void TestFontServer::Init()
{
	// SYSTEM 80
	g_pSystemFont = std::make_shared<libGraphics::Fonts::Fixed8byNFont>( 80, g_GEMSystem8x14Font,  14,  0,255,  10 );

	// LYNX font
	g_pCamLynxFont = std::make_shared<libGraphics::Fonts::Fixed8byNFont>( 80, g_CamputersLynxIIFont,  10,  32,126,  8 );
}



std::shared_ptr<libGraphics::Fonts::AbstractFont>  TestFontServer::AddRefFont( const char *fontName, uint32_t pointSizeTenths )
{
	// - FontName is the name of the font the caller desires.
	// - PointSizeTenths gives the font size desired.
	
	// In this simple font server, we don't even look at the 'PointSizeTenths' parameter!
	
	if( CaseInsensitiveCompare(fontName,"System") == 0 )
	{
		return g_pSystemFont;
	}
	else if( CaseInsensitiveCompare(fontName,"Lynx") == 0 )
	{
		return g_pCamLynxFont;
	}

	return g_pSystemFont; // a default
	// We can alternatively return no font:   return nullptr;
}



bool TestFontServer::ReleaseFont( std::shared_ptr<libGraphics::Fonts::AbstractFont> fontToRelease )
{
	if( fontToRelease == g_pSystemFont || fontToRelease == g_pCamLynxFont ) // <-- TODO: not proper "addref/release" faking but is close enough
	{
		return true;
	}
	return false;
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    REST OF PROGRAM !
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

void DrawPalette( uint32_t *colourStrip, uint32_t numColours, libGraphics::Devices::AbstractDevice &dc, int32_t widthOfDisplay, int32_t heightOfDisplay )
{
	int32_t  lx = widthOfDisplay / 10;
	int32_t  ly = heightOfDisplay / 10;

	// Create a bitmap object that refers to the colourStrip memory space:
	auto theBitmap = std::make_shared<libGraphics::Bitmaps::Colour>(
		colourStrip, numColours, 1, numColours * 4 );

	// Select the bitmap and draw it:
	auto targetRect = Rect<int32_t>( lx, ly, lx*9, ly*9 );
	auto sourceRect = Rect<int32_t>( 0, 0, theBitmap->WidthPixels, theBitmap->HeightPixels );
	dc.SelectBitmap( theBitmap );
	dc.DrawBitmap( targetRect, sourceRect, 0 );
	dc.SelectBitmap( nullptr );
}




 
int main()
{
	//
	// In this version of the library, the 32-bpp bitmap class "libGraphics::Bitmaps::Colour"
	// doesn't allocate bitmap memory.  So we do it separately:
	//
	
	auto sizeBytes = DemoBitmapWidth * DemoBitmapHeight * 4;
	void *demoBitmapMemory = malloc( sizeBytes );
	if( demoBitmapMemory == nullptr ) return 1;

	// Just ensure memory is clean.
	memset( demoBitmapMemory, 0, sizeBytes );
	
	// Now set up a bitmap object to reference this memory:
	libGraphics::Bitmaps::Colour  theColourBitmap(
		(uint32_t *) demoBitmapMemory,
		DemoBitmapWidth, DemoBitmapHeight,
		DemoBitmapWidth * 4 ); // <-- inter-row offset, in bytes

	// Create a "device" object that will allow drawing routines to operate
	// without knowing what they're drawing on:
	libGraphics::Devices::BitmapDevice  theBitmapDevice( theColourBitmap );
	
	// Create a "font server":  TODO: Should really be a high-level library responsibility to provide this.
	TestFontServer::Init();
	TestFontServer  testFontServer; // <-- This is the font server instance.

	// Plug the "Font server" into the device object, so that it request font definitions:
	theBitmapDevice.SetFontServer( testFontServer );

	//
	// Graphics start here...
	//
	
	DrawPalette( g_ColourStripData, 16, theBitmapDevice, DemoBitmapWidth, DemoBitmapHeight );

	// Set the colour for the text:
	dc.SetForegroundColour( libBasic::Colours::Cyan );
	
	// Select a font, and write a string.
	// The nullptr means no scaling is provided, so font will render 1:1 definition-to-pixels.
	dc.SelectFont( "System",80 );
	dc.Text( 20,20, nullptr, "This is a test of System font.", 34 );
	
	// Select another colour and show how to write text scaled by a ratio:
	dc.SetForegroundColour( libBasic::Colours::Red );

	auto scaleRatio = libGraphics::Scaling( 4, 2 );
	dc.Text( 20,240, &scaleRatio, "This is a test of System font.", 34 );
	
	// Save the file as ".data" so you can import it into GIMP and use the RAW import
	// format.  You would enter 640*480 as the dimensions, and set the format to RGBA:

	return SaveMemoryToFile( "output.data", demoBitmapMemory, sizeBytes ) ? 0 : 1;
}
