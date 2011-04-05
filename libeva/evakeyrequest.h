#ifndef EVAKEYREQUEST_H
#define EVAKEYREQUEST_H

#include "evapacket.h"

class RequestFTKeyPacket : public SessionOutPacket
{
    public:
        explicit RequestFTKeyPacket();
        virtual ~RequestFTKeyPacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

class RequestFTKeyReplyPacket : public SessionInPacket
{
    public:
        explicit RequestFTKeyReplyPacket();
        virtual ~RequestFTKeyReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
};

#endif // EVAKEYREQUEST_H
