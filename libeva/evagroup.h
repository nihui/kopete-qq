#ifndef EVAGROUP_H
#define EVAGROUP_H

#include "evapacket.h"

class GroupNameOpPacket : public SessionOutPacket
{
    public:
        explicit GroupNameOpPacket();
        virtual ~GroupNameOpPacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

class GroupNameOpReplyPacket : public SessionInPacket
{
    public:
        typedef struct {
            unsigned char groupId;
            unsigned char* groupName;
        } NameEntry;
        explicit GroupNameOpReplyPacket();
        virtual ~GroupNameOpReplyPacket();
        const NameEntry* entries() const { return m_entries; }
        int entryCount() const { return m_entryCount; }
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
    private:
        NameEntry* m_entries;
        int m_entryCount;
};

#endif // EVAGROUP_H
