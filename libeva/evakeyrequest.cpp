#include "evakeyrequest.h"

RequestFTKeyPacket::RequestFTKeyPacket()
{
    m_command = 0x001d;
}

RequestFTKeyPacket::~RequestFTKeyPacket()
{
}

void RequestFTKeyPacket::fillSessionBody(unsigned char* buf, int* pos)
{
    /// FIXME TODO
    fill1( buf, pos, 0x00 );
}

RequestFTKeyReplyPacket::RequestFTKeyReplyPacket()
{
    m_command = 0x001d;
}

RequestFTKeyReplyPacket::~RequestFTKeyReplyPacket()
{
}

void RequestFTKeyReplyPacket::extractSessionBody(const unsigned char* buf, int* pos, int len)
{
    /// FIXME TODO

    unsigned char b1;
    /// key type
    extract1( buf, pos, &b1 );

    /// result
    extract1( buf, pos, &b1 );

    /// unknown 2 bytes

    /// key

    /// size of token

    /// token
}
