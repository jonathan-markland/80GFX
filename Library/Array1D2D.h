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


#pragma once

#include <stdint.h>

template<typename Item>
Item *ArrayAverageFill(
						 Item *destinationAddress,
						 Item  fillerValue,
						 Item  ValueMask,    // eg: 0xFEFEFE
						 Item  ValueShift,   // eg: 1
						 uintptr_t  itemCount)
{
	fillerValue = (fillerValue & ValueMask) >> ValueShift;
	auto d = destinationAddress;
	auto e = destinationAddress + itemCount;
	while(d != e)
	{
		*d = (((*d) & ValueMask) >> ValueShift) + fillerValue;
		++d;
	}
	return e;
}




