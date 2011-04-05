#ifndef EVALEVEL_H
#define EVALEVEL_H

#include "evapacket.h"

class GetLevelPacket : public SessionOutPacket
{
    public:
        explicit GetLevelPacket();
        virtual ~GetLevelPacket();
        void setQQId( unsigned int qq ) { m_QQId = qq; }
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
    private:
        unsigned int m_QQId;
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
