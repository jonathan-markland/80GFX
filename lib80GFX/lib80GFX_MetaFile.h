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


// Metafile support:   Input and Output serialisation to/from a simple text format.

#pragma once

#include "lib80GFX_Geometric.h"
#include "lib80GFX_Abstractions.h"
#include "lib80GFX_SmallStringBuilder.h"
#include "lib80GFX_NumStr.h"


// --------------------------------------------------------------------------------------------------------
//    SERIALISATION -- IN
// --------------------------------------------------------------------------------------------------------

namespace lib80GFX
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

#define METAREAD_CMD( TAGNAME )                if( ! lib80GFX::MetaIn::ParseMetaCmd( _pos, TAGNAME ) ) return false;
#define METAREAD_PARSE_FIELD( DEST )           if( ! lib80GFX::MetaIn::ParseMetaField( _pos, DEST ) ) return false;
#define METAREAD_PARSE_BINARY( DEST, LENGTH )  if( ! lib80GFX::MetaIn::ParseBinary( _pos, DEST, LENGTH ) ) return false;








// --------------------------------------------------------------------------------------------------------
//    SERIALISATION -- OUT
// --------------------------------------------------------------------------------------------------------

#define META_IN                 ">"   /// indent
#define META_OUT                "<"   /// un-indent

namespace lib80GFX
{
	namespace MetaOut
	{
		class MetafileWriter
		{
		public:
		
			explicit MetafileWriter( libBasic::AbstractTextOutputStream *outputStream );

			// - - TAGS and VALUES - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			// Clears the tmpstr and returns the TagName in the tmpstr.
			// Hint: This is then ready for Add() to be called to append value(s) (if desired).
			// Hint: Call Done() when complete.
			void Start( const char *tagName );

			// Append a numeric value to a tag begun with Start().
			// Hint: Call Done() when all additions complete.
			void Add( intptr_t value );

			// Flushes the completed Tag and values string to the output.
			void Done();

			// - - BINARY - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			// Starts a fresh binary-output sequence using tmpstr as line storage.
			// Call AddBinary() to add chunks, passing the same tmpstr.
			void StartBinary();

			// Call AddBinary() as many times as desired to append binary chunk(s)
			// to the output.  Call FlushBinary() when all done, passing the same tmpstr.
			void AddBinary( const void *dataBuffer, const void *dataBufferEnd );

			// Call once all binary has been written with AddBinary().
			// Flushes any remaining bytes in 'tmpstr' to the output.
			void FlushBinary();
			
		private:

			SmallStringBuilder  _str;
			libBasic::AbstractTextOutputStream *_outputStream;
			
		};		
	}
}
