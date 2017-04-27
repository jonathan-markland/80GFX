//
// AssistanceForExamples.h
//

#pragma once

#include <string>
#include "libGraphics_Main.h"
#include "libGraphics_Fonts.h"


bool SaveMemoryToFile( const std::string &filename, const void *data, size_t sizeBytes );




class TestFontServer: public libGraphics::Fonts::AbstractFontServer
{
public:

	// TODO:  I have not made a font server provision in the Library itself yet!
	
	// This font server is very simple.  It doesn't even reference count the font usages,
	// because the font definitions that it supports are statically-allocated, and remain
	// present for the duration of the program run.
	
	static void Init();
	virtual std::shared_ptr<libGraphics::Fonts::AbstractFont> AddRefFont( const char *fontName, uint32_t pointSizeTenths ) override;
	virtual bool ReleaseFont( std::shared_ptr<libGraphics::Fonts::AbstractFont> font ) override;
};



template<typename LAMBDA>
bool WithNewBitmapDo( 
	int32_t demoBitmapWidth, int32_t demoBitmapHeight, 
	const char *outputFileName, LAMBDA drawingLambda )
{
	bool functionResult = false;
	
	//
	// In this version of the library, the 32-bpp bitmap class "libGraphics::Bitmaps::Colour"
	// doesn't allocate bitmap memory.  So we do it separately:
	//
	
	auto sizeBytes = demoBitmapWidth * demoBitmapHeight * 4;
	void *demoBitmapMemory = malloc( sizeBytes );
	if( demoBitmapMemory == nullptr ) return false;

	//
	// Just ensure memory is clean:
	//
	
	memset( demoBitmapMemory, 0, sizeBytes );

	{	
		//
		// Now set up a bitmap object to reference this memory:
		//
		
		libGraphics::Bitmaps::Colour  theColourBitmap(
			(uint32_t *) demoBitmapMemory,
			demoBitmapWidth, demoBitmapHeight,
			demoBitmapWidth * 4 ); // <-- inter-row offset, in bytes

		//
		// Create a "device" object that will allow drawing routines to operate
		// without knowing what they're drawing on:
		//
		
		libGraphics::Devices::BitmapDevice  theBitmapDevice( theColourBitmap );
		
		//
		// Create a "font server":  TODO: Should really be a high-level library responsibility to provide this.
		// Plug the "Font server" into the device object, so that it request font definitions:
		//
		
		TestFontServer::Init();
		auto testFontServer = std::make_shared<TestFontServer>(); // <-- This is the font server instance.
		theBitmapDevice.SetFontServer( testFontServer );

		//
		// Graphics start here...
		//

		drawingLambda( theBitmapDevice );
		
		// Save the file as ".data" so you can import it into GIMP and use the RAW import
		// format.  You would enter the dimensions, and set the format to RGBA:

		functionResult = SaveMemoryToFile( outputFileName, demoBitmapMemory, sizeBytes );
	}
	
	free(demoBitmapMemory);
	
	return functionResult;
}
