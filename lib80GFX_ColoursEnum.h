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

namespace lib80GFX
{
	namespace Colours
	{
		enum Enum
		{
			// Note:  The your system may place different interpretations
			//        on the colour values.  Typically I have found that Red and Blue
			//        can swap over on raw frame buffers vs. Windows DIBSECTIONs etc...
			//
			//        Be aware:  I am not trying to solve *that* problem!
			
			White   = 0xFFFFFFFF,
			Black   = 0xFF000000,
			Blue    = 0xFFFF0000,
			Red     = 0xFF0000FF,
			Green   = 0xFF00FF00,
			Magenta = 0xFFFF00FF,
			Cyan    = 0xFFFFFF00,
			Yellow  = 0xFF00FFFF,
		};
	}
}


