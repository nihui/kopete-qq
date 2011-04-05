#include "evakeepalive.h"

KeepAlivePacket::KeepAlivePacket() : SessionOutPacket()
{
    m_command = 0x0058;
}

KeepAlivePacket::~KeepAlivePacket()
{
}

void KeepAlivePacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// anything
    unsigned char filling[8] = { 0x19, 0x90, 0x04, 0x22, 0x19, 0x89, 0x12, 0x08 };
    fillN( buf, pos, filling, 8 );
}

KeepAliveReplyPacket::KeepAliveReplyPacket() : SessionInPacket()
{
    m_command = 0x0058;
}

KeepAliveReplyPacket::~KeepAliveReplyPacket()
{
}

void KeepAliveReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char b1;
    unsigned short b2;
    unsigned int b4;
    /// result
    extract1( buf, pos, &b1 );

    /// online total
    extract4( buf, pos, &b4 );

    /// ip
    extractN( buf, pos, &b4, 4 );

    /// port
    extract2( buf, pos, &b2 );

    /// 0x00 0x3c
    skipN( buf, pos, 2 );

    /// time
    extract4( buf, pos, &b4 );

    /// 00 00 00 00 00
    skipN( buf, pos, 5 );
}
