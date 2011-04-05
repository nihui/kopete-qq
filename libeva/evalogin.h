#ifndef EVALOGIN_H
#define EVALOGIN_H

#include "evapacket.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

class TouchPacket : public OutPacket
{
    public:
        explicit TouchPacket();
        virtual ~TouchPacket();
        void setRedirect( bool redirect ) { m_redirect = redirect; }
        void setNumberRedirected( unsigned char n ) { m_numberRedirected = n; }
        void setServerFlag( unsigned int f ) { m_serverFlag = f; }
        void setServerVersion( unsigned int v ) { m_serverVersion = v; }
    protected:
        virtual void fillBody( unsigned char* buf, int* pos );
    private:
        bool m_redirect;
        unsigned char m_numberRedirected;
        unsigned int m_serverFlag;
        unsigned int m_serverVersion;
};

class TouchReplyPacket : public InPacket
{
    public:
        explicit TouchReplyPacket();
        virtual ~TouchReplyPacket();
        bool redirect() const { return m_redirect; }
        void setRedirect( bool redirect ) { m_redirect = redirect; }
        unsigned int numberRedirected() const { return m_numberRedirected; }
        unsigned int serverFlag() const { return m_serverFlag; }
        unsigned int serverVersion() const { return m_serverVersion; }
        unsigned int redirectIp() const { return m_redirectIp; }
    protected:
        virtual void extractBody( const unsigned char* buf, int* pos, int len );
    private:
        time_t m_time;
//         unsigned int m_localIp;
        bool m_redirect;
        unsigned char m_numberRedirected;
        unsigned int m_serverFlag;
        unsigned int m_serverVersion;
        unsigned int m_redirectIp;
};

class CaptchaPacket : public OutPacket
{
    public:
        explicit CaptchaPacket();
        virtual ~CaptchaPacket();
    protected:
        virtual void fillBody( unsigned char* buf, int* pos );
};

class CaptchaReplyPacket : public InPacket
{
    public:
        explicit CaptchaReplyPacket();
        virtual ~CaptchaReplyPacket();
        unsigned char checkResultFlag() const { return m_checkResultFlag; }
    protected:
        virtual void extractBody( const unsigned char* buf, int* pos, int len );
    private:
        unsigned char m_checkResultFlag;
};

class AuthenticatePacket : public OutPacket
{
    public:
        explicit AuthenticatePacket();
        virtual ~AuthenticatePacket();
        void setPassword( const unsigned char* password, int len ) {
            free( m_password );
            m_password = (unsigned char*)malloc( len );
            memcpy( m_password, password, len );
            m_passwordLen = len;
        }
    protected:
        virtual void fillBody( unsigned char* buf, int* pos );
    private:
        unsigned char* m_password;
        int m_passwordLen;
};

class AuthenticateReplyPacket : public InPacket
{
    public:
        explicit AuthenticateReplyPacket();
        virtual ~AuthenticateReplyPacket();
        unsigned char authFlag() const { return m_authFlag; }
        unsigned char* reason() const { return m_reason; }
    protected:
        virtual void extractBody( const unsigned char* buf, int* pos, int len );
    private:
        unsigned char m_authFlag;
        unsigned char* m_reason;
};

class VerifyE5Packet : public OutPacket
{
    public:
        explicit VerifyE5Packet();
        virtual ~VerifyE5Packet();
    protected:
        virtual void fillBody( unsigned char* buf, int* pos );
};

class VerifyE5ReplyPacket : public InPacket
{
    public:
        explicit VerifyE5ReplyPacket();
        virtual ~VerifyE5ReplyPacket();
    protected:
        virtual void extractBody( const unsigned char* buf, int* pos, int len );
};

class VerifyE3Packet : public OutPacket
{
    public:
        explicit VerifyE3Packet();
        virtual ~VerifyE3Packet();
    protected:
        virtual void fillBody( unsigned char* buf, int* pos );
};

class VerifyE3ReplyPacket : public InPacket
{
    public:
        explicit VerifyE3ReplyPacket();
        virtual ~VerifyE3ReplyPacket();
    protected:
        virtual void extractBody( const unsigned char* buf, int* pos, int len );
};

class LoginPacket : public OutPacket
{
    public:
        explicit LoginPacket();
        virtual ~LoginPacket();
    protected:
        virtual void fillBody( unsigned char* buf, int* pos );
};

class LoginReplyPacket : public InPacket
{
    public:
        explicit LoginReplyPacket();
        virtual ~LoginReplyPacket();
    protected:
        virtual void extractBody( const unsigned char* buf, int* pos, int len );
};

class LoginE9Packet : public SessionOutPacket
{
    public:
        explicit LoginE9Packet();
        virtual ~LoginE9Packet();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

class LoginE9ReplyPacket : public SessionInPacket
{
    public:
        explicit LoginE9ReplyPacket();
        virtual ~LoginE9ReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
};

class LoginEAPacket : public SessionOutPacket
{
    public:
        explicit LoginEAPacket();
        virtual ~LoginEAPacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

class LoginEAReplyPacket : public SessionInPacket
{
    public:
        explicit LoginEAReplyPacket();
        virtual ~LoginEAReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
};

class GetListPacket : public SessionOutPacket
{
    public:
        explicit GetListPacket();
        virtual ~GetListPacket();
        void setIndex( unsigned short index ) { m_index = index; }
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
    private:
        unsigned short m_index;
};

class GetListReplyPacket : public SessionInPacket
{
    public:
        typedef struct {
            unsigned int QQId;
            unsigned char type;
            unsigned char groupId;
        } BQEntry;
        explicit GetListReplyPacket();
        virtual ~GetListReplyPacket();
        unsigned short indexCount() const { return m_indexCount; }
        unsigned short index() const { return m_index; }
        const BQEntry* entries() const { return m_entries; }
        unsigned short entryCount() const { return m_entryCount; }
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
    private:
        unsigned short m_indexCount;
        unsigned short m_index;
        BQEntry* m_entries;
        unsigned short m_entryCount;
};

class LoginECPacket : public SessionOutPacket
{
    public:
        explicit LoginECPacket();
        virtual ~LoginECPacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

class LoginECReplyPacket : public SessionInPacket
{
    public:
        explicit LoginECReplyPacket();
        virtual ~LoginECReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
};

class LoginEDPacket : public SessionOutPacket
{
    public:
        explicit LoginEDPacket();
        virtual ~LoginEDPacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

class LoginEDReplyPacket : public SessionInPacket
{
    public:
        explicit LoginEDReplyPacket();
        virtual ~LoginEDReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
};

#endif // EVALOGIN_H
