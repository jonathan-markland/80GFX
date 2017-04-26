
#include "BasicInputOutput.h"
#include "GraphicsDemo.h"
#include "UnitTest_Graphics.h"
#include "FileLoader.h"
#include <assert.h>


void GraphicsDemoMain( IFileOpener *courierNewTgaFontFileOpener, IFileOpener *bigFontFileOpener )
{
	//use the unit test code

    auto screenConfig = NegotiateScreenConfiguration(
        ScreenPresentationFormat::Windowed,
        ScreenRenderingMethod::SquarePixelsStretch,
        640, 480, // Host window client area dimensions requested
        640, 480, 32 );     // Guest specification

    {
        TestFontServer::Init( courierNewTgaFontFileOpener, bigFontFileOpener );

        auto testFontServer = std::make_shared<TestFontServer>();

        BasicInputOutput  basicIO;

        if( basicIO.OpenScreen( screenConfig ) )
        {
            basicIO.SetWindowTitleBarText("Graphics Library Demo");

            // Create bitmap to point to basicIO screen

            auto screenConfig = basicIO.GetFullScreenConfiguration();

            libGraphics::Bitmaps::Colour  colourBitmap(
                (uint32_t *) screenConfig.TopLeftPixelAddress,
                640, 480,
                screenConfig.RowStrafeInBytes );

            // Device context, and connecting the Font Server

            libGraphics::Devices::BitmapDevice  targetBitmapDevice( colourBitmap );
            targetBitmapDevice.SetFontServer( testFontServer );

            // Test data

            VectorOfInt32  testData = { 100,200,300,400,500 };

            // Draw graph

            Test_DrawBarChart( targetBitmapDevice, &testData, 640, 480 );

            // Draw pie

            Test_DrawPieChart( targetBitmapDevice, &testData, 640, 480 );

            // Draw text

            Test_DrawText( targetBitmapDevice );

            basicIO.Invalidate( 0, 0, 640, 480 );

            // Main loop

            basicIO.DoMainLoopUntilTermination();

            basicIO.CloseScreen();
        }
    }
}

