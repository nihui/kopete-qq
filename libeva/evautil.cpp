#include "evautil.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <winsock.h>
#else // _WIN32
#include <arpa/inet.h>
#endif // _WIN32

void EvaUtil::fill1( unsigned char* buf, int* pos, unsigned char n )
{
    buf[ *pos ] = n;
    *pos = *pos + 1;
}

void EvaUtil::fill2( unsigned char* buf, int* pos, unsigned short n )
{
    unsigned short tmp = htons( n );
    memcpy( buf + *pos, &tmp, 2 );
    *pos = *pos + 2;
}

void EvaUtil::fill4( unsigned char* buf, int* pos, unsigned int n )
{
    unsigned int tmp = htonl( n );
    memcpy( buf + *pos, &tmp, 4 );
    *pos = *pos + 4;
}

void EvaUtil::fillN( unsigned char* buf, int* pos, const void* array, int len )
{
    memcpy( buf + *pos, array, len );
    *pos = *pos + len;
}

void EvaUtil::zeroN( unsigned char* buf, int* pos, int len )
{
    memset( buf + *pos, 0x00, len );
    *pos = *pos + len;
}

void EvaUtil::randN( unsigned char* buf, int* pos, int len )
{
    unsigned int tmp;
    srand( time( NULL ) + rand() );
    for ( int i = 0; i < len / 4; ++i ) {
        tmp = rand();
        memcpy( buf + *pos, &tmp, 4 );
    }
    memset( buf + *pos, rand(), len % 4 );
    *pos = *pos + len;
}

void EvaUtil::extract1( const unsigned char* buf, int* pos, unsigned char* n )
{
    *n = buf[ *pos ];
    *pos = *pos + 1;
}

void EvaUtil::extract2( const unsigned char* buf, int* pos, unsigned short* n )
{
    unsigned short tmp;
    memcpy( &tmp, buf + *pos, 2 );
    *n = ntohs( tmp );
    *pos = *pos + 2;
}

void EvaUtil::extract4( const unsigned char* buf, int* pos, unsigned int* n )
{
    unsigned int tmp;
    memcpy( &tmp, buf + *pos, 4 );
    *n = ntohl( tmp );
    *pos = *pos + 4;
}

void EvaUtil::extractN( const unsigned char* buf, int* pos, void* array, int len )
{
    memcpy( array, buf + *pos, len );
    *pos = *pos + len;
}

void EvaUtil::skipN( const unsigned char* buf, int* pos, int len )
{
    (void)buf;
    *pos = *pos + len;
}

