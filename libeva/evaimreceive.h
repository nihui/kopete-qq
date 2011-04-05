#ifndef EVAIMRECEIVE_H
#define EVAIMRECEIVE_H

#include "evapacket.h"

class ReceiveIMPacket : public SessionInPacket
{
    public:
        explicit ReceiveIMPacket();
        virtual ~ReceiveIMPacket();
        unsigned int sender() const { return m_sender; }
        unsigned int receiver() const { return m_receiver; }
        unsigned int msgseq() const { return m_msgseq; }
        unsigned int serverIp() const { return m_serverIp; }
        unsigned short messageType() const { return m_messageType; }
        const unsigned char* messageText() const { return m_messageText; }
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
    private:
        unsigned int m_sender;
        unsigned int m_receiver;
        unsigned int m_msgseq;
        unsigned int m_serverIp;
        unsigned short m_messageType;
        unsigned char* m_messageText;
};

class ReceiveIMAckPacket : public SessionOutPacket
{
    public:
        explicit ReceiveIMAckPacket();
        virtual ~ReceiveIMAckPacket();
        void setSender( unsigned int sender ) { m_sender = sender; }
        void setReceiver( unsigned int receiver ) { m_receiver = receiver; }
        void setMsgseq( unsigned int msgseq ) { m_msgseq = msgseq; }
        void setServerIp( unsigned int serverIp ) { m_serverIp = serverIp; }
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
    private:
        unsigned int m_sender;
        unsigned int m_receiver;
        unsigned int m_msgseq;
        unsigned int m_serverIp;
};

#endif // EVAIMRECEIVE_H
