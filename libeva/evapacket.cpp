#include "evapacket.h"

#include "evacrypt.h"

#include <QByteArray>
#include <QDebug>

unsigned short EvaPacket::m_version = 0x1f57;
unsigned int EvaPacket::m_QQNumber = 0;

/// session key
unsigned char sessionKey[16];

OutPacket::OutPacket() : EvaPacket()
{
    static unsigned int seq = 0x5bab;
    ++seq;
    m_sequence = seq;
}

OutPacket::~OutPacket()
{
}

void OutPacket::fill( unsigned char* buf, int* len )
{
    *len = 0;

    /// start tag 02
    fill1( buf, len, 0x02 );

    /// qq version
    fill2( buf, len, EvaPacket::version() );

    /// command
    fill2( buf, len, m_command );

    /// sequence
    fill2( buf, len, m_sequence );

    /// qq number
    fill4( buf, len, EvaPacket::QQNumber() );

    /// header filling
    const unsigned char filling[11] = {
        0x02, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x64, 0x48
    };
    fillN( buf, len, filling, 11 );

    /// packet body
    int bodyLen = 0;
    fillBody( buf + *len, &bodyLen );
    *len = *len + bodyLen;

    /// end tag 03
    fill1( buf, len, 0x03 );
}

InPacket::InPacket() : EvaPacket()
{
}

InPacket::~InPacket()
{
}

void InPacket::extract( const unsigned char* buf, int len )
{
    int pos = 0;

    unsigned char b1;
    unsigned short b2;
    unsigned int b4;

    /// start tag 02
    extract1( buf, &pos, &b1 );

    /// qq version
    extract2( buf, &pos, &b2 );

    /// command
    extract2( buf, &pos, &m_command );

    /// sequence
    extract2( buf, &pos, &m_sequence );

    /// qq number
    extract4( buf, &pos, &b4 );

    /// 3 byte zero filling
    skipN( buf, &pos, 3 );

    /// packet body
    extractBody( buf, &pos, len - pos - 1 );

    /// end tag 03
    extract1( buf, &pos, &b1 );
}

SessionOutPacket::SessionOutPacket()
{
}

SessionOutPacket::~SessionOutPacket()
{
}

void SessionOutPacket::fillBody( unsigned char* buf, int* pos )
{
    unsigned char tmp[1000];
    int tmplen = 0;
    fillSessionBody( tmp, &tmplen );

    qWarning() << "SESSION SEND" << QByteArray( (char*)tmp, tmplen ).toHex();

    unsigned char out[1000];
    int outlen = 1000;
    EvaCrypt::encrypt( tmp, tmplen, sessionKey, out, &outlen );
    fillN( buf, pos, out, outlen );
}

SessionInPacket::SessionInPacket()
{
}

SessionInPacket::~SessionInPacket()
{
}

void SessionInPacket::extractBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char* tmp = (unsigned char*)malloc( len );
    extractN( buf, pos, tmp, len );

    unsigned char out[1000];
    int outlen = 1000;
    EvaCrypt::decrypt( tmp, len, sessionKey, out, &outlen );
    free( tmp );

    qWarning() << "SESSION RECV" << QByteArray( (char*)out, outlen ).toHex();

    int pout = 0;
    extractSessionBody( out, &pout, outlen );
}
