#include "evauhtransfer.h"

#include <stdlib.h>

UHTransferRequestPacket::UHTransferRequestPacket() : UHOutPacket()
{
    m_command = 0x02;
}

UHTransferRequestPacket::~UHTransferRequestPacket()
{
}

void UHTransferRequestPacket::fillBody( unsigned char* buf, int* pos )
{
    /// 8 bytes zero
    zeroN( buf, pos, 8 );

    /// size of data below
    fill2( buf, pos, 17 );

    /// buddy qq id
    fill4( buf, pos, m_QQId );

    /// fixed 0x01, count of the session id below ?
    fill1( buf, pos, 0x01 );

    /// session id
    fill4( buf, pos, m_sessionId );

    /// position, all 0xff
    fill4( buf, pos, 0xffffffff );

    /// requested data length, all 0x00
    fill4( buf, pos, 0x00000000 );
}

UHTransferPacket::UHTransferPacket() : UHInPacket()
{
    m_command = 0x02;

    m_imageDataSize = 0;
    m_imageData = NULL;
}

UHTransferPacket::~UHTransferPacket()
{
    free( m_imageData );
}

void UHTransferPacket::extractBody( const unsigned char* buf, int* pos, int len )
{
    /// 8 bytes zero
    skipN( buf, pos, 8 );

    /// size of data below
//     extract2( buf, pos, &dataSize );
    skipN( buf, pos, 2 );

    /// fixed 0x00, separator ?
    skipN( buf, pos, 1 );

    /// buddy qq id
    extract4( buf, pos, &m_QQId );

    /// session id
    extract4( buf, pos, &m_sessionId );

    /// avatar file size
    extract4( buf, pos, &m_avatarSize );

    /// position
    extract4( buf, pos, &m_position );

    /// remaining image data size
    extract4( buf, pos, &m_imageDataSize );

    /// image data
    m_imageData = (unsigned char*)malloc( m_imageDataSize );
    extractN( buf, pos, m_imageData, m_imageDataSize );
}
