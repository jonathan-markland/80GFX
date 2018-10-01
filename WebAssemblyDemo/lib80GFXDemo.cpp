
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

#include "AssistanceChartDrawing.h"


typedef std::vector<int32_t> VectorOfInt32;



// If any of your polygons are not showing, increase this:
const uint32_t PolyScanArrayElementCount = 10000; // TODO: Need to eliminate the need to do this by re-design in the library.











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












// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    LIBRARY
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<typename LAMBDA>
void WithSdlSurfaceDo( 
	int32_t demoBitmapWidth, int32_t demoBitmapHeight, 
	SDL_Surface *sdlSurface, LAMBDA drawingLambda )
{
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
			(uint32_t *) (sdlSurface->pixels),
			demoBitmapWidth, demoBitmapHeight,
			sdlSurface->pitch ); // <-- inter-row offset, in bytes

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
		
		// This is how to disconnect the temporary arrays from the bitmap device:
		
		theBitmapDevice.SetLRArray( nullptr, 0 );
		theBitmapDevice.SetPointsArray( nullptr, 0 );
	}

	delete [] lrArray;
	delete [] polygonScanConversionArray;
}










// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    STUFF FOR DEMO
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
















// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    Emscripten / SDL stuff
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



VectorOfInt32  g_testData = { 100,200,300,400,500 };



struct context
{
    SDL_Renderer *renderer;
    int iteration;
};



void mainloop(void *arg)
{
    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;
    
    // Create surface and draw to it using lib80GFX:

	auto projectionWidth = DemoBitmapWidth;
	auto projectionHeight = DemoBitmapHeight;
	
    auto theSurface = SDL_CreateRGBSurfaceWithFormat(0, projectionWidth, projectionHeight, 32, SDL_PIXELFORMAT_ABGR8888);
    if (theSurface == nullptr) return;
	
	WithSdlSurfaceDo( projectionWidth, projectionHeight, theSurface, 
		[&]( lib80GFX::Devices::AbstractDevice &theDevice )
		{
            switch((ctx->iteration >> 7) & 7)
			{
				 case 0: DrawPieChart( theDevice, &g_testData, projectionWidth, projectionHeight );  break;
				 case 1: DrawBarChart( theDevice, &g_testData, projectionWidth, projectionHeight );  break;
				 case 2: DrawFilledPolygonWithHoles( theDevice, projectionWidth, projectionHeight ); break;
				 case 3: DrawFilledPolygonWithHoles2( theDevice, projectionWidth, projectionHeight ); break;
				 case 4: DrawBrushesDemo( theDevice, projectionWidth, projectionHeight ); break;
				 case 5: DrawOverLinesTest( theDevice, projectionWidth, projectionHeight ); break;
				 case 6: DrawCapitalLetterA( theDevice, projectionWidth, projectionHeight ); break;
				 case 7: DrawOmega( theDevice, projectionWidth, projectionHeight ); break;
			}
		});

	// Create an SDL Texture from the surface, per recommendation:
	
	auto theTexture = SDL_CreateTextureFromSurface(renderer, theSurface);
	if (theTexture != nullptr)
	{
		// Red background
		
		// SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		// SDL_RenderClear(renderer);

		// Draw the texture (which has been separately supplied):
		
		SDL_Rect  targetRect;
		targetRect.x = 0;
		targetRect.y = 0;
		targetRect.w = DemoBitmapWidth;
		targetRect.h = DemoBitmapHeight;
		
		auto renderResult = SDL_RenderCopy( 
			renderer, theTexture, NULL /* render whole of source texture */, &targetRect );
			
		// int blue = 0;
		// if (renderResult == 0) blue = 255;  // Success = cyan moving box, fail = green
		
		// example: draw a moving rectangle
		
		// moving blue rectangle
		// SDL_Rect r;
		// r.x = ctx->iteration % 255;
		// r.y = 50;
		// r.w = 50;
		// r.h = 50;
		// SDL_SetRenderDrawColor(renderer, 0, 255, blue, 255 );
		// SDL_RenderFillRect(renderer, &r );

		SDL_RenderPresent(renderer);

		// Release stuff:
		
		SDL_DestroyTexture(theTexture);
		theTexture = nullptr;
	}
	
	// Release stuff:
	
	SDL_FreeSurface(theSurface);
	theSurface = nullptr;

	
    ctx->iteration++;
}




template<typename LAMBDA>
void WithSdlWindowAndRendererDo(int projectionWidth, int projectionHeight, LAMBDA lambda)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(projectionWidth, projectionHeight, 0, &window, &renderer);

	lambda(renderer);
    
    SDL_DestroyRenderer(renderer);
	renderer = nullptr;

    SDL_DestroyWindow(window);
	window = nullptr;

    SDL_Quit();
}




int main()
{
	printf("%s", "hello\n");  // Reminder: When hosted in emscripten, /n is essential -- it flushes the output!!

	WithSdlWindowAndRendererDo(
		DemoBitmapWidth, 
		DemoBitmapHeight,
		[&](SDL_Renderer *sdlRenderer)
		{
			// Create binding object for passing to the rendering loop handler:
			
			context ctx;
			ctx.renderer  = sdlRenderer;
			ctx.iteration = 0;
			
			// Ask the library to run the main loop:

			const int simulate_infinite_loop = 1; // call the function repeatedly
			const int fps = -1; // call the function as fast as the browser wants to render (typically 60fps)
			emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
		});
		
	SDL_Quit();

    return EXIT_SUCCESS;
}




