#include "evauhinfo.h"

#include <stdlib.h>

UHInfoPacket::UHInfoPacket() : UHOutPacket()
{
    m_command = 0x04;

    m_count = 0;
}

UHInfoPacket::~UHInfoPacket()
{
}

void UHInfoPacket::fillBody( unsigned char* buf, int* pos )
{
    /// 8 bytes zero
    zeroN( buf, pos, 8 );

    /// size of data below
    fill2( buf, pos, 2 + 2 + m_count * 4 );

    /// fixed 0x01 0x00
    fill2( buf, pos, 0x0100 );

    /// buddy count
    fill2( buf, pos, m_count );

    for ( unsigned short i = 0; i < m_count; ++i ) {
        /// buddy qq id
        fill4( buf, pos, m_QQIds[i] );
    }
}

UHInfoReplyPacket::UHInfoReplyPacket() : UHInPacket()
{
    m_command = 0x04;

    m_uhInfos = NULL;
    m_uhInfoCount = 0;
}

UHInfoReplyPacket::~UHInfoReplyPacket()
{
    free( m_uhInfos );
}

void UHInfoReplyPacket::extractBody( const unsigned char* buf, int* pos, int len )
{
    /// 8 bytes zero
    skipN( buf, pos, 8 );

    /// size of data below
    unsigned short dataSize = 0;
    extract2( buf, pos, &dataSize );

    /// fixed 0x00 0x02
    skipN( buf, pos, 2 );

    /// buddy count
    extract2( buf, pos, &m_uhInfoCount );

    m_uhInfos = (UHInfo*)malloc( m_uhInfoCount * sizeof(UHInfo) );

    for ( unsigned short i = 0; i < m_uhInfoCount; ++i ) {
        /// buddy qq id
        extract4( buf, pos, &(m_uhInfos[i].QQId) );

        /// session id
        extract4( buf, pos, &(m_uhInfos[i].sessionId) );

        /// md5(avatar file)
        extractN( buf, pos, &(m_uhInfos[i].avatarMd5), 16 );
    }
}
