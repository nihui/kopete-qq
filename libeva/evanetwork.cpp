#include "evanetwork.h"

#include "evafriend.h"
#include "evagroup.h"
#include "evaimreceive.h"
#include "evaimsend.h"
#include "evakeepalive.h"
#include "evalevel.h"
#include "evalogin.h"
#include "evalogout.h"
#include "evaonlinestatus.h"
#include "evasignature.h"
#include "evasocket.h"
#include "evauserinfo.h"
#include "evaweather.h"

#include <QByteArray>
#include <QDebug>

#ifdef _WIN32
#include <winsock.h>
#else // _WIN32
#include <arpa/inet.h>
#endif // _WIN32

EvaNetwork::EvaNetwork()
{
    m_socket = new EvaSocket;
    m_isOffline = true;

    m_redirect = false;
    m_numberRedirected = 0x00;
    m_serverFlag = 0;
    m_serverVersion = 0;

    connect( m_socket, SIGNAL(ready()), this, SLOT(slotReady()) );
    connect( m_socket, SIGNAL(packetReceived(InPacket*)), this, SLOT(handlePacket(InPacket*)) );
}

EvaNetwork::~EvaNetwork()
{
    delete m_socket;
}

void EvaNetwork::login( const QByteArray& password )
{
    m_socket->setHost( "sz2.tencent.com", 8000 );
    m_socket->connect();
    m_password = password;
}

void EvaNetwork::logout()
{
    if ( isOffline() )
        return;

    /// stop keep alive here
    m_socket->stopKeepAlive();

    /// send 4 times just like Tencent QQ
    m_socket->sendPacket( new LogoutPacket );
    m_socket->sendPacket( new LogoutPacket );
    m_socket->sendPacket( new LogoutPacket );
    m_socket->sendPacket( new LogoutPacket );
    m_isOffline = true;
    emit logoutSuccessed();
}

bool EvaNetwork::isOffline() const
{
    return m_isOffline;
}

void EvaNetwork::setOnlineStatus( unsigned char status )
{
    ChangeStatusPacket* p = new ChangeStatusPacket;
    p->setStatus( status );
    m_socket->sendPacket( p );
}

void EvaNetwork::sendMessage( unsigned int QQId, const QString& messageText )
{
    SendImPacket* p = new SendImPacket;
    p->setReceiver( QQId );
    p->setMsgText( (const unsigned char*)messageText.toUtf8().constData(), messageText.toUtf8().length() );
    m_socket->sendPacket( p );
}

void EvaNetwork::requestGroups()
{
    GroupNameOpPacket* p = new GroupNameOpPacket;
    m_socket->sendPacket( p );
}

void EvaNetwork::requestFriends()
{
    GetFriendListPacket* p = new GetFriendListPacket;
    m_socket->sendPacket( p );
}

void EvaNetwork::requestOnlineFriends()
{
    GetOnlineFriendsPacket* p = new GetOnlineFriendsPacket;
    m_socket->sendPacket( p );
}

void EvaNetwork::requestLevel( unsigned int QQId )
{
    GetLevelPacket* p = new GetLevelPacket;
    m_socket->sendPacket( p );
}

void EvaNetwork::requestLevels( const QList<unsigned int>& QQIds )
{
}

void EvaNetwork::requestSignature( unsigned int QQId )
{
    SignaturePacket* p = new SignaturePacket;
    p->addQQId( QQId );
    m_socket->sendPacket( p );
}

void EvaNetwork::requestSignatures( const QList<unsigned int>& QQIds )
{
    SignaturePacket* p = new SignaturePacket;
    QList<unsigned int>::ConstIterator it = QQIds.constBegin();
    QList<unsigned int>::ConstIterator end = QQIds.constEnd();
    while ( it != end ) {
        unsigned int QQId = *it;
        ++it;
        if ( !p->addQQId( QQId ) ) {
            /// full, send it
            m_socket->sendPacket( p );
            p = new SignaturePacket;
            p->addQQId( QQId );
        }
    }
    m_socket->sendPacket( p );
}

void EvaNetwork::requestFriendDetails( unsigned int QQId )
{
    GetUserInfoPacket* p = new GetUserInfoPacket;
    p->setQQId( QQId );
    m_socket->sendPacket( p );
}

void EvaNetwork::requestWeather()
{
    WeatherOpPacket* p = new WeatherOpPacket;
    m_socket->sendPacket( p );
}

void EvaNetwork::slotReady()
{
    qWarning() << "EvaNetwork::slotReady";
    /// send touch packet
    TouchPacket* p = new TouchPacket;
    p->setRedirect( m_redirect );
    p->setNumberRedirected( m_numberRedirected );
    p->setServerFlag( m_serverFlag );
    p->setServerVersion( m_serverVersion );
    m_socket->sendPacket( p );
}

void EvaNetwork::handlePacket( InPacket* in )
{
    qWarning() << "EvaNetwork::handlePacket";
    switch ( in->command() ) {
        case 0x0091: {
            TouchReplyPacket* p = static_cast<TouchReplyPacket*>(in);
            m_redirect = p->redirect();
            if ( !m_redirect ) {
                m_numberRedirected = p->numberRedirected();
                m_serverFlag = p->serverFlag();
                m_serverVersion = p->serverVersion();

                unsigned int serverIp = p->redirectIp();
                char serverIpStr[INET_ADDRSTRLEN];
                inet_ntop( AF_INET, &serverIp, serverIpStr, INET_ADDRSTRLEN );

                m_socket->setHost( serverIpStr, 8000 );
                m_socket->connect();
            }
            else {
                /// no redirection needed, send captcha packet
                CaptchaPacket* out = new CaptchaPacket;
                m_socket->sendPacket( out );
            }
            break;
        }
        case 0x00ba: {
            CaptchaReplyPacket* p = static_cast<CaptchaReplyPacket*>(in);
            if ( p->checkResultFlag() == 0x03 ) {
                /// successful
                AuthenticatePacket* out = new AuthenticatePacket;
                out->setPassword( (unsigned char*)m_password.constData(), m_password.length() );
                m_socket->sendPacket( out );
            }
            break;
        }
        case 0x00dd: {
            AuthenticateReplyPacket* p = static_cast<AuthenticateReplyPacket*>(in);
            if ( p->authFlag() == 0x00 ) {
                /// successful
                VerifyE5Packet* out = new VerifyE5Packet;
                m_socket->sendPacket( out );
            }
            else {
                qWarning() << "login failed";
                emit loginFailed( QString::fromUtf8( (char*)(p->reason()) ) );
            }
            break;
        }
        case 0x00e5: {
            VerifyE5ReplyPacket* p = static_cast<VerifyE5ReplyPacket*>(in);
            ///
            VerifyE3Packet* out = new VerifyE3Packet;
            m_socket->sendPacket( out );
            break;
        }
        case 0x00e3: {
            VerifyE3ReplyPacket* p = static_cast<VerifyE3ReplyPacket*>(in);
            ///
            LoginPacket* out = new LoginPacket;
            m_socket->sendPacket( out );
            break;
        }
        case 0x0030: {
            LoginReplyPacket* p = static_cast<LoginReplyPacket*>(in);
            ///
            LoginE9Packet* out = new LoginE9Packet;
            m_socket->sendPacket( out );
            break;
        }
        case 0x00e9: {
            LoginE9ReplyPacket* p = static_cast<LoginE9ReplyPacket*>(in);
            ///
            LoginEAPacket* out = new LoginEAPacket;
            m_socket->sendPacket( out );
            break;
        }
        case 0x00ea: {
            LoginEAReplyPacket* p = static_cast<LoginEAReplyPacket*>(in);
            ///
            GetListPacket* out = new GetListPacket;
            m_socket->sendPacket( out );
            break;
        }
        case 0x00eb: {
            GetListReplyPacket* p = static_cast<GetListReplyPacket*>(in);
            unsigned short entryCount = p->entryCount();
            const GetListReplyPacket::BQEntry* entries = p->entries();
            for ( unsigned short i = 0; i < entryCount; ++i ) {
                const GetListReplyPacket::BQEntry* e = entries + i;
                emit gotBQEntry( e->QQId, e->type, e->groupId );
            }
            if ( p->index() < p->indexCount() ) {
                /// get more list data
                GetListPacket* out = new GetListPacket;
                out->setIndex( p->index() + 1 );
                m_socket->sendPacket( out );
            }
            else {
                ///
                LoginEDPacket* out = new LoginEDPacket;
                m_socket->sendPacket( out );
            }
            break;
        }
        case 0x00ec: {
            LoginECReplyPacket* p = static_cast<LoginECReplyPacket*>(in);

            m_isOffline = false;
            emit loginSuccessed();

            /// keep alive starts here
            m_socket->startKeepAlive();

            break;
        }
        case 0x00ed: {
            LoginEDReplyPacket* p = static_cast<LoginEDReplyPacket*>(in);
            ///
            LoginECPacket* out = new LoginECPacket;
            m_socket->sendPacket( out );
            break;
        }
        case 0x003c: {
            GetUserInfoReplyPacket* p = static_cast<GetUserInfoReplyPacket*>(in);
            break;
        }
        case 0x0081: {
            FriendChangeStatusPacket* p = static_cast<FriendChangeStatusPacket*>(in);
            emit userStatusChanged( p->QQId(), p->newStatus() );
            break;
        }
        case 0x000d: {
            ChangeStatusReplyPacket* p = static_cast<ChangeStatusReplyPacket*>(in);
            break;
        }
        case 0x00ce:
        case 0x0017: {
            ReceiveIMPacket* p = static_cast<ReceiveIMPacket*>(in);
            switch ( p->messageType() ) {
                case 0x000a:    /// unknown message 0x000a
                case 0x0009:    /// buddy message 0x0009
                case 0x00a6:    /// buddy message 0x00a6
                case 0x00a7:    /// buddy message 0x00a7
                case 0x0078:    /// buddy message 0x0078
                case 0x0084:    /// buddy message 0x0084
                case 0x0085:    /// buddy message 0x0085
                    emit messageReceived( p->sender(), QString::fromUtf8( (char*)p->messageText() ) );
                    break;
                case 0x0030:   /// system message
                    emit systemMessageReceived( QString::fromUtf8( (char*)p->messageText() ) );
                    break;
            }
            /// ack
            ReceiveIMAckPacket* out = new ReceiveIMAckPacket;
            out->setCommand( p->command() );
            out->setSequence( p->sequence() );
            out->setSender( p->sender() );
            out->setReceiver( p->receiver() );
            out->setMsgseq( p->msgseq() );
            out->setServerIp( p->serverIp() );
            m_socket->sendPacket( out );
            break;
        }
        case 0x00cd: {
            SendImReplyPacket* p = static_cast<SendImReplyPacket*>(in);
            break;
        }
        case 0x0001: {
            GroupNameOpReplyPacket* p = static_cast<GroupNameOpReplyPacket*>(in);
            int entryCount = p->entryCount();
            const GroupNameOpReplyPacket::NameEntry* entries = p->entries();
            for ( int i = 0; i < entryCount; ++i ) {
                const GroupNameOpReplyPacket::NameEntry e = entries[i];
                emit gotGroup( e.groupId, QString::fromUtf8( (char*)( e.groupName ) ) );
            }
            emit noMoreGroups();
            break;
        }
        case 0x0027: {
            GetOnlineFriendsReplyPacket* p = static_cast<GetOnlineFriendsReplyPacket*>(in);
            int QQIdCount = p->QQIdCount();
            const GetOnlineFriendsReplyPacket::StatusEntry* statusEntries = p->statusEntries();
            for ( int i = 0; i < QQIdCount; ++i ) {
                const GetOnlineFriendsReplyPacket::StatusEntry e = statusEntries[i];
                emit userStatusChanged( e.QQId, e.status );
            }
            break;
        }
        case 0x0126: {
            GetFriendListReplyPacket* p = static_cast<GetFriendListReplyPacket*>(in);
            int friendEntryCount = p->friendEntryCount();
            const GetFriendListReplyPacket::FriendEntry* friendEntries = p->friendEntries();
            for ( int i = 0; i < friendEntryCount; ++i ) {
                const GetFriendListReplyPacket::FriendEntry e = friendEntries[i];
                emit gotFriend( e.QQId, e.face, QString::fromUtf8( (char*)(e.nickname) ) );
            }
            unsigned short nextPosition = p->nextPosition();
            if ( nextPosition > 0 && nextPosition < 0xffff ) {
                GetFriendListPacket* out = new GetFriendListPacket;
                out->setPosition( nextPosition );
                m_socket->sendPacket( out );
            }
            else {
                emit noMoreFriends();
            }
            break;
        }
        case 0x00a6: {
            WeatherOpReplyPacket* p = static_cast<WeatherOpReplyPacket*>(in);
            break;
        }
        case 0x005c: {
            GetLevelReplyPacket* p = static_cast<GetLevelReplyPacket*>(in);
            break;
        }
        case 0x0067: {
            SignatureReplyPacket* p = static_cast<SignatureReplyPacket*>(in);
            int signEntryCount = p->signEntryCount();
            const SignatureReplyPacket::SignEntry* signEntries = p->signEntries();
            for ( int i = 0; i < signEntryCount; ++i ) {
                const SignatureReplyPacket::SignEntry e = signEntries[i];
                emit gotSignature( e.QQId, QString::fromUtf8( (char*)e.signature ) );
            }
            break;
        }
        case 0x0058: {
            KeepAliveReplyPacket* p = static_cast<KeepAliveReplyPacket*>(in);
            break;
        }
        default: {
            unsigned short cmd = in->command();
            qWarning() << "Unhandled command" << QByteArray( (char*)&cmd, 2 ).toHex();
            break;
        }
    }
    delete in;
}
