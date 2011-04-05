#include "evafriend.h"

#include <stdlib.h>

GetFriendListPacket::GetFriendListPacket() : SessionOutPacket()
{
    m_command = 0x0126;

    m_position = 0x0000;
}

GetFriendListPacket::~GetFriendListPacket()
{
}

void GetFriendListPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// fixed 01 00
    fill2( buf, pos, 0x0100 );

    /// unknown B0 CA 12 00 B0 CA, all zero
    zeroN( buf, pos, 6 );

    /// unknown fixed 00 02
    fill2( buf, pos, 0x0002 );

    /// position
    fill2( buf, pos, m_position );

    /// fixed 00 00
    zeroN( buf, pos, 2 );
}

GetFriendListReplyPacket::GetFriendListReplyPacket() : SessionInPacket()
{
    m_command = 0x0126;

    m_friendEntries = 0;
    m_friendEntryCount = 0;
    m_nextPosition = 0x0000;
}

GetFriendListReplyPacket::~GetFriendListReplyPacket()
{
    for ( int i = 0; i < m_friendEntryCount; ++i )
        free( m_friendEntries[i].nickname );
    free( m_friendEntries );
}

void GetFriendListReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char b1;
    /// fixed 01 00 00 00 00 00 00 00 00 02
    skipN( buf, pos, 10 );

    /// position
    extract2( buf, pos, &m_nextPosition );

    /// unknown 00 00 02 c7 3a
    skipN( buf, pos, 5 );

    /// FIXME: hardcode buddy count here
    m_friendEntries = (FriendEntry*)malloc( 40 * sizeof(FriendEntry) );

    while ( *pos + 20 < len ) {
        /// buddy qq id
        extract4( buf, pos, &(m_friendEntries[m_friendEntryCount].QQId) );

        /// face
        extract2( buf, pos, &(m_friendEntries[m_friendEntryCount].face) );

        /// age
        extract1( buf, pos, &(m_friendEntries[m_friendEntryCount].age) );

        /// gender
        extract1( buf, pos, &(m_friendEntries[m_friendEntryCount].gender) );

        /// size of nickname
        unsigned char nicknameSize;
        extract1( buf, pos, &nicknameSize );

        /// nickname
        unsigned char* nickname = (unsigned char*)malloc( nicknameSize + 1 );
        extractN( buf, pos, nickname, nicknameSize );
        nickname[ nicknameSize ] = '\0';
        m_friendEntries[m_friendEntryCount].nickname = nickname;

        /// unknown flag 00 c8
        skipN( buf, pos, 2 );

        /// ext flag
        extract1( buf, pos, &b1 );

        /// common flag
        extract1( buf, pos, &b1 );

        /// unknown
        /// 07 C6 04 16 00 00 00 31
        /// 00 00 33 31 00 00 00 31
        /// 08 04 0C 04 04 09 04 00
        /// 04 00 00 2D
        skipN( buf, pos, 28 );

        ++m_friendEntryCount;
    }

    /// unknown 04 4D 86 C8 B3
    skipN( buf, pos, 5 );
}
