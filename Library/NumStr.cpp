
#include <assert.h>
#include "NumStr.h"


char g_HexChars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };





DateTimeStringStore::DateTimeStringStore( uint64_t dateAndTime, uint32_t flags )
{
	auto outStr = _buffer;

    // 0x00YYYYMMDDHHMMSS
	NumberAsStringStore<uint64_t> str( dateAndTime, 16, 16, '0' );
    const char *s = str;
    if( flags & 2 )
    {
        outStr[0] = s[2];
        outStr[1] = s[3];
        outStr[2] = s[4];
        outStr[3] = s[5];
        outStr[4] = '-';
        outStr[5] = s[6];
        outStr[6] = s[7];
        outStr[7] = '-';
        outStr[8] = s[8];
        outStr[9] = s[9];
        outStr += 10;
    }
    if( (flags & 3) == 3 )
    {
        outStr[0] = ' ';
        outStr += 1;
    }
    if( flags & 1 )
    {
        outStr[0] = s[10];
        outStr[1] = s[11];
        outStr[2] = ':';
        outStr[3] = s[12];
        outStr[4] = s[13];
        outStr[5] = '.';
        outStr[6] = s[14];
        outStr[7] = s[15];
        outStr += 8;
    }
    *outStr = '\0';
}


