//
// AssistanceForExamples.h
//

#pragma once

#include <string>
#include "libGraphics_Main.h"
#include "libGraphics_Fonts.h"



const uint32_t PolyScanArrayElementCount = 1000;




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

	//
	// Allocate polygon scan conversion array.
	// TODO: Upgrade library:  It's a bit annoying to have to do this!
	//

	auto polygonScanConversionArray = new Point<int32_t>[ PolyScanArrayElementCount ]; // TODO: avoid bare new really!

	// In order to see "direct" ellipse and triangle, we must do a once-only creation
	// of a temporary array to store the edges of each raster line of these shapes.
	// This array is attached to the bitmap device.
	// It must have one entry per row in the target bitmap:
	// TODO: Upgrade library:  It's a bit annoying to have to do this!
	
	auto lrArray = new libGraphics::System::Raster::RasterLR<int32_t>[ demoBitmapHeight ]; // TODO: avoid bare new really!
	
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
		// Connect device to the array for polygon scan conversion:
		//

		theBitmapDevice.SetPointsArray( polygonScanConversionArray, PolyScanArrayElementCount );
		
		//
		// Connect device to "left / right edges" array, for scan converting
		// "direct" triangles and ellipses.  (This is faster rendering than
		// polygon scan conversion, because it does not require sorting points):
		//
		
		theBitmapDevice.SetLRArray( lrArray, demoBitmapHeight );
		
		//
		// Graphics start here...
		//

		drawingLambda( theBitmapDevice );
		
		// Save the file as ".data" so you can import it into GIMP and use the RAW import
		// format.  You would enter the dimensions, and set the format to RGBA:

		functionResult = SaveMemoryToFile( outputFileName, demoBitmapMemory, sizeBytes );

		// This is how to disconnect the temporary arrays from the bitmap device:
		
		theBitmapDevice.SetLRArray( nullptr, 0 );
		theBitmapDevice.SetPointsArray( nullptr, 0 );
	}

	delete [] lrArray;
	delete [] polygonScanConversionArray;
	free(demoBitmapMemory);
	
	return functionResult;
}

