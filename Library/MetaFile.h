
// Metafile support:   Input and Output serialisation to/from a simple text format.

#pragma once

#include "Geometric.h"
#include "Abstractions.h"
#include "SmallStringBuilder.h"
#include "NumStr.h"


// --------------------------------------------------------------------------------------------------------
//    SERIALISATION -- IN
// --------------------------------------------------------------------------------------------------------

namespace libBasic
{
	namespace MetaIn
	{
		// - - TAGS and VALUES - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		bool ParseMetaCmd(   const char *&sourcePosition, const char *commandName );
		bool ParseMetaField( const char *&sourcePosition, int8_t   &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, uint8_t  &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, int16_t  &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, uint16_t &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, int32_t  &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, uint32_t &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, int64_t  &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, uint64_t &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, Size<int32_t> &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, Point<int32_t> &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, Rect<int32_t> &fieldReference );
		bool ParseMetaField( const char *&sourcePosition, bool &fieldReference );

		// - - BINARY - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		bool ParseKernelString(    const char *&sourcePosition, SmallStringBuilder *out_pString );
		bool ParseBinary(          const char *&sourcePosition, void *destinationBuffer, size_t destinationBufferLength );
		bool ParseByte(            const char *&sourcePosition, uint8_t &out_Value );
		bool ParseBinaryEndMarker( const char *&sourcePosition );

	} /// end namespace

} /// end namespace



// Convenience macros for parsing:

#define METAREAD_CMD( TAGNAME )                if( ! libBasic::MetaIn::ParseMetaCmd( _pos, TAGNAME ) ) return false;
#define METAREAD_PARSE_FIELD( DEST )           if( ! libBasic::MetaIn::ParseMetaField( _pos, DEST ) ) return false;
#define METAREAD_PARSE_BINARY( DEST, LENGTH )  if( ! libBasic::MetaIn::ParseBinary( _pos, DEST, LENGTH ) ) return false;








// --------------------------------------------------------------------------------------------------------
//    SERIALISATION -- OUT
// --------------------------------------------------------------------------------------------------------

#define META_IN                 ">"   /// indent
#define META_OUT                "<"   /// un-indent

namespace libBasic
{
	namespace MetaOut
	{
		// - - TAGS and VALUES - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Clears the tmpstr and returns the TagName in the tmpstr.
		// Hint: This is then ready for Add() to be called to append value(s) (if desired).
		// Hint: Call Done() when complete.
		void Start( SmallStringBuilder &tmpstr, const char *tagName );

		// Append a numeric value to a tag begun with Start().
		// Hint: Call Done() when all additions complete.
		void Add( SmallStringBuilder &tmpstr, intptr_t value );

		// Flushes the completed Tag and values string to the output.
		void Done( SmallStringBuilder &str, libBasic::AbstractTextOutputStream *outputStream );

		// - - BINARY - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// Starts a fresh binary-output sequence using tmpstr as line storage.
		// Call AddBinary() to add chunks, passing the same tmpstr.
		void StartBinary( SmallStringBuilder &tmpstr );

		// Call AddBinary() as many times as desired to append binary chunk(s)
		// to the output.  Call FlushBinary() when all done, passing the same tmpstr.
		void AddBinary( SmallStringBuilder &tmpstr, const void *dataBuffer, const void *dataBufferEnd, libBasic::AbstractTextOutputStream *outputStream );

		// Call once all binary has been written with AddBinary().
		// Flushes any remaining bytes in 'tmpstr' to the output.
		void FlushBinary( SmallStringBuilder &tmpstr, libBasic::AbstractTextOutputStream *outputStream );
	}
}
