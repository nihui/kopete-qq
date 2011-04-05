#include "evasocket.h"

#include "evafriend.h"
#include "evagroup.h"
#include "evaimreceive.h"
#include "evaimsend.h"
#include "evakeepalive.h"
#include "evalevel.h"
#include "evalogin.h"
#include "evaonlinestatus.h"
#include "evapacket.h"
#include "evasignature.h"
#include "evauserinfo.h"
#include "evaweather.h"

#include <QBasicTimer>
#include <QTimerEvent>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QTcpSocket>
#include <QUdpSocket>

#ifdef _WIN32
#include <winsock.h>
#else // _WIN32
#include <arpa/inet.h>
#endif // _WIN32

EvaSocket::EvaSocket( QObject* parent )
        : QObject(parent)
{
    m_connectionType = UDP;
    m_keepAliveTimer = new QBasicTimer;
    m_socket = NULL;
}

EvaSocket::~EvaSocket()
{
    delete m_keepAliveTimer;
    delete m_socket;
}

void EvaSocket::setConnectionType( ConnectionType type )
{
    m_connectionType = type;
}

void EvaSocket::setHost( const QString& host, int port )
{
    m_host = host;
    m_port = port;
}

void EvaSocket::sendPacket( OutPacket* out )
{
    unsigned char tmp[1000] = {0};
    int len;

    out->fill( tmp, &len );
    delete out;

    qWarning() << "SEND" << QByteArray( (char*)tmp, len ).toHex();

    m_socket->write( (char*)tmp, len );
}

void EvaSocket::startKeepAlive()
{
    m_keepAliveTimer->start( 60 * 1000, this );
}

void EvaSocket::stopKeepAlive()
{
    m_keepAliveTimer->stop();
}

void EvaSocket::connect()
{
    qWarning() << "EvaSocket::connect";
    close();
    delete m_socket;

    if ( m_connectionType == TCP )
        m_socket = new QTcpSocket;
    else
        m_socket = new QUdpSocket;
    QObject::connect( m_socket, SIGNAL(connected()), this, SIGNAL(ready()) );
    QObject::connect( m_socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()) );

    m_socket->connectToHost( m_host, m_port );
}

void EvaSocket::close()
{
    qWarning() << "EvaSocket::close";
    if ( m_socket )
        m_socket->close();
}

void EvaSocket::timerEvent( QTimerEvent* event )
{
    if ( event->timerId() == m_keepAliveTimer->timerId() ) {
        qWarning() << "SEND KEEP ALIVE PACKET";
        sendPacket( new KeepAlivePacket );
    }
    QObject::timerEvent( event );
}

void EvaSocket::slotReadyRead()
{
    qWarning() << "slotReadyRead";
    int byteCount = m_socket->bytesAvailable();
    qWarning() << "byteCount" << byteCount;
    unsigned char* tmp = (unsigned char*)malloc(byteCount);
    int ret = m_socket->read( (char*)tmp, byteCount );
    qWarning() << "readCount" << ret;

    qWarning() << "RECV" << QByteArray( (char*)tmp, ret ).toHex();

    unsigned short command;
    memcpy( &command, tmp + 3, 2 );
    command = htons( command );


    InPacket* p = NULL;
    switch ( command ) {
    case 0x0091: {
        p = new TouchReplyPacket;
        break;
    }
    case 0x00ba: {
        p = new CaptchaReplyPacket;
        break;
    }
    case 0x00dd: {
        p = new AuthenticateReplyPacket;
        break;
    }
    case 0x00e5: {
        p = new VerifyE5ReplyPacket;
        break;
    }
    case 0x00e3: {
        p = new VerifyE3ReplyPacket;
        break;
    }
    case 0x0030: {
        p = new LoginReplyPacket;
        break;
    }
    case 0x00e9: {
        p = new LoginE9ReplyPacket;
        break;
    }
    case 0x00ea: {
        p = new LoginEAReplyPacket;
        break;
    }
    case 0x00eb: {
        p = new GetListReplyPacket;
        break;
    }
    case 0x00ec: {
        p = new LoginECReplyPacket;
        break;
    }
    case 0x00ed: {
        p = new LoginEDReplyPacket;
        break;
    }
    case 0x003c: {
        p = new GetUserInfoReplyPacket;
        break;
    }
    case 0x0081: {
        p = new FriendChangeStatusPacket;
        break;
    }
    case 0x000d: {
        p = new ChangeStatusReplyPacket;
        break;
    }
    case 0x00ce:
    case 0x0017: {
        p = new ReceiveIMPacket;
        break;
    }
    case 0x00cd: {
        p = new SendImReplyPacket;
        break;
    }
    case 0x0001: {
        p = new GroupNameOpReplyPacket;
        break;
    }
    case 0x0027: {
        p = new GetOnlineFriendsReplyPacket;
        break;
    }
    case 0x0126: {
        p = new GetFriendListReplyPacket;
        break;
    }
    case 0x00a6: {
        p = new WeatherOpReplyPacket;
        break;
    }
    case 0x005c: {
        p = new GetLevelReplyPacket;
        break;
    }
    case 0x0067: {
        p = new SignatureReplyPacket;
        break;
    }
    case 0x0058: {
        p = new KeepAliveReplyPacket;
        break;
    }
    default: {
        qWarning() << "Unknown command" << QByteArray( (char*)&command, 2 ).toHex();
        free(tmp);
        return;
    }
    }

    p->extract( tmp, byteCount );

    free(tmp);

    emit packetReceived( p );
}
