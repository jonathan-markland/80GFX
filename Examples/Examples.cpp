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
// This is the core program for multiple examples.
//


#include <fstream>
#include <string>
#include <iostream>
#include "80GFX_libGraphics_Main.h"
#include "80GFX_libGraphics_Fonts.h"
#include "80GFX_ColoursEnum.h"
#include "80GFX_Resources_FixedFont.h"
#include "AssistanceForExamples.h"
#include "AssistanceChartDrawing.h"



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





 
int main()
{
	VectorOfInt32  testData = { 100,200,300,400,500 };

	//
	// Call the examples, emitting a bitmap file for each.
	//
	// The following will emit raw 32-bpp (640 x 480) image files.
	// These can be loaded into GIMP, which uses the ".data" extension
	// for raw files.  You'll need to specify "RGB Alpha" format, and enter
	// 640 x 480 manually in the GIMP import screen.
	//
	
	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "PaletteDemo.data", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
			DrawPalette( g_ColourStripData, 16, theDevice, DemoBitmapWidth, DemoBitmapHeight );
		})) return 1;

	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "DirectFunctionsDemo.data", 
		[]( libGraphics::Devices::BitmapDevice &theDevice )
		{
			DrawWithTheDirectGraphicsFunctions( theDevice, DemoBitmapWidth, DemoBitmapHeight );
		})) return 1;
		
	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "FontDemo.data", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
			DrawHorizontalPalette( theDevice, g_ColourStripData, 16, DemoBitmapWidth, DemoBitmapHeight );
			DrawFontDemo( theDevice );
		})) return 1;
		
	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "PieChart.data", 
		[&testData]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawPieChart( theDevice, &testData, DemoBitmapWidth, DemoBitmapHeight );
		})) return 1;

	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "BarChart.data", 
		[&testData]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawBarChart( theDevice, &testData, DemoBitmapWidth, DemoBitmapHeight );
		})) return 1;

	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "CapitalA.data", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawCapitalLetterA( theDevice, DemoBitmapWidth, DemoBitmapHeight );
		})) return 1;

	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "Omega.data", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawOmega( theDevice, DemoBitmapWidth, DemoBitmapHeight );
		})) return 1;

	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "PolyWithHoles.data", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawFilledPolygonWithHoles( theDevice, DemoBitmapWidth, DemoBitmapHeight );
		})) return 1;

	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "PolyWithHoles2.data", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawFilledPolygonWithHoles2( theDevice, DemoBitmapWidth, DemoBitmapHeight );
		})) return 1;

	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "Brushes.data", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawBrushesDemo( theDevice, DemoBitmapWidth, DemoBitmapHeight );
		})) return 1;

	if( ! WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "DrawOverLines.data", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawOverLinesTest( theDevice, DemoBitmapWidth, DemoBitmapHeight );
		})) return 1;

	//
	// Call some of the examples above, emitting a metafile instead of a bitmap.
	// Metafiles are a simple text format that I devised, and are emitted to a 
	// redirectable stream.
	//
	
	WithNewMetafileDo( DemoBitmapWidth, DemoBitmapHeight, "PieChart.metafile.txt", 
		[&testData]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawPieChart( theDevice, &testData, DemoBitmapWidth, DemoBitmapHeight );
		});

	WithNewMetafileDo( DemoBitmapWidth, DemoBitmapHeight, "CapitalA.metafile.txt", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawCapitalLetterA( theDevice, DemoBitmapWidth, DemoBitmapHeight );
		});

	WithNewMetafileDo( DemoBitmapWidth, DemoBitmapHeight, "Omega.metafile.txt", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawOmega( theDevice, DemoBitmapWidth, DemoBitmapHeight );
		});
		
	WithNewMetafileDo( DemoBitmapWidth, DemoBitmapHeight, "PaletteDemo.metafile.txt", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
			DrawPalette( g_ColourStripData, 16, theDevice, DemoBitmapWidth, DemoBitmapHeight );
		});

	WithNewMetafileDo( DemoBitmapWidth, DemoBitmapHeight, "PolyWithHoles.data", 
		[]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawFilledPolygonWithHoles( theDevice, DemoBitmapWidth, DemoBitmapHeight );
		});

	return 0;
}


