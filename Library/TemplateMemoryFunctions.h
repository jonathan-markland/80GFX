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

// template_strlen

template<typename CONST_IT>
uintptr_t  template_strlen( CONST_IT string )
{
	CONST_IT p = string;
	while( *p ) ++p;
	return p-string;
}

// template_strcat

template<typename IT, typename CONST_IT>
IT  template_strcat( IT Dest, CONST_IT Source )
{
	Dest += template_strlen(Dest);
	for(;;)
	{
		*Dest = *Source;
		if( *Source == 0 ) break;
		++Dest;
		++Source;
	}
	return Dest;
}

// template_strcpy

template<typename IT, typename CONST_IT>
IT template_strcpy( IT Dest, CONST_IT Source )
{
	IT save = Dest;
	for(; (*Dest = *Source) != 0; ++Source, ++Dest);
	return(save);
}

// template_memchr

template<typename CONST_IT, typename VAL>
CONST_IT  template_memchr( CONST_IT source, VAL value, uintptr_t count )
{
	for(; 0 < count; ++source, --count)
	{
		if (*source == value) return source;
	}
	return 0;
}

// template_memcmp

template<typename CONST_IT>
int template_memcmp( CONST_IT s1, CONST_IT s2, uintptr_t count )
{
	for(; 0 < count; ++s1, ++s2, --count)
	{
		if (*s1 != *s2) return (*s1 < *s2 ? -1 : +1);
	}
	return 0;
}

// template_memcpy

template<typename IT, typename CONST_IT>
IT  template_memcpy( IT destination, CONST_IT source, uintptr_t count )
{
	IT d = destination;
	IT e = d + count;
	while( d < e )
	{
		*d = *source;
		++d;
		++source;
	}
	// std::copy( source, source+count, destination );
	return destination; // per C library spec
}

// template_memset

template<typename IT, typename VAL>
IT  template_memset( IT ptr, const VAL &value, uintptr_t count )
{
	IT  f = ptr;
	IT  l = ptr+count;
	while( f < l )
	{
		*f = value;
		++f;
	}
	//std::fill( f, l, value );
	return ptr; // per C library spec
}

// template_memset0

/* template<typename T>
inline void template_memset0( T &Object )
{
	template_memset( (uint8_t *) &Object, (uint8_t) 0, sizeof T );
}*/

// template_memmove

template<typename IT, typename CONST_IT>
IT  template_memmove( IT destination, CONST_IT source, uintptr_t count )
{
	if( destination <= source )
	{
		template_memcpy( destination, source, count );
	}
	else
	{
		// std::copy_backward( source, source+count, destination+count );
		IT d = destination + count;
		CONST_IT s = source + count;
		while( d > destination )
		{
			--d;
			--s;
			*d = *s;
		}
	}
	return destination; // per C library spec
}

//
// template_Sprintf
//
// A kind of sprintf() supporting C#-style replacement of {1} {2} {3} {4} with
// up to four actual parameter strings.  The substitutions can be out-of-order.
//
// Generic type 'CHAR' is the character type, eg: char or wchar_t
//
// The generic type 'STRING' is any target string class.
// But before you can use any string class (eg: MyString) in place of 'STRING', you must ensure
// that these helper functions are written for it:
//
// - void template_Sprintf_Clear( MyString & );  // must clear a MyString object
// - void template_Sprintf_Append( MyString &s, const CHAR * );   // append null terminated string to a MyString
// - void template_Sprintf_Append( MyString &s, const CHAR *Start, const CHAR *End );   // append substring [Start..End) to a MyString
//

template<typename STRING, typename CHAR>
inline void template_Sprintf( STRING &targetString, const CHAR *patternString, const CHAR *s1=nullptr, const CHAR *s2=nullptr, const CHAR *s3=nullptr, const CHAR *s4=nullptr )
{
	template_Sprintf_Clear(targetString);
	for(;;)
	{
		// Parse as much as possible from the source patternString until '{' or end of string:
		auto startPosition = patternString;
		CHAR ch;
		for(;;)
		{
			ch = *patternString;
			if( ch == '{' || ch == 0 ) break; // non-regular character seen
			++patternString;
		}

		// Flush anything we just parsed to the output string:
		template_Sprintf_Append( targetString, startPosition, patternString );

		// Did the parser stop on the nul terminator?  If so, we're done:
		if( ch == 0 ) break;

		// Else, parser stopped because of '{' seen.
		// Find which string we're substituting in:
		ch = patternString[1];  // ch is the number
		if( ch == 0 ) break;

		// We require a closing '}':
		if( patternString[2] != '}' ) break;

		// Do substitution:
		if( ch == '1' )
		{
			template_Sprintf_Append( targetString, s1 );
		}
		else if( ch == '2' )
		{
			template_Sprintf_Append( targetString, s2 );
		}
		else if( ch == '3' )
		{
			template_Sprintf_Append( targetString, s3 );
		}
		else if( ch == '4' )
		{
			template_Sprintf_Append( targetString, s4 );
		}

		patternString += 3;
	}
}

