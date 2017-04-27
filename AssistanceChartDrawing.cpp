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
// AssistanceChartDrawing.cpp
//

#include <stdint.h>
#include <memory>
#include <vector>
#include "libGraphics_Main.h"
#include "libGraphics_Fonts.h"
#include "AssistanceChartDrawing.h"
#include "ColoursEnum.h"



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
	libGraphics::Devices::AbstractDevice &theDevice, 
	const VectorOfInt32 *dataSet, int32_t projectionWidth, int32_t projectionHeight )
{
	auto lx = projectionWidth / 10;
	auto ly = projectionHeight / 10;

	// Create pens:
	auto axisPen    = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::White );
	auto outlinePen = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::Blue );

	// Create brush:
	auto patternedBrush = std::make_shared<libGraphics::Brushes::Patterned>( 
		g_Pattern1616_RoughWeave, libBasic::Colours::Black, libBasic::Colours::Black );

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




void DrawPieChart( 
	libGraphics::Devices::AbstractDevice &theDevice, 
	const VectorOfInt32 *dataSet, int32_t projectionWidth, int32_t projectionHeight )
{
	auto lx = projectionWidth / 10;
	auto ly = projectionHeight / 10;

	// Create pens:
	auto outlinePen = std::make_shared<libGraphics::Pens::Solid>( libBasic::Colours::White );

	// Create brush:
	auto patternedBrush = std::make_shared<libGraphics::Brushes::Patterned>(
		g_Pattern1616_Balls, libBasic::Colours::Black, libBasic::Colours::Black );

	// Draw series
	theDevice.SelectPen( outlinePen );
	auto  cx = lx * 5;
	auto  cy = ly * 5;
	auto   r = std::min(lx,ly) * 3;
	auto  numberOfSlices = int32_t( dataSet->size() );
	if( numberOfSlices > 0 )
	{
		auto dataTotal = Sum( *dataSet );
		if( dataTotal > 0 )
		{
			auto pieExtentsRect = Rect<int32_t>( cx-r, cy-r, cx+r, cy+r );
			auto sumSoFar  = 0;
			auto prevAngle = 0;
			for( int32_t i = 0; i < numberOfSlices; i++ )
			{
				sumSoFar += dataSet->at(i);
				auto startAngle = prevAngle;
				auto endAngle   = (360 * sumSoFar) / dataTotal;
				prevAngle = endAngle;
				patternedBrush->Settings.ForeColour = g_ChartDrawingPalette[ i & 15 ];
				theDevice.SelectBrush( patternedBrush );
				theDevice.StartPoly();
				theDevice.Pie( pieExtentsRect, startAngle, endAngle );  // The fill (in brush)
				theDevice.EndPoly();
				theDevice.Pie( pieExtentsRect, startAngle, endAngle );  // The outline (in pen)
			}
		}
	}
}




