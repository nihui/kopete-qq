#include "evalevel.h"

#include <QDebug>
#include <QString>

GetLevelPacket::GetLevelPacket() : SessionOutPacket()
{
    m_command = 0x005c;

    m_QQId = 0;
}

GetLevelPacket::~GetLevelPacket()
{
}

void GetLevelPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// sub cmd type
    fill1( buf, pos, 0x88 );

    /// qq id
    fill4( buf, pos, m_QQId );

    /// fixed 0x00
    fill1( buf, pos, 0x00 );
}

GetLevelReplyPacket::GetLevelReplyPacket() : SessionInPacket()
{
    m_command = 0x005c;
}

GetLevelReplyPacket::~GetLevelReplyPacket()
{
}

void GetLevelReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char b1;
    unsigned short b2;
    unsigned int b4;
    unsigned char* str;
    /// sub cmd type
    extract1( buf, pos, &b1 );

    /// qq id
    extract4( buf, pos, &b4 );

    /// online time
    extract4( buf, pos, &b4 );

    /// level
    extract2( buf, pos, &b2 );

    /// active days
    extract2( buf, pos, &b2 );

    /// unknown 00 00 00 03
    skipN( buf, pos, 4 );

    /// size of text below
    extract2( buf, pos, &b2 );

    /// text about qq vip member
    str = (unsigned char*)malloc( b2 );
    extractN( buf, pos, str, b2 );
    qWarning() << QString::fromUtf8( (char*)str, b2 );
    free( str );

    /// size of text below
    extract2( buf, pos, &b2 );

    /// text about level
    str = (unsigned char*)malloc( b2 );
    extractN( buf, pos, str, b2 );
    qWarning() << QString::fromUtf8( (char*)str, b2 );
    free( str );

    /// size of text below
    extract2( buf, pos, &b2 );

    /// text about active days
    str = (unsigned char*)malloc( b2 );
    extractN( buf, pos, str, b2 );
    qWarning() << QString::fromUtf8( (char*)str, b2 );
    free( str );

    /// size of text below
    extract2( buf, pos, &b2 );

    /// text about next upgrade days
    str = (unsigned char*)malloc( b2 );
    extractN( buf, pos, str, b2 );
    qWarning() << QString::fromUtf8( (char*)str, b2 );
    free( str );

    /// size of text below
    extract2( buf, pos, &b2 );

    /// text about qq vip member upgrade level
    str = (unsigned char*)malloc( b2 );
    extractN( buf, pos, str, b2 );
    qWarning() << QString::fromUtf8( (char*)str, b2 );
    free( str );

    /// size of text below
    extract2( buf, pos, &b2 );

    /// text about level
    str = (unsigned char*)malloc( b2 );
    extractN( buf, pos, str, b2 );
    qWarning() << QString::fromUtf8( (char*)str, b2 );
    free( str );

    /// size of text below
    extract2( buf, pos, &b2 );

    /// text about level and vip member
    str = (unsigned char*)malloc( b2 );
    extractN( buf, pos, str, b2 );
    qWarning() << QString::fromUtf8( (char*)str, b2 );
    free( str );
}
