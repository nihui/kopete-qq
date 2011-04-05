#include "evalogout.h"

LogoutPacket::LogoutPacket() : SessionOutPacket()
{
    m_command = 0x0062;
}

LogoutPacket::~LogoutPacket()
{
}

void LogoutPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// 16 bytes zero
    zeroN( buf, pos, 16 );
}
