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
// Assistance routines for the example programs.
//

#include "AssistanceForExamples.h"
#include <fstream>
#include <iostream>
#include "80GFX_libGraphics_Main.h"
#include "80GFX_libGraphics_Fonts.h"
#include "80GFX_Resources_FixedFont.h"





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





void OutputToStandardOut::Write( const char *message )
{
	// An abstract interface through which text can be reported.
	// Reminder: Does NOT apply a new-line.
	std::cout << message;
}




OutputToFile::OutputToFile( const char *filePath )
{
	_body = new std::ofstream( filePath );
}



OutputToFile::~OutputToFile()
{
	auto pStream = (std::ofstream *) _body;
	if(pStream != nullptr)
	{
		pStream->close();
		delete pStream;
		_body = nullptr;
	}
}



void OutputToFile::Write( const char *message )
{
	auto &theStream = *((std::ofstream *) _body);
	theStream << message;
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    FONT SERVER
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// This simple font server will only serve these font objects:

std::shared_ptr<lib80GFX::Fonts::Fixed8byNFont>  g_pSystemFont;
std::shared_ptr<lib80GFX::Fonts::Fixed8byNFont>  g_pCamLynxFont;

	// Note: The Font object must provide 1-bpp bitmaps for each character,
	//       upon request, but all that's hidden in the library!  You could
	//       make your own kind of font *class* to support a new font format,
	//       as well as providing new font definitions in any supported format.



void TestFontServer::Init()
{
	// SYSTEM 80
	g_pSystemFont = std::make_shared<lib80GFX::Fonts::Fixed8byNFont>( 
		80,     // Font size (for SelectFont() call)
		g_PcBios8x16Font,   // Font definition.  (As you would expect, just array of bytes in character order).
		16,     // Height of cell (pixels)
		0,255,  // Character code low/high range supported (ie: defines array indexing).
		10 );   // Offset of the base-line (from top) in pixels.

	// LYNX font
	g_pCamLynxFont = std::make_shared<lib80GFX::Fonts::Fixed8byNFont>( 
		80, g_CamputersLynxIIFont,  10,  32,126,  7 );
}



std::shared_ptr<lib80GFX::Fonts::AbstractFont>  TestFontServer::AddRefFont( const char *fontName, uint32_t pointSizeTenths )
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



bool TestFontServer::ReleaseFont( std::shared_ptr<lib80GFX::Fonts::AbstractFont> fontToRelease )
{
	if( fontToRelease == g_pSystemFont || fontToRelease == g_pCamLynxFont ) // <-- TODO: not proper "addref/release" usage-count faking but is close enough
	{
		return true;
	}
	return false;
}



