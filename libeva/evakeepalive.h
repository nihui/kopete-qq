#ifndef EVAKEEPALIVE_H
#define EVAKEEPALIVE_H

#include "evapacket.h"

class KeepAlivePacket : public SessionOutPacket
{
    public:
        explicit KeepAlivePacket();
        virtual ~KeepAlivePacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

class KeepAliveReplyPacket : public SessionInPacket
{
    public:
        explicit KeepAliveReplyPacket();
        virtual ~KeepAliveReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
};

#endif // EVAKEEPALIVE_H
