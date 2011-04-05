#include "evaimsend.h"

#include <stdlib.h>
#include <time.h>

unsigned char sessionMd5[16];

SendImPacket::SendImPacket() : SessionOutPacket()
{
    m_command = 0x00cd;

    m_fontColor = 0x00000000;
    m_fontSize = 0x09;
    /// FIXME  retrieve from global settings
    m_fontName = (unsigned char*)strdup( "Sans" );
    m_msgText = (unsigned char*)strdup( "Default message of libeva." );
}

SendImPacket::~SendImPacket()
{
    free( m_fontName );
    free( m_msgText );
}

void SendImPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// qq id  FIXME
    fill4( buf, pos, 421013644 );

    /// buddy qq id
    fill4( buf, pos, m_receiver );//839028090 );

    /// unknown fixed
    const unsigned char ff[16] = {
        0x00, 0x00, 0x00, 0x0d, 0x00, 0x01, 0x00, 0x04,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01
    };
    fillN( buf, pos, ff, 16 );

    /// unknown fixed, version depended
    fill1( buf, pos, 0x08 );

    /// qq version
    fill2( buf, pos, EvaPacket::version() );

    /// qq id  FIXME
    fill4( buf, pos, 421013644 );

    /// buddy qq id
    fill4( buf, pos, m_receiver );//839028090 );

    /// md5(qqid + sessionKey)
    fillN( buf, pos, sessionMd5, 16 );

    /// message type
    fill2( buf, pos, 0x000b );

    /// message sequence
    fill2( buf, pos, 0x0000 );

    /// time
    time_t localtime = time( NULL );
    fill4( buf, pos, localtime );

    /// face
    fill2( buf, pos, 0x0000 );

    /// unknown zero 00 00 00
    zeroN( buf, pos, 3 );

    /// has font attribute
    fill1( buf, pos, 0x01 );

    /// index count
    fill1( buf, pos, 0x01 );

    /// index
    fill1( buf, pos, 0x00 );

    /// message id 00 00
    fill2( buf, pos, 0x0000 );

    /// normal or auto-reply
    fill1( buf, pos, 0x01 );

    /// "MSG" + 0x00
    fill4( buf, pos, 0x4d534700 );

    /// fixed zero
    zeroN( buf, pos, 4 );

    /// time
    fill4( buf, pos, localtime );

    /// unknown random
    fill4( buf, pos, rand() );

    /// unknown font bit
    fill1( buf, pos, 0x00 );

    /// font color rgb[2]
    fill1( buf, pos, ((unsigned char*)&m_fontColor)[2] );

    /// font color rgb[1]
    fill1( buf, pos, ((unsigned char*)&m_fontColor)[1] );

    /// font color rgb[0]
    fill1( buf, pos, ((unsigned char*)&m_fontColor)[0] );

    /// font size
    fill1( buf, pos, m_fontSize );

    /// font format FIXME
    fill1( buf, pos, 0x00 );

    /// encoding
    fill2( buf, pos, 0x8600 );

    /// size of font name
    unsigned short fontNameLen = strlen( (char*)m_fontName );
    fill2( buf, pos, fontNameLen );//0x000c );

    /// font name
    fillN( buf, pos, m_fontName, fontNameLen );// 12 );

    /// fixed zero 00 00
    zeroN( buf, pos, 2 );

    /// content type
    fill1( buf, pos, 0x01 );

    /// size of content
    unsigned short msgTextLen = strlen( (char*)m_msgText );
    fill2( buf, pos, msgTextLen + 3 );

    /// sub type
    fill1( buf, pos, 0x01 );

    /// size of message text
    fill2( buf, pos, msgTextLen );

    /// message text
    fillN( buf, pos, m_msgText, msgTextLen );
}

SendImReplyPacket::SendImReplyPacket() : SessionInPacket()
{
    m_command = 0x00cd;
}

SendImReplyPacket::~SendImReplyPacket()
{
}

void SendImReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    /// result 0x00
    skipN( buf, pos, 1 );
}
