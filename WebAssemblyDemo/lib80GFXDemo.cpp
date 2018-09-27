#include <SDL2/SDL.h>
#include <emscripten.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>

#include "../lib80GFX/lib80GFX_Main.h"
#include "../lib80GFX/lib80GFX_Fonts.h"
#include "../lib80GFX/lib80GFX_Resources_FixedFont.h"
#include "../lib80GFX/lib80GFX_ColoursEnum.h"


typedef std::vector<int32_t> VectorOfInt32;



// If any of your polygons are not showing, increase this:
const uint32_t PolyScanArrayElementCount = 10000; // TODO: Need to eliminate the need to do this by re-design in the library.




bool SaveMemoryToFile( const std::string &filename, const void *data, size_t sizeBytes );




class OutputToStandardOut: public libBasic::AbstractTextOutputStream
{
public:
	// Reminder: Does NOT apply a new-line.
	virtual void Write( const char *message ) override;
};



class OutputToFile: public libBasic::AbstractTextOutputStream
{
public:
	// Reminder: Does NOT apply a new-line.
	OutputToFile( const char *filePath );
	~OutputToFile();
	virtual void Write( const char *message ) override;
private:
	void *_body;
};



class TestFontServer: public lib80GFX::Fonts::AbstractFontServer
{
public:

	// TODO:  I have not made a font server provision in the Library itself yet!
	
	// This font server is very simple.  It doesn't even reference count the font usages,
	// because the font definitions that it supports are statically-allocated, and remain
	// present for the duration of the program run.
	
	static void Init();
	virtual std::shared_ptr<lib80GFX::Fonts::AbstractFont> AddRefFont( const char *fontName, uint32_t pointSizeTenths ) override;
	virtual bool ReleaseFont( std::shared_ptr<lib80GFX::Fonts::AbstractFont> font ) override;
};



template<typename LAMBDA>
bool WithNewBitmapDo( 
	int32_t demoBitmapWidth, int32_t demoBitmapHeight, 
	const char *outputFileName, LAMBDA drawingLambda )
{
	bool functionResult = false;
	
	//
	// In this version of the library, the 32-bpp bitmap class "lib80GFX::Bitmaps::Colour"
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
	
	auto lrArray = new lib80GFX::System::Raster::RasterLR<int32_t>[ demoBitmapHeight ]; // TODO: avoid bare new really!
	
	{	
		//
		// Now set up a bitmap object to reference this memory:
		//
		
		lib80GFX::Bitmaps::Colour  theColourBitmap(
			(uint32_t *) demoBitmapMemory,
			demoBitmapWidth, demoBitmapHeight,
			demoBitmapWidth * 4 ); // <-- inter-row offset, in bytes

		//
		// Create a "device" object that will allow drawing routines to operate
		// without knowing what they're drawing on:
		//
		
		lib80GFX::Devices::BitmapDevice  theBitmapDevice( theColourBitmap );
		
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
	lib80GFX::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight )
{
	auto blackBrush = std::make_shared<lib80GFX::Brushes::Solid>( 0xFF000000 );
	
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
	lib80GFX::Devices::AbstractDevice &theDevice, 
	const VectorOfInt32 *dataSet, int32_t projectionWidth, int32_t projectionHeight )
{
	DrawBackground( theDevice, projectionWidth, projectionHeight );
	
	auto lx = projectionWidth / 10;
	auto ly = projectionHeight / 10;

	// Create pens:
	auto axisPen    = std::make_shared<lib80GFX::Pens::Solid>( lib80GFX::Colours::White );
	auto outlinePen = std::make_shared<lib80GFX::Pens::Solid>( lib80GFX::Colours::Blue );

	// Create brush:
	auto patternedBrush = std::make_shared<lib80GFX::Brushes::Patterned>( 
		g_Pattern1616_RoughWeave, lib80GFX::Colours::Black, lib80GFX::Colours::Black );

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

















struct context
{
    SDL_Renderer *renderer;
    int iteration;
};

void mainloop(void *arg)
{
    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;
    
    // example: draw a moving rectangle
    
    // red background
//    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//    SDL_RenderClear(renderer);
    
    // moving blue rectangle
    SDL_Rect r;
    r.x = ctx->iteration % 255;
    r.y = 50;
    r.w = 50;
    r.h = 50;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255 );
    SDL_RenderFillRect(renderer, &r );

    SDL_RenderPresent(renderer);

    ctx->iteration++;
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(255, 255, 0, &window, &renderer);

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;

    const int simulate_infinite_loop = 1; // call the function repeatedly
    const int fps = -1; // call the function as fast as the browser wants to render (typically 60fps)
    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
