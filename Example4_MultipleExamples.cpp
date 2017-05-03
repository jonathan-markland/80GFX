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


 
int main()
{
	VectorOfInt32  testData = { 100,200,300,400,500 };

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

	return 0;
}


