#include "evasignature.h"

#include <QDebug>
#include <QString>

SignaturePacket::SignaturePacket() : SessionOutPacket()
{
    m_command = 0x0067;

    m_count = 0;
}

SignaturePacket::~SignaturePacket()
{
}

void SignaturePacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// fixed 0x83
    fill1( buf, pos, 0x83 );

    /// buddy count
    fill2( buf, pos, m_count );

    for ( unsigned short i = 0; i < m_count; ++i ) {
        /// buddy qq id
        fill4( buf, pos, m_QQIds[i] );
        /// last signature update time, all 0x00 here
        zeroN( buf, pos, 4 );
    }
}

SignatureReplyPacket::SignatureReplyPacket() : SessionInPacket()
{
    m_command = 0x0067;

    m_signEntries = 0;
    m_signEntryCount = 0;
}

SignatureReplyPacket::~SignatureReplyPacket()
{
    for ( int i = 0; i < m_signEntryCount; ++i )
        free( m_signEntries[i].signature );
    free( m_signEntries );
}

void SignatureReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char b1;
    unsigned int b4;
    /// fixed 0x83
    skipN( buf, pos, 1 );

    /// result
    extract1( buf, pos, &b1 );

    /// last buddy qq id + 1
    unsigned int lastId = 0;
    extract4( buf, pos, &lastId );
    --lastId;

    /// FIXME: hardcode buddy count here
    m_signEntries = (SignEntry*)malloc( 40 * sizeof(SignEntry) );

    unsigned int qqId = 0;
    while ( qqId != lastId ) {
        /// buddy qq id
        extract4( buf, pos, &qqId );
        m_signEntries[m_signEntryCount].QQId = qqId;

        /// last signature update time
        extract4( buf, pos, &b4 );

        /// size of signature text
        unsigned char signSize;
        extract1( buf, pos, &signSize );

        /// signature text
        unsigned char* sign = (unsigned char*)malloc( signSize + 1 );
        extractN( buf, pos, sign, signSize );
        sign[ signSize ] = '\0';
        qWarning() << qqId << QString::fromUtf8( (char*)sign );
        m_signEntries[m_signEntryCount].signature = sign;

        ++m_signEntryCount;
    }
}
