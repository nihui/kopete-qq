#ifndef EVAPACKET_H
#define EVAPACKET_H

#include "evautil.h"

using namespace EvaUtil;

class EvaPacket
{
    public:
        explicit EvaPacket() {}
        virtual ~EvaPacket() {}
        static unsigned short version() { return m_version; }
        static void setVersion( unsigned short v ) { m_version = v; }
        static unsigned int QQNumber() { return m_QQNumber; }
        static void setQQNumber( unsigned int qq ) { m_QQNumber = qq; }
        unsigned short command() const { return m_command; }
        void setCommand( unsigned short command ) { m_command = command; }
        unsigned short sequence() const { return m_sequence; }
        void setSequence( unsigned short sequence ) { m_sequence = sequence; }
    protected:
        unsigned short m_command;
        unsigned short m_sequence;
    private:
        static unsigned short m_version;
        static unsigned int m_QQNumber;
};

class OutPacket : public EvaPacket
{
    public:
        explicit OutPacket();
        virtual ~OutPacket();
        void fill( unsigned char* buf, int* len );
    protected:
        virtual void fillBody( unsigned char* buf, int* pos ) = 0;
};

class InPacket : public EvaPacket
{
    public:
        explicit InPacket();
        virtual ~InPacket();
        void extract( const unsigned char* buf, int len );
    protected:
        virtual void extractBody( const unsigned char* buf, int* pos, int len ) = 0;
};

class SessionOutPacket : public OutPacket
{
    public:
        explicit SessionOutPacket();
        virtual ~SessionOutPacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos ) = 0;
    private:
        virtual void fillBody( unsigned char* buf, int* pos );
};

class SessionInPacket : public InPacket
{
    public:
        explicit SessionInPacket();
        virtual ~SessionInPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len ) = 0;
    private:
        virtual void extractBody( const unsigned char* buf, int* pos, int len );
};

#endif // EVAPACKET_H
