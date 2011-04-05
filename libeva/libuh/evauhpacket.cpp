#include "evauhpacket.h"

#include <stdlib.h>

UHOutPacket::UHOutPacket() : EvaUHPacket()
{
}

UHOutPacket::~UHOutPacket()
{
}

void UHOutPacket::fill( unsigned char* buf, int* len )
{
    *len = 0;

    /// packet header 0x03
    fill1( buf, len, 0x03 );

    /// command
    fill1( buf, len, m_command );

    /// random
    randN( buf, len, 2 );

    /// fixed 00 00
    zeroN( buf, len, 2 );

    /// sequence
    fill2( buf, len, 0x0001 );

    /// fixed 00 00 00 00 00 00
    zeroN( buf, len, 6 );

    /// unknown flag 0x0000
    fill2( buf, len, 0x0000 );

    /// 24 bytes zero filling
    zeroN( buf, len, 24 );

    /// index count
    fill1( buf, len, m_indexCount );

    /// index
    fill1( buf, len, m_index );

    /// fixed 0x00
    fill1( buf, len, 0x00 );

    /// version
    fill2( buf, len, m_version );

    /// fixed 0x00
    fill1( buf, len, 0x00 );

    /// packet body
    int bodyLen = 0;
    fillBody( buf + *len, &bodyLen );
    *len = *len + bodyLen;
}

UHInPacket::UHInPacket() : EvaUHPacket()
{
}

UHInPacket::~UHInPacket()
{
}

void UHInPacket::extract( const unsigned char* buf, int len )
{
    int pos = 0;

    /// packet header 0x03
    skipN( buf, &pos, 1 );

    /// command
    extract1( buf, &pos, &m_command );

    /// random
    skipN( buf, &pos, 2 );

    /// fixed 00 00
    skipN( buf, &pos, 2 );

    /// sequence
    skipN( buf, &pos, 2 );

    /// fixed 00 00 00 00 00 00
    skipN( buf, &pos, 6 );

    /// data packet flag
    unsigned short flag;
    extract2( buf, &pos, &flag );

    /// 24 bytes zero
    skipN( buf, &pos, 24 );

    /// index count
    extract1( buf, &pos, &m_indexCount );

    /// index
    extract1( buf, &pos, &m_index );

    /// fixed 0x00
    skipN( buf, &pos, 1 );

    /// fixed version 0x00 0x00
    skipN( buf, &pos, 2 );

    /// fixed 0x00
    skipN( buf, &pos, 1 );

    /// packet body
    extractBody( buf, &pos, len - pos );
}
