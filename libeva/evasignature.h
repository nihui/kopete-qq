#ifndef EVASIGNATURE_H
#define EVASIGNATURE_H

#include "evapacket.h"

class SignaturePacket : public SessionOutPacket
{
    public:
        explicit SignaturePacket();
        virtual ~SignaturePacket();
        bool addQQId( unsigned int qq ) {
            if ( m_count >= 10 )
                return false;
            m_QQIds[ m_count++ ] = qq;
            return true;
        }
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
    private:
        unsigned int m_QQIds[10];
        unsigned short m_count;
};

class SignatureReplyPacket : public SessionInPacket
{
    public:
        typedef struct {
            unsigned int QQId;
            unsigned char* signature;
        } SignEntry;
        explicit SignatureReplyPacket();
        virtual ~SignatureReplyPacket();
        const SignEntry* signEntries() const { return m_signEntries; }
        int signEntryCount() const { return m_signEntryCount; }
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
    private:
        SignEntry* m_signEntries;
        int m_signEntryCount;
};

#endif // EVASIGNATURE_H
