#ifndef EVAFRIEND_H
#define EVAFRIEND_H

#include "evapacket.h"

class GetFriendListPacket : public SessionOutPacket
{
    public:
        explicit GetFriendListPacket();
        virtual ~GetFriendListPacket();
        void setPosition( unsigned short pos ) { m_position = pos; }
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
    private:
        unsigned short m_position;
};

class GetFriendListReplyPacket : public SessionInPacket
{
    public:
        typedef struct {
            unsigned int QQId;
            unsigned short face;
            unsigned char age;
            unsigned char gender;
            unsigned char* nickname;
        } FriendEntry;
        explicit GetFriendListReplyPacket();
        virtual ~GetFriendListReplyPacket();
        const FriendEntry* friendEntries() const { return m_friendEntries; }
        int friendEntryCount() const { return m_friendEntryCount; }
        unsigned short nextPosition() const { return m_nextPosition; }
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
    private:
        FriendEntry* m_friendEntries;
        int m_friendEntryCount;
        unsigned short m_nextPosition;
};

#endif // EVAFRIEND_H
