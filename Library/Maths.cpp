
#include "Maths.h"

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
