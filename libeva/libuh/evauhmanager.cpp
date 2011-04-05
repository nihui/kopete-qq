#include "evauhmanager.h"

#include "evauhinfo.h"
#include "evauhpacket.h"
#include "evauhsocket.h"
#include "evauhtransfer.h"

#include <QFile>
#include <QImage>

#include <stdlib.h>

EvaUHManager::EvaUHManager()
{
    m_socket = new EvaUHSocket;
    connect( m_socket, SIGNAL(packetReceived(UHInPacket*)),
             this, SLOT(handlePacket(UHInPacket*)) );
    m_socket->connect();
}

EvaUHManager::~EvaUHManager()
{
    m_socket->close();
    delete m_socket;
}

void EvaUHManager::requestUHImage( unsigned int QQId )
{
    UHInfoPacket* p = new UHInfoPacket;
    p->addQQId( QQId );
    m_socket->sendPacket( p );
}

void EvaUHManager::requestUHImages( const unsigned int* QQIds, int count )
{
    if ( count <= 0 )
        return;

    /// 20 per packet
    int groups = count / 20;
    for ( int g = 0; g < groups; ++g ) {
        UHInfoPacket* p = new UHInfoPacket;
        for ( int i = 0; i < 20; ++i ) {
            p->addQQId( QQIds[g*20+i] );
        }
        m_socket->sendPacket( p );
    }

    int remains = count % 20;
    if ( remains > 0 ) {
        UHInfoPacket* p = new UHInfoPacket;
        for ( int i = 0; i < remains; ++i ) {
            p->addQQId( QQIds[groups*20+i] );
        }
        m_socket->sendPacket( p );
    }
}

void EvaUHManager::handlePacket( UHInPacket* in )
{
    switch ( in->command() ) {
        case 0x02: {
            UHTransferPacket* p = static_cast<UHTransferPacket*>(in);
            unsigned int qqid = p->QQId();
            unsigned int avatarDataSize = p->avatarSize();
            if ( !m_QQUHs.contains( qqid ) )
                m_QQUHs.insert( qqid, (unsigned char*)malloc( avatarDataSize ) );
            unsigned char* avatarData = m_QQUHs[ qqid ];
            unsigned int pos = p->position();
            unsigned int len = p->imageDataSize();
            memcpy( avatarData + pos, p->imageData(), len );
            if ( pos + len == avatarDataSize ) {
                QImage avatarImage = QImage::fromData( avatarData, avatarDataSize );
                avatarImage.save( QString::number( qqid ) + ".bmp" );
                emit UHImageRetrieved( qqid, avatarImage );
                free( avatarData );
                m_QQUHs.remove( qqid );
            }
            break;
        }
        case 0x04: {
            UHInfoReplyPacket* p = static_cast<UHInfoReplyPacket*>(in);
            unsigned short uhInfoCount = p->uhInfoCount();
            for ( unsigned short i = 0; i < uhInfoCount; ++i ) {
                const UHInfoReplyPacket::UHInfo* uhInfo = p->uhInfoAt( i );
                UHTransferRequestPacket* out = new UHTransferRequestPacket;
                out->setQQId( uhInfo->QQId );
                out->setSessionId( uhInfo->sessionId );
                m_socket->sendPacket( out );
            }
            break;
        }
    }
}
