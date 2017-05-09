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
// Demo of "direct" triangle, ellipse and rectangle painting to a 32-bpp bitmap.
//

#include <fstream>
#include <string>
#include <iostream>
#include "80GFX_libGraphics_Main.h"
#include "80GFX_ColoursEnum.h"
#include "AssistanceForExamples.h"




const int DemoBitmapWidth = 640;
const int DemoBitmapHeight = 480;



		
void DrawWithTheDirectGraphicsFunctions( libGraphics::Devices::BitmapDevice &theBitmapDevice )
{
	// Note the colour values are 32-bpp, so you should specify RGBA where A=255
	// which will let you see the output in GIMP.  Obviously the "A" component is
	// not processed by this library, only stored.  Alpha just isn't retro enough.
	
	auto lx = DemoBitmapWidth / 10;
	auto ly = DemoBitmapHeight / 10;
	
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
	theBitmapDevice.DirectRectangle( lx,ly, lx*9,ly*9 );
	
	theBitmapDevice.SelectBrush( patternedBrush );
	theBitmapDevice.DirectRectangle( lx*2,ly*2, lx*3,ly*3 );

	theBitmapDevice.SelectBrush( patternedBrush );
	theBitmapDevice.DirectEllipse( lx*5,ly*2, lx*8,ly*4 );
	theBitmapDevice.DirectTriangle( lx*2,ly*4, lx*2,ly*8, lx*4, ly*5 );
}


	


int main()
{
	return WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "output.data", 
		[]( libGraphics::Devices::BitmapDevice &theDevice )
		{
			DrawWithTheDirectGraphicsFunctions( theDevice );
		}) ? 0 : 1;
}
