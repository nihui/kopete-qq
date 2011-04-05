#ifndef EVAUHPACKET_H
#define EVAUHPACKET_H

#include "../evautil.h"

using namespace EvaUtil;

class EvaUHPacket
{
    public:
        explicit EvaUHPacket() {}
        virtual ~EvaUHPacket() {}
        unsigned char command() const { return m_command; }
    protected:
        unsigned char m_command;
        unsigned char m_indexCount;
        unsigned char m_index;
        unsigned short m_version;
};

class UHOutPacket : public EvaUHPacket
{
    public:
        explicit UHOutPacket();
        virtual ~UHOutPacket();
        void fill( unsigned char* buf, int* len );
    protected:
        virtual void fillBody( unsigned char* buf, int* pos ) = 0;
};

class UHInPacket : public EvaUHPacket
{
    public:
        explicit UHInPacket();
        virtual ~UHInPacket();
        void extract( const unsigned char* buf, int len );
    protected:
        virtual void extractBody( const unsigned char* buf, int* pos, int len ) = 0;
};

#endif // EVAUHPACKET_H
