//
// Assistance routines for the example programs.
//

#include "AssistanceForExamples.h"
#include <fstream>
#include <iostream>
#include "libGraphics_Main.h"
#include "libGraphics_Fonts.h"
#include "Resources_FixedFont.h"





bool SaveMemoryToFile( const std::string &filename, const void *data, size_t sizeBytes )
{
	std::ofstream b_stream( filename.c_str(), std::fstream::out | std::fstream::binary );
	if (b_stream)
	{
		b_stream.write( (const char *) data, sizeBytes );
		return b_stream.good();
	}
	return false;
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    FONT SERVER
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// This simple font server will only serve these font objects:

std::shared_ptr<libGraphics::Fonts::Fixed8byNFont>  g_pSystemFont;
std::shared_ptr<libGraphics::Fonts::Fixed8byNFont>  g_pCamLynxFont;

	// Note: The Font object must provide 1-bpp bitmaps for each character,
	//       upon request, but all that's hidden in the library!  You could
	//       make your own kind of font *class* to support a new font format,
	//       as well as providing new font definitions in any supported format.



void TestFontServer::Init()
{
	// SYSTEM 80
	g_pSystemFont = std::make_shared<libGraphics::Fonts::Fixed8byNFont>( 
		80,     // Font size (for SelectFont() call)
		g_PcBios8x16Font,   // Font definition.  (As you would expect, just array of bytes in character order).
		16,     // Height of cell (pixels)
		0,255,  // Character code low/high range supported (ie: defines array indexing).
		10 );   // Offset of the base-line (from top) in pixels.

	// LYNX font
	g_pCamLynxFont = std::make_shared<libGraphics::Fonts::Fixed8byNFont>( 
		80, g_CamputersLynxIIFont,  10,  32,126,  7 );
}



std::shared_ptr<libGraphics::Fonts::AbstractFont>  TestFontServer::AddRefFont( const char *fontName, uint32_t pointSizeTenths )
{
	// - FontName is the name of the font the caller desires.  Hint:  The SelectFont() function passes in this string.
	// - PointSizeTenths gives the font size desired.
	
	// In this simple font server, we don't even look at the 'PointSizeTenths' parameter!

	// Look up the name string, and return the font object:
	
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
	if( fontToRelease == g_pSystemFont || fontToRelease == g_pCamLynxFont ) // <-- TODO: not proper "addref/release" usage-count faking but is close enough
	{
		return true;
	}
	return false;
}



