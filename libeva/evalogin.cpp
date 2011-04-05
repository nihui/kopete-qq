#include "evalogin.h"

#include "evacrypt.h"
#include "md5.h"

#include <QDebug>
#include <QtGlobal>

#include <time.h>

#ifdef _WIN32
#include <winsock.h>
#else // _WIN32
#include <arpa/inet.h>
#endif // _WIN32

static const unsigned char data1[6] = { 0x00, 0x00, 0x08, 0x04, 0x01, 0xe0 };
static const unsigned char data2[12] = { 0x00, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x0c, 0x73 };

static unsigned char data3[4];

unsigned int localIp;

static time_t authTime;

static unsigned char key1[16];
static unsigned char key2[16];
static unsigned char key3[16];
static unsigned char key4[16];
static unsigned char key5[16];
static unsigned char key6[16];
static unsigned char key7[16];
static unsigned char key8[16];

static unsigned short token1Size = 0;
static unsigned char* token1 = 0;
static unsigned short token2Size = 0;
static unsigned char* token2 = 0;
static unsigned short token3Size = 0;
static unsigned char* token3 = 0;
static unsigned short token4Size = 0;
static unsigned char* token4 = 0;
static unsigned short token5Size = 0;
static unsigned char* token5 = 0;
static unsigned short token6Size = 0;
static unsigned char* token6 = 0;
static unsigned short token7Size = 0;
static unsigned char* token7 = 0;
static unsigned short token8Size = 0;
// static unsigned char* token8 = 0;
static unsigned short token9Size = 0;
// static unsigned char* token9 = 0;

/// session key
extern unsigned char sessionKey[16];
/// session md5
extern unsigned char sessionMd5[16];


TouchPacket::TouchPacket() : OutPacket()
{
    m_command = 0x0091;

    m_redirect = false;
    m_numberRedirected = 0x00;
    m_serverFlag = 0;
    m_serverVersion = 0;
}

TouchPacket::~TouchPacket()
{
}

void TouchPacket::fillBody( unsigned char* buf, int* pos )
{
    /// random TEA key1
    qsrand( time( NULL ) );
    for ( int i = 0; i < 16; ++i )
        key1[i] = qrand();
    fillN( buf, pos, key1, 16 );

    /// 48 bytes TEA encrypted data
    int tpos = 0;
    unsigned char ttt[35];

    /// fixed 0x00 0x01
    unsigned char fixed[2] = { 0x00, 0x01 };
    fillN( ttt, &tpos, fixed, 2 );

    /// data1
    fillN( ttt, &tpos, data1, 6 );

    /// data2
    fillN( ttt, &tpos, data2, 12 );

    if ( m_redirect ) {
        /// start with 0x00
        zeroN( ttt, &tpos, 1 );

        /// flag redirection needed
        fill1( ttt, &tpos, 0x01 );

        /// number redirected
        fill1( ttt, &tpos, m_numberRedirected );

        /// server flag
        fill4( ttt, &tpos, m_serverFlag );

        /// server version
        fill4( ttt, &tpos, m_serverVersion );

        /// orignal redirection ip
        unsigned char ff4[4] = { 0xff, 0xff, 0xff, 0xff };
        fillN( ttt, &tpos, ff4, 4 );
    }
    else {
        zeroN( ttt, &tpos, 15 );
    }

    unsigned char out[48];
    int outlen = 48;
    EvaCrypt::encrypt( ttt, 35, key1, out, &outlen );
    fillN( buf, pos, out, outlen );
}

TouchReplyPacket::TouchReplyPacket() : InPacket()
{
    m_command = 0x0091;

    m_time = 0;
    m_redirect = false;
    m_numberRedirected = 0x00;
    m_serverFlag = 0;
    m_serverVersion = 0;
    m_redirectIp = 0;
}

TouchReplyPacket::~TouchReplyPacket()
{
}

void TouchReplyPacket::extractBody( const unsigned char* buf, int* pos, int len )
{
    m_redirect = ( len == 88 );

    /// 104/88 bytes TEA encrypted data
    unsigned char code[104];
    extractN( buf, pos, code, m_redirect ? 88 : 104 );

    unsigned char out[89];
    int outlen = m_redirect ? 76 : 89;
    EvaCrypt::decrypt( code, m_redirect ? 88 : 104, key1, out, &outlen );

    /// extract decrypted body
    unsigned char b1;
//     unsigned short b2;
    unsigned int b4;
    int pout = 0;

    /// flag touch success
    extract1( out, &pout, &b1 );

    /// time
    extract4( out, &pout, (unsigned int*)&m_time );

    qWarning() << "time: " << ctime( &m_time );

    /// ip
    extractN( out, &pout, &localIp, 4 );

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &localIp, ipstr, INET_ADDRSTRLEN );
    qWarning() << "local ip: " << ipstr;

    /// 8 bytes zero filling
    skipN( out, &pout, 8 );

    /// size of token touch
    extract2( out, &pout, &token1Size );

    /// 56 bytes token touch
    token1 = (unsigned char*)malloc( token1Size );
    extractN( out, &pout, token1, token1Size );

    /// flag redirection needed
    extract1( out, &pout, &b1 );

    if ( m_redirect )
        return;

    /// number redirected
    extract1( out, &pout, &b1 );

    /// server flag
    extract4( out, &pout, &b4 );

    /// server version
    extract4( out, &pout, &b4 );

    /// redirection ip
    extractN( out, &pout, &m_redirectIp, 4 );

    char redirect_ipstr[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &m_redirectIp, redirect_ipstr, INET_ADDRSTRLEN );
    qWarning() << "redirect server ip: " << redirect_ipstr;
}

CaptchaPacket::CaptchaPacket() : OutPacket()
{
    m_command = 0x00ba;
}

CaptchaPacket::~CaptchaPacket()
{
}

void CaptchaPacket::fillBody( unsigned char* buf, int* pos )
{
    /// random TEA key
    qsrand( time( NULL ) );
    for ( int i = 0; i < 16; ++i )
        key2[i] = qrand();
    fillN( buf, pos, key2, 16 );

    /// 104 bytes TEA encrypted data
    unsigned char ttt[88];
    int tpos = 0;

    /// fixed 0x00 0x01
    fill2( ttt, &tpos, 0x0001 );

    /// data1
    fillN( ttt, &tpos, data1, 6 );

    /// data2
    fillN( ttt, &tpos, data2, 12 );

    /// tokenTouch size
    fill2( ttt, &tpos, token1Size );

    /// tokenTouch
    fillN( ttt, &tpos, token1, token1Size );

    /// fixed filling
    unsigned char filling[10] = {
        0x03, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    fillN( ttt, &tpos, filling, 10 );

    unsigned char out[104];
    int outlen = 0;
    EvaCrypt::encrypt( ttt, 88, key2, out, &outlen );
    fillN( buf, pos, out, outlen );
}

CaptchaReplyPacket::CaptchaReplyPacket() : InPacket()
{
    m_command = 0x00ba;
}

CaptchaReplyPacket::~CaptchaReplyPacket()
{
}

void CaptchaReplyPacket::extractBody( const unsigned char* buf, int* pos, int len )
{
    /// len bytes TEA encrypted data
    unsigned char* code = (unsigned char*)malloc(len);
    extractN( buf, pos, code, len );

    /// FIXME: if captcha reply overflow, fix here  --- nihui
    unsigned char out[66];
    int outlen = len;
    EvaCrypt::decrypt( code, len, key2, out, &outlen );

    free(code);

    unsigned char b1;
//     unsigned short b2;
    unsigned int b4;
    int pout = 0;

    /// check result flag, 03 successful, 04 captcha needed
    extract1( out, &pout, &m_checkResultFlag );

    /// fixed 0x00 0x05
    skipN( out, &pout, 2 );

    /// flag if captcha needed
    extract1( out, &pout, &b1 );

    /// all 0x00 if captcha not needed, 0x00 0x00 0x01 0x23 if needed
    extract4( out, &pout, &b4 );

    /// size of token captcha
    extract2( out, &pout, &token2Size );

    /// token captcha
    token2 = (unsigned char*)malloc( token2Size );
    extractN( out, &pout, token2, token2Size );
}

AuthenticatePacket::AuthenticatePacket() : OutPacket()
{
    m_command = 0x00dd;

    m_password = NULL;
    m_passwordLen = 0;
}

AuthenticatePacket::~AuthenticatePacket()
{
    free( m_password );
}

void AuthenticatePacket::fillBody( unsigned char* buf, int* pos )
{
    /// random TEA key
    qsrand( time( NULL ) );
    for ( int i = 0; i < 16; ++i )
        key3[i] = qrand();
    fillN( buf, pos, key3, 16 );

    /// 601 bytes
    unsigned char ttt[601];
    int tpos = 0;

    /// fixed filling
    unsigned char filling[4] = {
        0x00, 0xde, 0x00, 0x01
    };
    fillN( ttt, &tpos, filling, 4 );

    /// data1
    fillN( ttt, &tpos, data1, 6 );

    /// data2
    fillN( ttt, &tpos, data2, 12 );

    /// tokenCaptchaSize
    fill2( ttt, &tpos, token2Size );

    /// tokenCaptcha
    fillN( ttt, &tpos, token2, token2Size );

    /// TEA encrypted data size, 120bytes, key is md5(md5(password))
    fill2( ttt, &tpos, 0x0078 );

    /// authentication data
    unsigned char authdata[104];
    int apos = 0;

    /// get TEA key
    md5_state_t state;

    unsigned char digest1[16];
    md5_init( &state );
    md5_append( &state, m_password, m_passwordLen );
    md5_finish( &state, digest1 );

    unsigned char digest2[16];
    md5_init( &state );
    md5_append( &state, digest1, 16 );
    md5_finish( &state, digest2 );

    /// unknown random
    fill4( authdata, &apos, rand() );

    /// fixed 0x00 0x01
    fill2( authdata, &apos, 0x0001 );

    /// qq number
    fill4( authdata, &apos, EvaPacket::QQNumber() );

    /// data2
    fillN( authdata, &apos, data2, 12 );

    /// fixed 0x00 0x00 0x00
    zeroN( authdata, &apos, 3 );

    /// qq md5(password)
    fillN( authdata, &apos, digest1, 16 );

    /// last login time, just set now
    fill4( authdata, &apos, time( NULL ) );

    /// 13 bytes fixed 0x00
    zeroN( authdata, &apos, 13 );

    /// ip
    fillN( authdata, &apos, &localIp, 4 );

    /// 8 bytes fixed 0x00
    zeroN( authdata, &apos, 8 );

    /// size of key below
    fill2( authdata, &apos, 0x0010 );

    /// auth key 1
    const unsigned char authKey1[16] = {
        0x54, 0xae, 0xc9, 0x0e, 0x28, 0x31, 0x3c, 0x41,
        0x94, 0x9c, 0xa9, 0xa4, 0x7b, 0xfa, 0x9e, 0xe3
    };
    fillN( authdata, &apos, authKey1, 16 );

    /// auth key 2
    qsrand( time( NULL ) );
    for ( int i = 0; i < 16; ++i )
        key4[i] = qrand();
    fillN( authdata, &apos, key4, 16 );

    unsigned char aout[120];
    int aoutlen = 120;
    EvaCrypt::encrypt( authdata, 104, digest2, aout, &aoutlen );
    fillN( ttt, &tpos, aout, aoutlen );

    /// size of data below
    fill2( ttt, &tpos, 0x0014 );

    /// FIXME: unknown random key + unknown random 4 bytes ?
    unsigned char rk[16];
    for ( int i = 0; i < 16; ++i )
        rk[i] = qrand();
    fillN( ttt, &tpos, rk, 16 );
    unsigned char rk4[4];
    for ( int i = 0; i < 4; ++i )
        rk4[i] = qrand();
    fillN( ttt, &tpos, rk4, 4 );

    /// version depended data
    fill4( ttt, &tpos, 0x01772e01 );
    fill4( ttt, &tpos, 0x3327b33b );

    /// size of auth key 2
    fill2( ttt, &tpos, 0x0010 );

    /// auth key 2
    const unsigned char authKey2[16] = {
        0x03, 0x5d, 0xc5, 0xe7, 0xbd, 0xbd, 0x42, 0xff,
        0x56, 0xed, 0x4c, 0xa4, 0x8a, 0x27, 0x5f, 0x2c
    };
    fillN( ttt, &tpos, authKey2, 16 );

    /// fixed data 0x02 0x04 0xbb 0x96 0xb5
    unsigned char bkfilling[5] = { 0x02, 0x24, 0xbb, 0x96, 0xb5 };
    fillN( ttt, &tpos, bkfilling, 5 );

    /// size of auth key 3
    fill2( ttt, &tpos, 0x0010 );

    /// auth key 3
    const unsigned char authKey3[16] = {
        0x26, 0x66, 0x3f, 0xf3, 0xf1, 0x6d, 0x96, 0x0f,
        0x4d, 0x94, 0x85, 0xe0, 0x3f, 0xe4, 0x15, 0xb4
    };
    fillN( ttt, &tpos, authKey3, 16 );

    /// 328 bytes zero filling
    zeroN( ttt, &tpos, 328 );

    unsigned char out[616];
    int outlen = 616;
    EvaCrypt::encrypt( ttt, 601, key3, out, &outlen );
    fillN( buf, pos, out, outlen );
}

AuthenticateReplyPacket::AuthenticateReplyPacket() : InPacket()
{
    m_command = 0x00dd;

    m_authFlag = 0x00;
    m_reason = 0;
}

AuthenticateReplyPacket::~AuthenticateReplyPacket()
{
    free( m_reason );
}

void AuthenticateReplyPacket::extractBody( const unsigned char* buf, int* pos, int len )
{
    qWarning() << "AuthenticateReplyPacket::extractBody" << len;
    unsigned char* code = (unsigned char*)malloc( len );
    extractN( buf, pos, code, len );
    unsigned char out[285];
    int outlen = 285;

    if ( len != 296 )
        EvaCrypt::decrypt( code, len, key3, out, &outlen );
    else
        EvaCrypt::decrypt( code, len, key4, out, &outlen );

    qWarning() << "out" << outlen << QByteArray( (char*)out, outlen ).toHex();

    free( code );

    int pout = 0;

//     unsigned char b1;
    unsigned short b2;
//     unsigned int b4;
    /// size of value - size(2 bytes) - tail(2 bytes)
    extract2( out, &pout, &b2 );

    /// authentication flag
    /// 0x00 successful
    /// 0x33 or 0x51 rejected
    /// 0xbf qq number not exists
    /// 0x34 password incorrect
    extract1( out, &pout, &m_authFlag );

    switch ( m_authFlag ) {
        case 0x00: {
            /// successful
            /// size of token3
            extract2( out, &pout, &token3Size );

            /// tokenAuth1
            token3 = (unsigned char*)malloc( token3Size );
            extractN( out, &pout, token3, token3Size );

            /// size of time + tokenAuth2
            extract2( out, &pout, &b2 );

            /// fixed filling 0x00 0x02
            skipN( out, &pout, 2 );

            /// time
            extract4( out, &pout, (unsigned int*)&authTime );

            qWarning() << "time: " << ctime( &authTime );

            /// size of tokenAuth2
            extract2( out, &pout, &token4Size );

            /// tokenAuth2
            token4 = (unsigned char*)malloc( token4Size );
            extractN( out, &pout, token4, token4Size );

            /// size of tokenAuth3
            extract2( out, &pout, &token5Size );

            /// tokenAuth3
            token5 = (unsigned char*)malloc( token5Size );
            extractN( out, &pout, token5, token5Size );

            /// verifyE5 auth key1
            extractN( out, &pout, key5, 16 );

            /// fixed filling 0x00 0x00
            skipN( out, &pout, 2 );

            /// verifyE5 auth key2
            extractN( out, &pout, key6, 16 );

            /// fixed tail filling 0x00 0x00
            skipN( out, &pout, 2 );
            break;
        }
        case 0x33:
        case 0x34: {
            /// password incorrect
            /// fixed filling 0x00 0x00 0x01 0x02 0x00 0x00 0x00 0x00
            skipN( out, &pout, 8 );

            /// length of message below
            unsigned short msglen = 0;
            extract2( out, &pout, &msglen );

            /// utf-8 message
            m_reason = (unsigned char*)malloc( msglen + 1 );
            extractN( out, &pout, m_reason, msglen );
            m_reason[ msglen ] = '\0';

            qWarning() << "MSG" << QString::fromUtf8( (char*)m_reason );

            /// fixed tail filling 0x00 0x00
            skipN( out, &pout, 2 );
            break;
        }
    }
}

VerifyE5Packet::VerifyE5Packet() : OutPacket()
{
    m_command = 0x00e5;
}

VerifyE5Packet::~VerifyE5Packet()
{
}

void VerifyE5Packet::fillBody( unsigned char* buf, int* pos )
{
    /// size of tokenAuth3
    fill2( buf, pos, token5Size );

    /// tokenAuth3
    fillN( buf, pos, token5, token5Size );

    /// TEA encrypted data
    unsigned char ttt[276];
    int tpos = 0;

    /// fixed filling 0x01 0x0e 0x00 0x01 0x01
    unsigned char filling[5] = { 0x01, 0x0e, 0x00, 0x01, 0x01 };
    fillN( ttt, &tpos, filling, 5 );

    /// data1
    fillN( ttt, &tpos, data1, 6 );

    /// data2
    fillN( ttt, &tpos, data2, 12 );

    /// tokenCaptchaSize
    fill2( ttt, &tpos, token2Size );

    /// tokenCaptcha
    fillN( ttt, &tpos, token2, token2Size );

    /// size of tokenAuth1
    fill2( ttt, &tpos, token3Size );

    /// tokenAuth1
    fillN( ttt, &tpos, token3, token3Size );

    /// size of time + tokenAuth2
    fill2( ttt, &tpos, 0x0098 );

    /// fixed 0x00 0x02
    fill2( ttt, &tpos, 0x0002 );

    /// time
    fill4( ttt, &tpos, (unsigned int)authTime );

    /// size of tokenAuth2
    fill2( ttt, &tpos, token4Size );

    /// tokenAuth2
    fillN( ttt, &tpos, token4, token4Size );

    /// fixed filling 0x00 0x00 0x01 0x00 0x00 0x00 0x00
    unsigned char filling2[7] = { 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 };
    fillN( ttt, &tpos, filling2, 7 );

    unsigned char out[288];
    int outlen = 288;
    EvaCrypt::encrypt( ttt, 276, key5, out, &outlen );
    fillN( buf, pos, out, outlen );
}

VerifyE5ReplyPacket::VerifyE5ReplyPacket() : InPacket()
{
    m_command = 0x00e5;
}

VerifyE5ReplyPacket::~VerifyE5ReplyPacket()
{
}

void VerifyE5ReplyPacket::extractBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char* code = (unsigned char*)malloc( len );
    extractN( buf, pos, code, len );

    unsigned char out[529];
    int outlen = 529;
    EvaCrypt::decrypt( code, len, key6, out, &outlen );
    qWarning() << "outlen" << outlen;

    free( code );

    int pout = 0;

//     unsigned char b1;
    unsigned short b2;
    unsigned int b4;

    /// 2 bytes, qq number depended
    extract2( out, &pout, &b2 );

    /// fixed 0x01 0x00
    skipN( out, &pout, 2 );

    /// verifyE5Key1
    extractN( out, &pout, key7, 16 );

    /// 8 bytes fixed filling
    skipN( out, &pout, 8 );

    /// 4 bytes unknown, used in login packet
    extractN( out, &pout, data3, 4 );

    /// time
    time_t time;
    extract4( out, &pout, (unsigned int*)&time );

    qWarning() << "time: " << ctime( &time );

    /// ip
    unsigned int ip;
    extractN( out, &pout, &ip, 4 );

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &ip, ipstr, INET_ADDRSTRLEN );
    qWarning() << "local ip: " << ipstr;

    /// 8 bytes fixed 0x00
    skipN( out, &pout, 8 );

    /// size of tokenVerifyE51
    extract2( out, &pout, &token6Size );

    /// tokenVerifyE51
    token6 = (unsigned char*)malloc( token6Size );
    extractN( out, &pout, token6, token6Size );

    /// verifyE5Key2
    extractN( out, &pout, key8, 16 );

    /// size of time + tokenVerifyE52
    extract2( out, &pout, &b2 );

    /// fixed 0x00 0x01
    skipN( out, &pout, 2 );

    /// time
    extract4( out, &pout, &b4 );

    /// size of tokenVerifyE52
    extract2( out, &pout, &token7Size );

    /// tokenVerifyE52
    token7 = (unsigned char*)malloc( token7Size );
    extractN( out, &pout, token7, token7Size );

    /// 4 bytes filling, version depended
    skipN( out, &pout, 4 );

    /// fixed 0x00 0x01
    skipN( out, &pout, 2 );

    /// fixed 0x51 0x80 0x00 0x00 0x00 0x3c 0x00 0x02
    skipN( out, &pout, 8 );

    /// size of time + tokenVerifyE53
    extract2( out, &pout, &b2 );

    /// fixed 0x01
    skipN( out, &pout, 1 );

    /// 16 bytes, unknown key
    skipN( out, &pout, 16 );

    /// fixed 0x00 0x60 0x00 0x01
    skipN( out, &pout, 4 );

    /// time
    time_t time2;
    extract4( out, &pout, (unsigned int*)&time2 );

    qWarning() << "time: " << ctime( &time2 );

    /// size of tokenVerifyE53
    extract2( out, &pout, &token8Size );

    /// tokenVerifyE53
    skipN( out, &pout, token8Size );
//     extractN( out, &pout, token8, token8Size );

    /// fixed 0x00 0x00
    skipN( out, &pout, 2 );
}

VerifyE3Packet::VerifyE3Packet() : OutPacket()
{
    m_command = 0x00e3;
}

VerifyE3Packet::~VerifyE3Packet()
{
}

void VerifyE3Packet::fillBody( unsigned char* buf, int* pos )
{
    /// size of tokenAuth3
    fill2( buf, pos, token5Size );

    /// tokenAuth3
    fillN( buf, pos, token5, token5Size );

    /// TEA encrypted data
    unsigned char ttt[238];
    int tpos = 0;

    /// fixed 0x00 0xc8 0x00 0x01
    fill4( ttt, &tpos, 0x00c80001 );

    /// data1
    fillN( ttt, &tpos, data1, 6 );

    /// data2
    fillN( ttt, &tpos, data2, 12 );

    /// tokenCaptchaSize
    fill2( ttt, &tpos, token2Size );

    /// tokenCaptcha
    fillN( ttt, &tpos, token2, token2Size );

    /// size of time + tokenVerifyE52
    fill2( ttt, &tpos, 0x0078 );

    /// fixed 0x00 0x01
    fill2( ttt, &tpos, 0x0001 );

    /// time
    fill4( ttt, &tpos, authTime );

    /// size of tokenVerifyE52
    fill2( ttt, &tpos, token7Size );

    /// tokenVerifyE52
    fillN( ttt, &tpos, token7, token7Size );

    /// 2 bytes fixed filling 0x00 0x00
    zeroN( ttt, &tpos, 2 );

    /// size of zero filling
    fill2( ttt, &tpos, 0x0020 );

    /// 32 bytes zero filling
    zeroN( ttt, &tpos, 32 );

    unsigned char out[248];
    int outlen = 248;
    EvaCrypt::encrypt( ttt, 238, key5, out, &outlen );
    qWarning() << "outlen" << outlen;
    fillN( buf, pos, out, outlen );
}

VerifyE3ReplyPacket::VerifyE3ReplyPacket() : InPacket()
{
    m_command = 0x00e3;
}

VerifyE3ReplyPacket::~VerifyE3ReplyPacket()
{
}

void VerifyE3ReplyPacket::extractBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char* code = (unsigned char*)malloc( len );
    extractN( buf, pos, code, len );
    unsigned char out[60];
    int outlen = 60;
    EvaCrypt::decrypt( code, len, key8, out, &outlen );

    free( code );

    qWarning() << "VerifyE3ReplyPacket::extractBody" << QByteArray( (char*)out, outlen ).toHex();

    unsigned char b1;

    int pout = 0;

    /// 7 bytes qq number depended
    skipN( out, &pout, 7 );

    /// size of qq username below
    unsigned char usernamelen;
    extract1( out, &pout, &usernamelen );

    /// qq username
    unsigned char* username = (unsigned char*)malloc( usernamelen + 1 );
    extractN( out, &pout, username, usernamelen );
    username[ usernamelen ] = '\0';

    qWarning() << "username" << QString::fromUtf8( (char*)username );

    free( username );

    /// result
    extract1( out, &pout, &b1 );

    /// 12 bytes unknown fixed filling
    /// 0x03 0x80 0x02 0x00 0x00 0x04 0x00 0x00 0x00 0x00 0x00 0x00
    skipN( out, &pout, 12 );
}

LoginPacket::LoginPacket() : OutPacket()
{
    m_command = 0x0030;
}

LoginPacket::~LoginPacket()
{
}

void LoginPacket::fillBody( unsigned char* buf, int* pos )
{
    /// size of tokenAuth3
    fill2( buf, pos, token5Size );

    /// tokenAuth3
    fillN( buf, pos, token5, token5Size );

    /// TEA encrypted data
    unsigned char ttt[736];
    int tpos = 0;

    /// fixed 0x00 0x01
    fill2( ttt, &tpos, 0x0001 );

    /// data2
    fillN( ttt, &tpos, data2, 12 );

    /// unknown data in VerifyE5ReplyPacket
    fillN( ttt, &tpos, data3, 4 );

    /// login time in AuthenticateReplyPacket
    fill4( ttt, &tpos, (unsigned int)authTime );

    /// ip in TouchReplyPacket
    fillN( ttt, &tpos, &localIp, 4 );

    /// 8 bytes zero fixed filling
    zeroN( ttt, &tpos, 8 );

    /// size of tokenVerifyE51
    fill2( ttt, &tpos, token6Size );

    /// tokenVerifyE51
    fillN( ttt, &tpos, token6, token6Size );

    /// 35 bytes zero filling
    zeroN( ttt, &tpos, 35 );

    /// 16 bytes version depended
    const unsigned char filling[16] = {
        0xff, 0xe8, 0x46, 0x0b, 0xd7, 0x0e, 0x55, 0x54, 0x46, 0x4d, 0x91, 0x5c, 0x87, 0xb5, 0xf6, 0x7f
    };
    fillN( ttt, &tpos, filling, 16 );

    /// unknown random 0xf5, 0xcc is ok
    fill1( ttt, &tpos, 0xcc );

    /// login mode, FIXME--->hardcode<--- 0x0a normal, 0x28 invisible
    fill1( ttt, &tpos, 0x28 );

    /// 25 bytes zero filling
    zeroN( ttt, &tpos, 25 );

    /// data1
    fillN( ttt, &tpos, data1, 6 );

    /// 16 bytes zero filling
    zeroN( ttt, &tpos, 16 );

    /// tokenCaptchaSize
    fill2( ttt, &tpos, token2Size );

    /// tokenCaptcha
    fillN( ttt, &tpos, token2, token2Size );

    /// unknown bytes fixed, version depended
    const unsigned char filling2[37] = {
        0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0x10, 0x00, 0x01, 0x40, 0x01, 0x67,
        0xf7, 0xd6, 0xe6, 0x00, 0x10, 0xaf, 0x2b, 0x1d, 0x01, 0x70, 0x06, 0xce, 0xa3, 0xcc, 0xe7, 0xaf,
        0x61, 0x7f, 0xb4, 0x17, 0xef
    };
    fillN( ttt, &tpos, filling2, 37 );

    /// 00 00 00 00 00 01 00 00 00
    const unsigned char fz[9] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 };
    fillN( ttt, &tpos, fz, 9 );

    /// 16 bytes zero filling
    zeroN( ttt, &tpos, 16 );

    /// unknown bytes fixed, version depended
    const unsigned char filling3[5] = { 0x02, 0x24, 0xbb, 0x96, 0xb5 };
    fillN( ttt, &tpos, filling3, 5 );

    /// size of data below
    fill2( ttt, &tpos, 0x0010 );

    /// unknown bytes fixed, version depended
    const unsigned char filling4[16] = {
        0x53, 0x7d, 0x09, 0x14, 0xe1, 0x32, 0x81, 0x48, 0xd8, 0x8a, 0xc9, 0x89, 0xa6, 0x5e, 0xaa, 0x43
    };
    fillN( ttt, &tpos, filling4, 16 );

    /// 249 bytes zero filling
    zeroN( ttt, &tpos, 249 );

    unsigned char out[752];
    int outlen = 752;
    EvaCrypt::encrypt( ttt, 736, key5, out, &outlen );
    fillN( buf, pos, out, outlen );
}

LoginReplyPacket::LoginReplyPacket() : InPacket()
{
    m_command = 0x0030;
}

LoginReplyPacket::~LoginReplyPacket()
{
}

void LoginReplyPacket::extractBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char* code = (unsigned char*)malloc( len );
    extractN( buf, pos, code, len );
    unsigned char out[142];
    int outlen = 142;
    EvaCrypt::decrypt( code, len, key7, out, &outlen );

    qWarning() << "outlen" << outlen << QByteArray( (char*)out, outlen ).toHex();

    free( code );

    int pout = 0;

    unsigned char b1;
    unsigned short b2;
    unsigned int b4;

    /// flag login successful
    extract1( out, &pout, &b1 );
    qWarning() << "login flag" << QByteArray( (char*)&b1, 1 ).toHex();

    /// session key
    extractN( out, &pout, sessionKey, 16 );
    qWarning() << "sessionKey" << QByteArray( (char*)sessionKey, 16 ).toHex();

    /// qq number
    extract4( out, &pout, &b4 );
    qWarning() << "qq" << b4;

    /// generate session md5
    unsigned char raw[20];
    memcpy( raw, &b4, 4 );
    memcpy( raw + 4, sessionKey, 16 );
    md5_state_t state;
    md5_init( &state );
    md5_append( &state, raw, 20 );
    md5_finish( &state, sessionMd5 );

    /// ip
    extractN( out, &pout, &b4, 4 );

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &b4, ipstr, INET_ADDRSTRLEN );
    qWarning() << "ip: " << ipstr;

    /// port
    extract2( out, &pout, &b2 );
    qWarning() << "port" << b2;

    /// login time
    extract4( out, &pout, &b4 );
    qWarning() << "login time" << ctime( (time_t*)&b4 );

    /// login ip, 0x00 0x00 0x00 0x00
    extract4( out, &pout, &b4 );

    char ipstr2[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &b4, ipstr2, INET_ADDRSTRLEN );
    qWarning() << "login ip: " << ipstr2;

    /// 42 bytes fixed
    skipN( out, &pout, 42 );

    /// size of tokenLogin
    extract2( out, &pout, &token9Size );

    /// tokenLogin
    skipN( out, &pout, token9Size );
//     extractN( out, &pout, token9, token9Size );

    /// unknown 8 bytes
    skipN( out, &pout, 8 );

    /// 4 bytes fixed
    skipN( out, &pout, 4 );
}

LoginE9Packet::LoginE9Packet() : SessionOutPacket()
{
    m_command = 0x00e9;
}

LoginE9Packet::~LoginE9Packet()
{
}

void LoginE9Packet::fillSessionBody( unsigned char* buf, int* pos )
{
    /// fixed 0x01 0x01
    fill2( buf, pos, 0x0101 );
}

LoginE9ReplyPacket::LoginE9ReplyPacket() : SessionInPacket()
{
    m_command = 0x00e9;
}

LoginE9ReplyPacket::~LoginE9ReplyPacket()
{
}

void LoginE9ReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    qWarning() << "@@@@@ LoginE9ReplyPacket::extractBody";
    /// fixed 0x01 0x01 0x00
    skipN( buf, pos, 3 );
}

LoginEAPacket::LoginEAPacket() : SessionOutPacket()
{
    m_command = 0x00ea;
}

LoginEAPacket::~LoginEAPacket()
{
}

void LoginEAPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// fixed 0x01
    fill1( buf, pos, 0x01 );
}

LoginEAReplyPacket::LoginEAReplyPacket() : SessionInPacket()
{
    m_command = 0x00ea;
}

LoginEAReplyPacket::~LoginEAReplyPacket()
{
}

void LoginEAReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    qWarning() << "@@@@@ LoginEAReplyPacket::extractBody";
    /// FIXME skip all here
    skipN( buf, pos, len );
}

GetListPacket::GetListPacket() : SessionOutPacket()
{
    m_command = 0x00eb;

    m_index = 0x0001;
}

GetListPacket::~GetListPacket()
{
}

void GetListPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// fixed 0x01 0x00 0x00 0x00
    fill4( buf, pos, 0x01000000 );

    /// qq number depended, could be fixed
    /// FIXME: all zero now  --- nihui
//     const unsigned char filling[5] = { 0x0f, 0x4d, 0x2f, 0x1a, 0xfe };
//     fillN( buf, pos, filling, 5 );
    zeroN( buf, pos, 5 );

    /// index
    fill2( buf, pos, m_index );
}

GetListReplyPacket::GetListReplyPacket() : SessionInPacket()
{
    m_command = 0x00eb;

    m_indexCount = 0x0001;
    m_index = 0x0001;
    m_entries = NULL;
    m_entryCount = 0;
}

GetListReplyPacket::~GetListReplyPacket()
{
    free( m_entries );
}

void GetListReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    qWarning() << "LIST" << QByteArray( (char*)buf, len ).toHex();

    /// fixed 0x01
    skipN( buf, pos, 1 );

    /// refresh flag, 0x00 needed, 0x01 not needed
    unsigned char refreshFlag;
    extract1( buf, pos, &refreshFlag );

    if ( refreshFlag == 0x00 ) {
        /// 12 bytes unknown
        skipN( buf, pos, 12 );

        /// index count
        extract2( buf, pos, &m_indexCount );

        /// index
        extract2( buf, pos, &m_index );

        /// buddy + qun count
        extract2( buf, pos, &m_entryCount );

        m_entries = (BQEntry*)malloc( m_entryCount * sizeof(BQEntry) );

        for ( unsigned short i = 0; i < m_entryCount; ++i ) {
            /// qq number or qun number
            extract4( buf, pos, &(m_entries[i].QQId) );

            /// type, 0x01 -> buddy, 0x04 -> qun
            extract1( buf, pos, &(m_entries[i].type) );

            /// group id, divide by 4
            extract1( buf, pos, &(m_entries[i].groupId) );
            m_entries[i].groupId /= 4;

            qWarning() << "BQ" << '\t' << m_entries[i].QQId << '\t'
                << m_entries[i].type << '\t' << m_entries[i].groupId;
        }
    }
}

LoginECPacket::LoginECPacket() : SessionOutPacket()
{
    m_command = 0x00ec;
}

LoginECPacket::~LoginECPacket()
{
}

void LoginECPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// fixed 0x01 0x00 0x0a
    const unsigned char filling[3] = { 0x01, 0x00, 0x0a };
    fillN( buf, pos, filling, 3 );
}

LoginECReplyPacket::LoginECReplyPacket() : SessionInPacket()
{
    m_command = 0x00ec;
}

LoginECReplyPacket::~LoginECReplyPacket()
{
}

void LoginECReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    qWarning() << "@@@@@ LoginECReplyPacket::extractBody";
    /// FIXME skip all here
    skipN( buf, pos, len );
}

LoginEDPacket::LoginEDPacket() : SessionOutPacket()
{
    m_command = 0x00ed;
}

LoginEDPacket::~LoginEDPacket()
{
}

void LoginEDPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// fixed 0x01
    fill1( buf, pos, 0x01 );
}

LoginEDReplyPacket::LoginEDReplyPacket() : SessionInPacket()
{
    m_command = 0x00ed;
}

LoginEDReplyPacket::~LoginEDReplyPacket()
{
}

void LoginEDReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    qWarning() << "@@@@@ LoginEDReplyPacket::extractBody";
    /// FIXME skip all here
    skipN( buf, pos, len );
}
