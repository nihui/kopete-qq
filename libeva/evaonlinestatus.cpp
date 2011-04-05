#include "evaonlinestatus.h"

#include <QByteArray>
#include <QDebug>

#include <stdlib.h>

GetOnlineFriendsPacket::GetOnlineFriendsPacket() : SessionOutPacket()
{
    m_command = 0x0027;
}

GetOnlineFriendsPacket::~GetOnlineFriendsPacket()
{
}

void GetOnlineFriendsPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// 02
    fill1( buf, pos, 0x02 );

    /// 00 00 00 00
    fill4( buf, pos, 0x00000000 );
}

GetOnlineFriendsReplyPacket::GetOnlineFriendsReplyPacket() : SessionInPacket()
{
    m_command = 0x0027;

    m_statusEntries = 0;
    m_QQIdCount = 0;
}

GetOnlineFriendsReplyPacket::~GetOnlineFriendsReplyPacket()
{
    free( m_statusEntries );
}

void GetOnlineFriendsReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char b1;
    /// ff position
    extract1( buf, pos, &b1 );

    /// FIXME: hardcode buddy count here
    m_statusEntries = (StatusEntry*)malloc( 40 * sizeof(StatusEntry) );

    while ( *pos + 42 < len ) {
        /// buddy qq id
        extract4( buf, pos, &(m_statusEntries[ m_QQIdCount ].QQId) );

        /// unknown 0x01
        extract1( buf, pos, &b1 );

        /// no longer ip
        skipN( buf, pos, 4 );

        /// no longer port
        skipN( buf, pos, 2 );

        /// fixed 0x00
        skipN( buf, pos, 1 );

        /// buddy online status
        extract1( buf, pos, &(m_statusEntries[ m_QQIdCount ].status) );

        /// client tag
        unsigned short clientTag;
        extract2( buf, pos, &clientTag );

        /// unknown key
        skipN( buf, pos, 16 );

        /// unknown 13 81
        skipN( buf, pos, 2 );

        /// ext flag
        extract1( buf, pos, &b1 );

        /// common flag
        extract1( buf, pos, &b1 );

        /// unknown 00 00
        skipN( buf, pos, 2 );

        /// ending 0x00
        skipN( buf, pos, 1 );

        /// fixed 00 00 00 00
        skipN( buf, pos, 4 );

        ++m_QQIdCount;
    }
}

FriendChangeStatusPacket::FriendChangeStatusPacket() : SessionInPacket()
{
    m_command = 0x0081;

    m_QQId = 0;
    m_newStatus = 20;
}

FriendChangeStatusPacket::~FriendChangeStatusPacket()
{
}

void FriendChangeStatusPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char b1;
    unsigned short b2;
    unsigned int b4;

    /// qq id
    extract4( buf, pos, &m_QQId );

    /// unknown 0x01
    skipN( buf, pos, 1 );

    /// ip no longer here since qq 2006, all 0x00
    skipN( buf, pos, 4 );

    /// port no longer here either
    skipN( buf, pos, 2 );

    /// unknown 0x00
    skipN( buf, pos, 1 );

    /// status
    extract1( buf, pos, &m_newStatus );

    /// client tag
    extract2( buf, pos, &b2 );

    /// unknown key
    skipN( buf, pos, 16 );

    /// unknown 2 bytes
    skipN( buf, pos, 2 );

    /// ext flag
    extract1( buf, pos, &b1 );

    /// common flag
    extract1( buf, pos, &b1 );

    /// This has a value of 0 when we've changed our status to QQ_BUDDY_ONLINE_INVISIBLE
    /// my qq id
    extract4( buf, pos, &b4 );

    qWarning() << "qqid" << m_QQId << "status changed to" << m_newStatus;
}

ChangeStatusPacket::ChangeStatusPacket()
{
    m_command = 0x000d;

    /// online normal
    m_status = 0x10;
}

ChangeStatusPacket::~ChangeStatusPacket()
{
}

void ChangeStatusPacket::fillSessionBody(unsigned char* buf, int* pos)
{
    fill1( buf, pos, m_status );

    /// status version
    fill2( buf, pos, 0x0000 );

    /// misc status, 0x0001 fake video
    fill2( buf, pos, 0x0001 );

    /// custom status message, now empty
    fill2( buf, pos, 0x0000 );

    fill4( buf, pos, 0x00000000 );
}

ChangeStatusReplyPacket::ChangeStatusReplyPacket()
{
    m_command = 0x000d;
}

ChangeStatusReplyPacket::~ChangeStatusReplyPacket()
{
}

void ChangeStatusReplyPacket::extractSessionBody(const unsigned char* buf, int* pos, int len)
{
    unsigned char b1;
    /// result
    extract1( buf, pos, &b1 );
}
