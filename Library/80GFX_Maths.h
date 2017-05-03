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


namespace Maths
{

    class McGillRandom
    {
    public:

        // The McGill Super-Duper Random Number Generator
        // G. Marsaglia, K. Ananthanarayana, N. Paul
        //
        // Incorporating the Ziggurat method of sampling from decreasing
        // or symmetric unimodal density functions.
        // G. Marsaglia, W.W. Tsang
        //
        // Rewritten into C by E. Schneider
        //
        // Seeding two RandomNumberSource from the *same* integers
        // will cause them to generate the same sequence.
        // The sequence generated is reproducable.

        inline McGillRandom( )
        {
            Reseed( 12345, 67890 );
        }

        inline McGillRandom( int32_t seed1, int32_t seed2 )
        {
            Reseed( seed1, seed2 );
        }

        void Reseed( int32_t seed1, int32_t seed2 );

        int32_t   Next( );

        uint32_t  NextUnsigned( )
        {
            return (uint32_t) Next( );
        }

    private:

        uint32_t  _mcgn;
        uint32_t  _srgn;

    } ;



    // Return sign of argument (-1, 0, 1)

    template<typename T>
    inline T Sgn( T X )
    {
        if( X < (T) 0 ) return (T) - 1;
        if( X > (T) 0 ) return (T) 1;
        return (T) 0;
    }



    // Return absolute of argument

    template<typename T>
    inline T Abs( T X )
    {
        return (X >= 0 ) ? X : -X;
    }



}



