//
// String class with fixed storage (non-heap).
// Includes StringToInteger and compare.
//

#pragma once

#include "NumStr.h"

class SmallStringBuilder
{
public:
	SmallStringBuilder()          { Clear(); }
	operator const char *() const { return m_Data; }
	const char *c_str() const     { return m_Data; }
	uint32_t size() const         { return m_Size; }
	void Rewind( uint32_t sz );
	void Clear()                  { m_Size = 0; m_Data[0] = '\0'; }
	void AppendChar( char ch );
	enum { Max = 255 };
private:
	char m_Data[Max+1];
	uint32_t m_Size;
};

void Append( SmallStringBuilder &Dest, const char *s );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool CaseInsensitiveCompare( char c1, char c2 );
	// Case-insensitive comparison of two ASCII characters.
	// Returns 'true' if they match, 'false' if they don't.

int32_t CaseSensitiveCompare( const char *s1, const char *s2 );
int32_t CaseSensitiveCompareN( const char *s1, const char *s2, uintptr_t n );
	// Case-sensitive comparison of two ASCII strings
	// The 'N' variant compares up to first 'n' characters in the string, if they exist.
	// -1 = s1 < s2, 0 = s1 == s2, 1 = s1 > s2

int32_t CaseInsensitiveCompare( const char *s1, const char *s2 );
int32_t CaseInsensitiveCompareN( const char *s1, const char *s2, uintptr_t n );
	// Case-insensitive comparison of two ASCII strings
	// The 'N' variant compares up to first 'n' characters in the string, if they exist.
	// -1 = s1 < s2, 0 = s1 == s2, 1 = s1 > s2

bool StringToInteger( const char *&Str, intptr_t Base, intptr_t *out_pValue );
	// Convert string to integer.
	// Returns true if success and updates Str.
	// Returns false if parse or base error, and leaves Str unchanged.

const char *KStr( SmallStringBuilder &tempstr, const char *Pattern, const char *s1, const char *s2=0, const char *s3=0, const char *s4=0 );
	// Perform libBasic's template_Sprintf() into the given SmallStringBuilder 'tempstr' and
	// returns tempstr as a (const char *).  This is intended to allow use
	// of template_Sprintf() in an expression, but this requires a user-defined temporary
	// to ensure saving of stack space overall.  The user would have to ensure
	// the temporary exists for the lifetime of the expression.


