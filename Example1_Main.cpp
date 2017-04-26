
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



 
int main()
{
	//std::cout << "Hello World!" << g_FixedPointSineTable[45] << std::endl;

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

	// Plug in the "Font server":
	// theBitmapDevice.SetFontServer( testFontServer );

	//
	// Graphics start here...
	//
	
	// Note the colour values are 32-bpp, so you should specify RGBA where A=255
	// which will let you see the output in GIMP.  Obviously the "A" component is
	// not processed by this library, only stored.  Alpha just isn't retro enough.
	
	// Create pens:
	auto whitePen = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::White );
	auto bluePen  = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::Blue );

	// Create brushes:
	auto patternedBrush = std::make_shared<libGraphics::Brushes::Patterned>( 
		g_Pattern1616_Balls, libBasic::Colours::Yellow, libBasic::Colours::Green );

	auto solidBrush = std::make_shared<libGraphics::Brushes::Solid>( 
		libBasic::Colours::Blue );

	// Demo the "direct" functions, although these are for bitmap devices only:

	theBitmapDevice.SelectBrush( solidBrush );
	theBitmapDevice.DirectRectangle( 0,0, 400,400 );
	
	theBitmapDevice.SelectBrush( patternedBrush );
	theBitmapDevice.DirectRectangle( 50,50, 100,100 );

	// In order to see "direct" ellipse and triangle, we must do a once-only creation
	// of a temporary array to store the edges of each raster line of these shapes.
	// This array is attached to the bitmap device.
	// It must have one entry per row in the target bitmap:
	// TODO: It's a bit annoying to have to do this!
	
	auto lrArray = new libGraphics::System::Raster::RasterLR<int32_t>[ DemoBitmapHeight ]; // TODO: avoid bare new really!
	theBitmapDevice.SetLRArray(lrArray, DemoBitmapHeight);

	// Now we can demonstrate ellipse and triangle:
	
	theBitmapDevice.SelectBrush( patternedBrush );
	theBitmapDevice.DirectEllipse( 200,50, 250,100 );
	theBitmapDevice.DirectTriangle( 400,50, 450,100, 350,125 );

	theBitmapDevice.SelectBrush( solidBrush );
	theBitmapDevice.DirectEllipse( 500,50, 600,125 );
	theBitmapDevice.DirectTriangle( 500,250, 550,275, 525,400 );
	
	// Do clean up:
	// TODO: It's a bit annoying to have to do this!
	
	theBitmapDevice.SetLRArray(nullptr, 0);
	delete [] lrArray;

	// Save the file as ".data" so you can import it into GIMP and use the RAW import
	// format.  You would enter 640*480 as the dimensions, and set the format to RGBA:

	return SaveMemoryToFile( "output.data", demoBitmapMemory, sizeBytes ) ? 0 : 1;
}
