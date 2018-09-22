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


#include "lib80GFX_Maths.h"

namespace Maths
{



    void McGillRandom::Reseed( int32_t seed1, int32_t seed2 )
    {
        _mcgn = (uint32_t) ((seed1 == 0L) ? 0L : seed1 | 1L);
        _srgn = (uint32_t) ((seed2 == 0L) ? 0L : (seed2 & 0x7FFL) | 1L);
    }



    int32_t McGillRandom::Next( )
    {
        uint32_t r1 = (_srgn >> 15) ^ _srgn;
        _srgn = (r1 << 17) ^ r1;
        _mcgn *= 69069;
        return _mcgn ^ _srgn;
    }

}
