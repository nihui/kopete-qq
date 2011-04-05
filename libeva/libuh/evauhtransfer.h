#ifndef EVAUHTRANSFER_H
#define EVAUHTRANSFER_H

#include "evauhpacket.h"

class UHTransferRequestPacket : public UHOutPacket
{
    public:
        explicit UHTransferRequestPacket();
        virtual ~UHTransferRequestPacket();
        void setQQId( unsigned int qq ) { m_QQId = qq; }
        void setSessionId( unsigned int sid ) { m_sessionId = sid; }
    protected:
        virtual void fillBody( unsigned char* buf, int* pos );
    private:
        unsigned int m_QQId;
        unsigned int m_sessionId;
};

class UHTransferPacket : public UHInPacket
{
    public:
        explicit UHTransferPacket();
        virtual ~UHTransferPacket();
        unsigned int QQId() const { return m_QQId; }
        unsigned int sessionId() const { return m_sessionId; }
        unsigned int avatarSize() const { return m_avatarSize; }
        unsigned int position() const { return m_position; }
        unsigned int imageDataSize() const { return m_imageDataSize; }
        const unsigned char* imageData() const { return m_imageData; }
    protected:
        virtual void extractBody( const unsigned char* buf, int* pos, int len );
    private:
        unsigned int m_QQId;
        unsigned int m_sessionId;
        unsigned int m_avatarSize;
        unsigned int m_position;
        unsigned int m_imageDataSize;
        unsigned char* m_imageData;
};

#endif // EVAUHTRANSFER_H
