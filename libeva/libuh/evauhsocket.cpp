#include "evauhsocket.h"

#include "evauhinfo.h"
#include "evauhpacket.h"
#include "evauhtransfer.h"

#include <QUdpSocket>

#ifdef _WIN32
#include <winsock.h>
#else // _WIN32
#include <arpa/inet.h>
#endif // _WIN32

EvaUHSocket::EvaUHSocket()
{
    m_socket = new QUdpSocket;
    QObject::connect( m_socket, SIGNAL(connected()), this, SIGNAL(ready()) );
    QObject::connect( m_socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()) );
}

EvaUHSocket::~EvaUHSocket()
{
    delete m_socket;
}

void EvaUHSocket::sendPacket( UHOutPacket* out )
{
    unsigned char tmp[1000] = {0};
    int len;

    out->fill( tmp, &len );
    delete out;

    qWarning() << "SEND" << QByteArray( (char*)tmp, len ).toHex();

    m_socket->write( (char*)tmp, len );
}

void EvaUHSocket::connect()
{
    m_socket->connectToHost( "cface_tms.qq.com", 4000 );
    m_socket->waitForConnected( 3000 );
}

void EvaUHSocket::close()
{
    m_socket->close();
}

void EvaUHSocket::slotReadyRead()
{
    int byteCount = m_socket->bytesAvailable();
    qWarning() << "byteCount" << byteCount;
    unsigned char* tmp = (unsigned char*)malloc(byteCount);
    int ret = m_socket->read( (char*)tmp, byteCount );
    qWarning() << "readCount" << ret;

    qWarning() << "RECV" << QByteArray( (char*)tmp, ret ).toHex();

    unsigned char command;
    memcpy( &command, tmp + 1, 1 );

    UHInPacket* p = NULL;
    switch ( command ) {
        case 0x02: {
            if ( ret == 46 ) {
                /// ignore useless 0x02 reply packet
                free( tmp );
                return;
            }
            p = new UHTransferPacket;
            break;
        }
        case 0x04: {
            p = new UHInfoReplyPacket;
            break;
        }
        default: {
            qWarning() << "unknown uh command type" << QByteArray( (char*)&command, 1 ).toHex();
            free( tmp );
            return;
        }
    }
    p->extract( tmp, byteCount );
    free( tmp );
    emit packetReceived( p );
}
