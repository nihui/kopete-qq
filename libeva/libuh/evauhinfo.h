#ifndef EVAUHINFO_H
#define EVAUHINFO_H

#include "evauhpacket.h"

class UHInfoPacket : public UHOutPacket
{
    public:
        explicit UHInfoPacket();
        virtual ~UHInfoPacket();
        bool addQQId( unsigned int qq ) {
            if ( m_count >= 20 )
                return false;
            m_QQIds[m_count++] = qq;
            return true;
        }
    protected:
        virtual void fillBody( unsigned char* buf, int* pos );
    private:
        unsigned int m_QQIds[20];
        unsigned short m_count;
};

class UHInfoReplyPacket : public UHInPacket
{
    public:
        typedef struct {
            unsigned int QQId;
            unsigned int sessionId;
            unsigned char avatarMd5[16];
        } UHInfo;
        explicit UHInfoReplyPacket();
        virtual ~UHInfoReplyPacket();
        unsigned short uhInfoCount() const { return m_uhInfoCount; }
        const UHInfo* uhInfoAt( int i ) const { return m_uhInfos + i; }
    protected:
        virtual void extractBody( const unsigned char* buf, int* pos, int len );
    private:
        UHInfo* m_uhInfos;
        unsigned short m_uhInfoCount;
};

#endif // EVAUHINFO_H
