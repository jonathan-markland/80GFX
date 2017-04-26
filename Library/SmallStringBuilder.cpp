//
// Kernel's string and int-to-string support.  ** Don't experiment here! **
//

#include "SmallStringBuilder.h"
#include "TemplateMemoryFunctions.h"




void SmallStringBuilder::Rewind( uint32_t sz )
{
	if( sz < m_Size )
	{
		m_Size = sz;
		m_Data[sz] = '\0';
	}
}

void SmallStringBuilder::AppendChar( char ch )
{
	uint32_t sz = m_Size;
	if( sz < Max )
	{
		m_Data[sz] = ch;
		m_Data[sz+1] = '\0';
		m_Size = sz+1;
	}
}

void Append( SmallStringBuilder &Dest, const char *s )
{
	if(s != 0)
	{
		while( *s )
		{
			Dest.AppendChar(*s);
			++s;
		}
	}
}

//
// CaseInsensitiveCompare
//

//
// CaseInsensitiveCompare
//

inline char ToLower( char c )
{
	if( c >= 'A' && c <= 'Z' )
	{
		return c + ('a' - 'A');
	}
	else return c;
}

inline char CharIdentity( char c )
{
	return c;
}

bool CaseInsensitiveCompare( char c1, char c2 )
{
	return ToLower(c1) == ToLower(c2);
}

template<class MODIFIER>
inline int32_t StringCompareCore( const char *s1, const char *s2, MODIFIER f, const char *barrier_s1 )
{
	char c1;
	char c2;
	for(;;)
	{
		if(s1 >= barrier_s1)
		{
			return 0; // we reached the barrier so they must be the same.
		}
		c1 = f( *s1 );
		c2 = f( *s2 );
		if( c1 == '\0' || c2 == '\0' ) break;
		if( c1 < c2 ) return -1;
		if( c1 > c2 ) return 1;
		// They are the same, keep looping.
		s1++;
		s2++;
	}
	if( c1 == '\0' && c2 == '\0' ) return 0; // same length
	if( c1 == '\0' ) return -1; // s1 is shorter
	return 1; // s2 is shorter
}


int32_t CaseSensitiveCompare( const char *s1, const char *s2 )
{
	// ** FIX: I suppose the end assumption could be 1 less than what we want
	return StringCompareCore( s1, s2, CharIdentity, (const char *) (~0) );
}

int32_t CaseInsensitiveCompare( const char *s1, const char *s2 )
{
	// ** FIX: I suppose the end assumption could be 1 less than what we want
	return StringCompareCore( s1, s2, ToLower, (const char *) (~0) );
}

int32_t CaseSensitiveCompareN( const char *s1, const char *s2, uintptr_t n )
{
	return StringCompareCore( s1, s2, CharIdentity, s1+n );
}

int32_t CaseInsensitiveCompareN( const char *s1, const char *s2, uintptr_t n )
{
	return StringCompareCore( s1, s2, ToLower, s1+n );
}





bool StringToInteger( const char *&pString, intptr_t Base, intptr_t *out_pValue )
{
	const char *Str = pString; // take copy locally
	*out_pValue = 0; // assume

	//
	// Skip spaces:
	//

	while( *Str == ' ' ) Str++;

	//
	// See if it's Negative:
	//

	bool Negative = (*Str == '-');
	if( Negative ) Str++;

	//
	// Skip spaces:
	//

	while( *Str == ' ' ) Str++;

	//
	// Evaluation loop.
	// DEPRECATED: This allows commas in the number, which are ignored, eg: 1,024
	//

	bool    FoundSomething = false;
	intptr_t Value = 0;
	intptr_t DigitValue;

	while( *Str != '\0' )
	{
		char Ch = *Str;

//		if( Ch != ',' )
		{
			     if((Ch >= '0') && (Ch <= '9'))   { DigitValue = (Ch-'0'); }
			else if((Ch >= 'A') && (Ch <= 'Z'))   { DigitValue = (Ch-('A'-10)); }
			else if((Ch >= 'a') && (Ch <= 'z'))   { DigitValue = (Ch-('a'-10)); }
			else break; // first unknown character breaks the loop

			if( DigitValue >= Base ) { return false; } // Value illegal for this base
			Value = (Value * Base) + DigitValue;
			FoundSomething = true;
		}

		Str++;
	}

	//
	// Return the resulting value:
	//

	if( FoundSomething )
	{
		*out_pValue = Negative ? (-Value) : (Value);
		pString = Str; // return updated pointer
		return true;
	}

	return false;
}







// Binding for using SmallStringBuilder with template_Sprintf()
inline  void template_Sprintf_Clear( SmallStringBuilder &str )
{
	str.Clear();
}

// Binding for using SmallStringBuilder with template_Sprintf()
inline  void template_Sprintf_Append( SmallStringBuilder &str, const char *s, const char *e )
{
	while( s < e )
	{
		str.AppendChar(*s);
		++s;
	}
}

// Binding for using SmallStringBuilder with template_Sprintf()
inline  void template_Sprintf_Append( SmallStringBuilder &str, const char *s )
{
	if(s) // this routine is required to accept NULLs in case of defaulted parameters or null actual parameters.
	{
		Append(str,s);
	}
}


const char *KStr( SmallStringBuilder &tempstr, const char *Pattern, const char *s1, const char *s2, const char *s3, const char *s4 )
{
	tempstr.Clear();
	template_Sprintf( tempstr, Pattern, s1,s2,s3,s4 );
	return tempstr.c_str();
}
