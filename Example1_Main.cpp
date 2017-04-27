
#include <fstream>
#include <string>
#include <iostream>
#include "libGraphics_Main.h"
#include "ColoursEnum.h"
#include "AssistanceForExamples.h"




const int DemoBitmapWidth = 640;
const int DemoBitmapHeight = 480;



		
void DrawWithTheDirectGraphicsFunctions( libGraphics::Devices::BitmapDevice &theBitmapDevice )
{
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

		
		
	// Demo the "direct" functions, although these are for BitmapDevice only:

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
}


	


int main()
{
	return WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "output.data", 
		[]( libGraphics::Devices::BitmapDevice &theDevice )
		{
			DrawWithTheDirectGraphicsFunctions( theDevice );
		}) ? 0 : 1;
}
