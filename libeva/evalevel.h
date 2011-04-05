#ifndef EVALEVEL_H
#define EVALEVEL_H

#include "evapacket.h"

class GetLevelPacket : public SessionOutPacket
{
    public:
        explicit GetLevelPacket();
        virtual ~GetLevelPacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

class GetLevelReplyPacket : public SessionInPacket
{
    public:
        explicit GetLevelReplyPacket();
        virtual ~GetLevelReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
};

#endif // EVALEVEL_H
