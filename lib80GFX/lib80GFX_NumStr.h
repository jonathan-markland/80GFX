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


// Num, Uns, Hex, Bin
// NumStr, UnsStr

#pragma once

#include <stdint.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    Conversion of a nibble to a value.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

extern char g_HexChars[16];

inline char NibbleToChar( uint32_t value )
{
	return g_HexChars[ value & 15 ];
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    STORAGE CLASSES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<typename INTTYPE>
class NumberAsStringStore
{
public:
    NumberAsStringStore( INTTYPE number, uint32_t base, uint32_t paddingLength, char paddingChar );
	operator const char *() const  { return _dest; }
	uintptr_t size() const         { return _size; }
protected:
	enum      { SpaceMax  = (sizeof(INTTYPE)*8) + 1 }; // Largest representation is binary.  Add 1 for the potential "-"
	char      _tmpStr[ SpaceMax + 1 ]; // plus one for null terminator
	char     *_dest;
	uintptr_t _size;
};



template<class INTTYPE>
inline INTTYPE  QuietlyGetValidNumberBase( INTTYPE base )
{
	if( base < 2 )       { return 2;  }
	else if( base > 16 ) { return 16; }
    return base;
}



template<class INTTYPE>
NumberAsStringStore<INTTYPE>::NumberAsStringStore( INTTYPE number, uint32_t base, uint32_t paddingLength, char paddingChar )
{
    bool minusSign = (number < 0);

    if( paddingLength > SpaceMax ) paddingLength = SpaceMax;
    auto padStartAddress = _tmpStr + (SpaceMax - paddingLength);

    base = QuietlyGetValidNumberBase( base );

	if( minusSign )
	{
		number = (~number) + 1;   // avoiding a compiler warning on negation (if INTTYPE is unsigned)
	}

	//
	// Start at the final slot, place a null terminator there:
	//

	char *dest = _tmpStr + SpaceMax;
	*dest = '\0';

	//
	// Squeeze out all of the digits:
	//

	for(;;)
	{
		--dest;
		*dest = NibbleToChar( uint8_t(number % base) );
		number = number / base;
		if( number == 0 ) break; // all done
	}

	if( minusSign )
	{
		--dest;
		*dest = '-';
	}

	while( dest > padStartAddress )
    {
        --dest;
        *dest = paddingChar;
    }

	char *end = _tmpStr + SpaceMax;
	_size = end - dest;
	_dest = dest;
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// Integer Number-to-string conversion classes.
//
// Conversion of INTTYPE to any base.
// This converts to (const char *)
//
// Instantiate with either a signed or unsigned type as required.
// This is an object that has an integral buffer that stores the string safely.
//

inline NumberAsStringStore<int32_t>   ToString( int32_t  signedNumber,   uint32_t base )  { return NumberAsStringStore<int32_t>(  signedNumber,   base, 0, ' ' ); }
inline NumberAsStringStore<uint32_t>  ToString( uint32_t unsignedNumber, uint32_t base )  { return NumberAsStringStore<uint32_t>( unsignedNumber, base, 0, ' ' ); }
inline NumberAsStringStore<int64_t>   ToString( int64_t  signedNumber,   uint32_t base )  { return NumberAsStringStore<int64_t>(  signedNumber,   base, 0, ' ' ); }
inline NumberAsStringStore<uint64_t>  ToString( uint64_t unsignedNumber, uint32_t base )  { return NumberAsStringStore<uint64_t>( unsignedNumber, base, 0, ' ' ); }

inline NumberAsStringStore<int32_t>     ToString( int32_t  signedNumber )            { return ToString( signedNumber,   10 ); }
inline NumberAsStringStore<uint32_t>    ToString( uint32_t unsignedNumber )          { return ToString( unsignedNumber, 10 ); }
inline NumberAsStringStore<int64_t>     ToString( int64_t  signedNumber )            { return ToString( signedNumber,   10 ); }
inline NumberAsStringStore<uint64_t>    ToString( uint64_t unsignedNumber )          { return ToString( unsignedNumber, 10 ); }
