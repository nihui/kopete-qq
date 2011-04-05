#ifndef EVAONLINESTATUS_H
#define EVAONLINESTATUS_H

#include "evapacket.h"

class GetOnlineFriendsPacket : public SessionOutPacket
{
    public:
        explicit GetOnlineFriendsPacket();
        virtual ~GetOnlineFriendsPacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

class GetOnlineFriendsReplyPacket : public SessionInPacket
{
    public:
        typedef struct {
            unsigned int QQId;
            unsigned char status;
        } StatusEntry;
        explicit GetOnlineFriendsReplyPacket();
        virtual ~GetOnlineFriendsReplyPacket();
        const StatusEntry* statusEntries() const { return m_statusEntries; }
        int QQIdCount() const { return m_QQIdCount; }
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
    private:
        StatusEntry* m_statusEntries;
        int m_QQIdCount;
};

class FriendChangeStatusPacket : public SessionInPacket
{
    public:
        explicit FriendChangeStatusPacket();
        virtual ~FriendChangeStatusPacket();
        unsigned int QQId() const { return m_QQId; }
        unsigned char newStatus() const { return m_newStatus; }
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
    private:
        unsigned int m_QQId;
        unsigned char m_newStatus;
};

class ChangeStatusPacket : public SessionOutPacket
{
    public:
        explicit ChangeStatusPacket();
        virtual ~ChangeStatusPacket();
        void setStatus( unsigned char status ) { m_status = status; }
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
    private:
        unsigned char m_status;
};

class ChangeStatusReplyPacket : public SessionInPacket
{
    public:
        explicit ChangeStatusReplyPacket();
        virtual ~ChangeStatusReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
};

#endif // EVAONLINESTATUS_H
