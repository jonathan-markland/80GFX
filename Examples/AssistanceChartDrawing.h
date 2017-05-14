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
// AssistanceChartDrawing.h
//

#pragma once

typedef std::vector<int32_t> VectorOfInt32;

void DrawFontDemo(
	lib80GFX::Devices::AbstractDevice &theDevice );

void DrawHorizontalPalette( 
	lib80GFX::Devices::AbstractDevice &theDevice,
	uint32_t *colourStrip, 
	uint32_t numColours, 
	int32_t widthOfDisplay, 
	int32_t heightOfDisplay );

void DrawPalette(
	uint32_t *colourStrip, uint32_t numColours, 
	lib80GFX::Devices::AbstractDevice &theDevice, 
	int32_t widthOfDisplay, int32_t heightOfDisplay );

void DrawWithTheDirectGraphicsFunctions(
	lib80GFX::Devices::BitmapDevice &theBitmapDevice,
	int32_t demoBitmapWidth,
	int32_t demoBitmapHeight );

void DrawBarChart( 
	lib80GFX::Devices::AbstractDevice &theDevice, 
	const VectorOfInt32 *dataVector, int32_t projectionWidth, int32_t projectionHeight );

void DrawPieChart( 
	lib80GFX::Devices::AbstractDevice &theDevice, 
	const VectorOfInt32 *dataVector, int32_t projectionWidth, int32_t projectionHeight );

void DrawFilledPolygonWithHoles( 
	lib80GFX::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight );

void DrawFilledPolygonWithHoles2( 
	lib80GFX::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight );

void DrawBrushesDemo( 
	lib80GFX::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight );

void DrawOverLinesTest(
	lib80GFX::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight );

void DrawCapitalLetterA(
	lib80GFX::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight );

void DrawOmega(
	lib80GFX::Devices::AbstractDevice &theDevice, 
	int32_t projectionWidth, int32_t projectionHeight );
