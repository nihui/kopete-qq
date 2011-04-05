#include "evagroup.h"

#include <stdlib.h>

GroupNameOpPacket::GroupNameOpPacket() : SessionOutPacket()
{
    m_command = 0x0001;
}

GroupNameOpPacket::~GroupNameOpPacket()
{
}

void GroupNameOpPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// cmd 1F
    fill1( buf, pos, 0x1f );

    /// position 01
    fill1( buf, pos, 0x01 );

    /// 00 00 00 00
    fill4( buf, pos, 0x00000000 );
}

GroupNameOpReplyPacket::GroupNameOpReplyPacket() : SessionInPacket()
{
    m_command = 0x0001;

    m_entries = 0;
    m_entryCount = 0;
}

GroupNameOpReplyPacket::~GroupNameOpReplyPacket()
{
    for ( int i = 0; i < m_entryCount; ++i )
        free( m_entries[i].groupName );
    free( m_entries );
}

void GroupNameOpReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char b1;
    /// cmd 1f
    extract1( buf, pos, &b1 );

    /// position
    extract1( buf, pos, &b1 );

    /// unknown 00 00 00 34 00 00
    skipN( buf, pos, 6 );

    /// FIXME: no hardcode to most 10 groups
    m_entries = (NameEntry*)malloc( 20 * sizeof(NameEntry) );

    while ( *pos + 3 < len ) {
        /// group id
        extract1( buf, pos, &(m_entries[m_entryCount].groupId) );

        /// unknown flag 01
        extract1( buf, pos, &b1 );

        /// size of group name
        unsigned char groupNameSize;
        extract1( buf, pos, &groupNameSize );

        /// group name
        unsigned char* groupName = (unsigned char*)malloc( groupNameSize + 1 );
        extractN( buf, pos, groupName, groupNameSize );
        groupName[ groupNameSize ] = '\0';
        m_entries[m_entryCount].groupName = groupName;

        ++m_entryCount;
    }
}
