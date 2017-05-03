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


#include <assert.h>
#include "80GFX_MetaFile.h"

#define META_BINARY_TERMINATOR  '.'   /// terminator for binary sequences




// --------------------------------------------------------------------------------------------------------
//    SERIALISATION -- IN
// --------------------------------------------------------------------------------------------------------

namespace libBasic
{
	namespace MetaIn
	{
		namespace Internal
		{
			inline bool IsWhitespace( char c )
			{
				return c == ' ' || c == 13 || c == 10 || c == 9;
			}

			uint8_t  CharToNibble( char ch )
			{
				if( ch >= '0' && ch <= '9' ) return ch - '0';
				if( ch >= 'A' && ch <= 'F' ) return (ch - 'A') + 10;
				return 0xFF; // invalid
			}
		}



		void ParseOptionalWhitespace( const char *&sourcePosition )
		{
			while( Internal::IsWhitespace(*sourcePosition) )
			{
				++sourcePosition;
			}
		}



		bool ParseLine( const char *&sourcePosition, SmallStringBuilder &out_String )
		{
			out_String.Clear();
			if( sourcePosition && *sourcePosition )
			{
				for(;;)
				{
					char ch = *sourcePosition;
					if( ch == 0 ) break;
					if( ch == 13 )
					{
						++sourcePosition;
						if( *sourcePosition == 10 )
						{
							++sourcePosition;
							return true;
						}
						break;
					}
					out_String.AppendChar(ch);
					++sourcePosition;
				}
			}
			return false;
		}



		bool ParseMetaCmd( const char *&sourcePosition, const char *commandName )
		{
			// Parse for a metafile command name
			ParseOptionalWhitespace( sourcePosition );
			const char *rewindPosition = sourcePosition;
			while( *commandName )
			{
				if( *sourcePosition == *commandName )
				{
					++sourcePosition;
					++commandName;
				}
				else break;
			}
			if( *commandName == 0 )
			{
				return true;
			}
			sourcePosition = rewindPosition;
			return false;
		}



		namespace Internal
		{
			template<typename T>
			inline bool ParseMetaIntegerField( const char *&sourcePosition, T &fieldReference )
			{
				ParseOptionalWhitespace( sourcePosition );

				/// Datum:
				intptr_t v; // TODO: Investigate type?
				if( StringToInteger( sourcePosition, 10, &v ) )
				{
					fieldReference = T(v);
					return true;
				}
				return false; /// syntax error
			}

		} /// end namespace Internal

		bool ParseMetaField( const char *&sourcePosition, int8_t   &fieldReference )     { return Internal::ParseMetaIntegerField<int8_t>(   sourcePosition, fieldReference ); }
		bool ParseMetaField( const char *&sourcePosition, uint8_t  &fieldReference )     { return Internal::ParseMetaIntegerField<uint8_t>(  sourcePosition, fieldReference ); }
		bool ParseMetaField( const char *&sourcePosition, int16_t  &fieldReference )     { return Internal::ParseMetaIntegerField<int16_t>(  sourcePosition, fieldReference ); }
		bool ParseMetaField( const char *&sourcePosition, uint16_t &fieldReference )     { return Internal::ParseMetaIntegerField<uint16_t>( sourcePosition, fieldReference ); }
		bool ParseMetaField( const char *&sourcePosition, int32_t  &fieldReference )     { return Internal::ParseMetaIntegerField<int32_t>(  sourcePosition, fieldReference ); }
		bool ParseMetaField( const char *&sourcePosition, uint32_t &fieldReference )     { return Internal::ParseMetaIntegerField<uint32_t>( sourcePosition, fieldReference ); }
		bool ParseMetaField( const char *&sourcePosition, int64_t  &fieldReference )     { return Internal::ParseMetaIntegerField<int64_t>(  sourcePosition, fieldReference ); }
		bool ParseMetaField( const char *&sourcePosition, uint64_t &fieldReference )     { return Internal::ParseMetaIntegerField<uint64_t>( sourcePosition, fieldReference ); }

		bool ParseMetaField( const char *&sourcePosition, Size<int32_t> &fieldReference )
		{
			if( ! ParseMetaField( sourcePosition, fieldReference.cx ) ) return false;
			if( ! ParseMetaField( sourcePosition, fieldReference.cy ) ) return false;
			return true;
		}

		bool ParseMetaField( const char *&sourcePosition, Point<int32_t> &fieldReference )
		{
			if( ! ParseMetaField( sourcePosition, fieldReference.x ) ) return false;
			if( ! ParseMetaField( sourcePosition, fieldReference.y ) ) return false;
			return true;
		}

		bool ParseMetaField( const char *&sourcePosition, Rect<int32_t> &fieldReference )
		{
			if( ! ParseMetaField( sourcePosition, fieldReference.left ) ) return false;
			if( ! ParseMetaField( sourcePosition, fieldReference.top ) ) return false;
			if( ! ParseMetaField( sourcePosition, fieldReference.right ) ) return false;
			if( ! ParseMetaField( sourcePosition, fieldReference.bottom ) ) return false;
			return true;
		}

		bool ParseMetaField( const char *&sourcePosition, bool &fieldReference )
		{
			ParseOptionalWhitespace( sourcePosition );

			/// Datum:
			if( *sourcePosition == '1' ) { fieldReference = true;  ++sourcePosition; return true; }
			if( *sourcePosition == '0' ) { fieldReference = false; ++sourcePosition; return true; }
			--sourcePosition;
			return false; /// syntax error
		}



		bool ParseKernelString( const char *&sourcePosition, SmallStringBuilder *out_pString )
		{
			assert( out_pString != 0 );
			out_pString->Clear();
			uint8_t v = 0;
			while( ParseByte( sourcePosition, v ) )
			{
				out_pString->AppendChar(char(v));
			}
			return ParseBinaryEndMarker( sourcePosition );
		}



		bool ParseBinary( const char *&sourcePosition, void *destinationBuffer, size_t destinationBufferLength )
		{
			auto destinationPosition = reinterpret_cast<uint8_t *>(destinationBuffer);
			auto destinationEnd      = destinationPosition + destinationBufferLength;

			while( destinationPosition < destinationEnd )
			{
				if( ! ParseByte( sourcePosition, *destinationPosition ) ) return false; /// syntax error
				++destinationPosition;
			}

			return ParseBinaryEndMarker( sourcePosition );
		}




		bool ParseBinaryEndMarker( const char *&sourcePosition )
		{
			// Textualised binary data is terminated with META_BINARY_TERMINATOR
			// as a delimiter, but also to allow representation of
			// zero-length data.
			if( *sourcePosition == META_BINARY_TERMINATOR )
			{
				++sourcePosition;
				return true;
			}
			return false;
		}



		bool ParseByte( const char *&sourcePosition, uint8_t &out_Value )
		{
			ParseOptionalWhitespace( sourcePosition );

			/// Parse most significant nibble of the byte:
			uint8_t value = Internal::CharToNibble(*sourcePosition);
			if( value != 0xFF )
			{
				uint8_t highNibbleValue = value << 4;
				++sourcePosition;

				/// Parse least significant nibble of the byte:
				value = Internal::CharToNibble(*sourcePosition);
				if( value != 0xFF )
				{
					out_Value = highNibbleValue | value;
					++sourcePosition;
					return true;
				}
			}

			return false;
		}



	} /// end namespace

} /// end namespace












// --------------------------------------------------------------------------------------------------------
//    SERIALISATION -- OUT
// --------------------------------------------------------------------------------------------------------

namespace libBasic
{
	namespace MetaOut
	{
		// - - TAGS and VALUES - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		void Start( SmallStringBuilder &tmpstr, const char *tagName )
		{
			tmpstr.Clear();
			Append( tmpstr, tagName );
		}



		void Add( SmallStringBuilder &tmpstr, intptr_t value )
		{
			tmpstr.AppendChar( ' ' );
			Append( tmpstr, ToString(value) );
		}



		void Done( SmallStringBuilder &str, libBasic::AbstractTextOutputStream *outputStream )
		{
			str.AppendChar( '\n' );
			outputStream->Write( str.c_str() );
			str.Clear();
		}




		// - - BINARY - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		void StartBinary( SmallStringBuilder &tmpstr )
		{
			tmpstr.Clear();
		}



		void AddBinary( SmallStringBuilder &str, const void *pData, const void *pEnd, libBasic::AbstractTextOutputStream *outputStream )
		{
			auto pData8 = reinterpret_cast<const uint8_t *>(pData);
			auto pEnd8  = reinterpret_cast<const uint8_t *>(pEnd);

			while( pData8 != pEnd8 )
			{
				if( str.size() == 64 )
				{
					// Flush:
					str.AppendChar('\n');
					outputStream->Write(str.c_str());
					str.Clear();
				}
				uint8_t v = *pData8;
				str.AppendChar( NibbleToChar(v >> 4) );
				str.AppendChar( NibbleToChar(v & 15) );
				++pData8;
			}
		}



		void FlushBinary( SmallStringBuilder &str, libBasic::AbstractTextOutputStream *outputStream )
		{
			str.AppendChar( META_BINARY_TERMINATOR );
			Done( str, outputStream );
		}

	}
}





#undef META_BINARY_TERMINATOR


