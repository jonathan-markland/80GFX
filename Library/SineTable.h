
#pragma once

#include <stdint.h>
#include <assert.h>

extern int32_t g_FixedPointSineTable[450];

inline int32_t FixedPointSine( uint32_t theta )
{
	assert( theta < 450 );
	return g_FixedPointSineTable[theta];
}

inline int32_t FixedPointCosine( uint32_t theta )
{
	assert( theta < 360 );
    return FixedPointSine( theta + 90 );
}
