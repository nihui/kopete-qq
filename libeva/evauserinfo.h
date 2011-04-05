#ifndef EVAUSERINFO_H
#define EVAUSERINFO_H

#include "evapacket.h"

class GetUserInfoPacket : public SessionOutPacket
{
    public:
        explicit GetUserInfoPacket();
        virtual ~GetUserInfoPacket();
        void setQQId( unsigned int qq ) { m_QQId = qq; }
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
    private:
        unsigned int m_QQId;
};

class GetUserInfoReplyPacket : public SessionInPacket
{
    public:
        explicit GetUserInfoReplyPacket();
        virtual ~GetUserInfoReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
    private:
        unsigned int m_QQId;
        char* m_nickname;
        char* m_name;
        char* m_email;
        char* m_occupation;
        char* m_homepage;
        char* m_mobile;
        char* m_school;
        char* m_introduction;
};

#endif // EVAUSERINFO_H
