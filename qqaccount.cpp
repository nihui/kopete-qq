#include "qqaccount.h"

#include "qqchatsession.h"
#include "qqcontact.h"
#include "qqprotocol.h"

/// libeva headers
#include "libeva/libuh/evauhmanager.h"
#include "libeva/evapacket.h"
#include "libeva/evanetwork.h"

#include <kopeteavatarmanager.h>
#include <kopetechatsession.h>
#include <kopetecontactlist.h>
#include <kopetegroup.h>
#include <kopetemetacontact.h>
#include <kopeteonlinestatus.h>
#include <kopetestatusmessage.h>

#include <KAction>
#include <KActionMenu>
#include <KDialog>
#include <KLocale>
#include <KMessageBox>

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <KDebug>

QQAccount::QQAccount( QQProtocol* parent, const QString& accountId )
: Kopete::PasswordedAccount( parent, accountId )
{
    EvaPacket::setVersion( 0x1f57 );

    setMyself( new QQContact( this, accountId, Kopete::ContactList::self()->myself() ) );
    myself()->setOnlineStatus( QQProtocol::protocol()->qqOffline );

    m_network = new EvaNetwork;
    QObject::connect( m_network, SIGNAL(loginSuccessed()),
                      this, SLOT(slotLoginSuccessed()) );
    QObject::connect( m_network, SIGNAL(loginFailed(const QString&)),
                      this, SLOT(slotLoginFailed(const QString&)) );
    QObject::connect( m_network, SIGNAL(logoutSuccessed()),
                      this, SLOT(slotLogoutSuccessed()) );
    QObject::connect( m_network, SIGNAL(messageReceived(unsigned int,const QString&)),
                      this, SLOT(slotMessageReceived(unsigned int,const QString&)) );
    QObject::connect( m_network, SIGNAL(systemMessageReceived(const QString&)),
                      this, SLOT(slotSystemMessageReceived(const QString&)) );
    QObject::connect( m_network, SIGNAL(gotGroup(unsigned char,const QString&)),
                      this, SLOT(slotGotGroup(unsigned char,const QString&)) );
    QObject::connect( m_network, SIGNAL(noMoreGroups()),
                      this, SLOT(slotNoMoreGroups()) );
    QObject::connect( m_network, SIGNAL(gotFriend(unsigned int,unsigned short,const QString&)),
                      this, SLOT(slotGotFriend(unsigned int,unsigned short,const QString&)) );
    QObject::connect( m_network, SIGNAL(noMoreFriends()),
                      this, SLOT(slotNoMoreFriends()) );
    QObject::connect( m_network, SIGNAL(gotBQEntry(unsigned int,unsigned char,unsigned char)),
                      this, SLOT(slotGotBQEntry(unsigned int,unsigned char,unsigned char)) );
    QObject::connect( m_network, SIGNAL(userStatusChanged(unsigned int,unsigned char)),
                      this, SLOT(slotUserStatusChanged(unsigned int,unsigned char)) );
    QObject::connect( m_network, SIGNAL(gotSignature(unsigned int,const QString&)),
                      this, SLOT(slotGotSignature(unsigned int,const QString&)) );

    m_uhManager = new EvaUHManager;
    QObject::connect( m_uhManager, SIGNAL(UHImageRetrieved(unsigned int,const QImage&)),
                      this, SLOT(slotUHImageRetrieved(unsigned int,const QImage&)) );
}

QQAccount::~QQAccount()
{
    qWarning() << "QQAccount::~QQAccount()";
    delete m_network;
    delete m_uhManager;
}

void QQAccount::connectWithPassword( const QString& password )
{
    if ( !m_network->isOffline() )
        return;

    EvaPacket::setQQNumber( accountId().toInt() );
    m_network->login( password.toAscii() );
}

void QQAccount::disconnect()
{
    m_network->logout();
}

void QQAccount::fillActionMenu( KActionMenu* actionMenu )
{
    Kopete::Account::fillActionMenu( actionMenu );
    actionMenu->addSeparator();
}

void QQAccount::setOnlineStatus( const Kopete::OnlineStatus& status,
                                     const Kopete::StatusMessage& reason,
                                     const OnlineStatusOptions& options )
{
    Kopete::OnlineStatus oldStatus = myself()->onlineStatus();

    if ( !oldStatus.isDefinitelyOnline() && status.isDefinitelyOnline() ) {
        connect();
        return;
    }
    if ( oldStatus.isDefinitelyOnline() && !status.isDefinitelyOnline() ) {
        disconnect();
        return;
    }
    if ( !oldStatus.isDefinitelyOnline() && !status.isDefinitelyOnline() ) {
        return;
    }

    /// change status id
    unsigned char s = 10;
    switch ( status.status() ) {
        case Kopete::OnlineStatus::Online:
            s = 10;
            break;
        case Kopete::OnlineStatus::Away:
            s = 30;
            break;
        case Kopete::OnlineStatus::Invisible:
            s = 40;
            break;
        case Kopete::OnlineStatus::Busy:
            s = 50;
            break;
    }
    m_network->setOnlineStatus( s );
    myself()->setOnlineStatus( status );
}

void QQAccount::setStatusMessage( const Kopete::StatusMessage& statusMessage )
{
}

void QQAccount::sendMessage( unsigned int QQId, const QString& messageText )
{
    m_network->sendMessage( QQId, messageText );
}

void QQAccount::requestFriendDetails( unsigned int QQId )
{
    m_network->requestFriendDetails( QQId );
}

bool QQAccount::createContact( const QString& contactId, Kopete::MetaContact* parentContact )
{
    if ( !contacts().value( contactId ) ) {
        QQContact* newContact = new QQContact( this, contactId, parentContact );
        return newContact != NULL;
    }
    return false;
}

void QQAccount::slotLoginSuccessed()
{
    myself()->setOnlineStatus( QQProtocol::protocol()->qqOnline );
    m_network->requestGroups();
}

void QQAccount::slotLoginFailed( const QString& reason )
{
    KMessageBox::information( 0, reason );
    myself()->setOnlineStatus( QQProtocol::protocol()->qqOffline );
}

void QQAccount::slotLogoutSuccessed()
{
    myself()->setOnlineStatus( QQProtocol::protocol()->qqOffline );
}

void QQAccount::slotMessageReceived( unsigned int QQId, const QString& messageText )
{
    QQContact* contact = qobject_cast<QQContact*>(contacts().value( QString::number( QQId ) ));
    if ( !contact ) {
        qWarning() << "slotMessageReceived unknown sender" << QQId << messageText;
        return;
    }
    contact->slotMessageReceived( messageText );
}

void QQAccount::slotSystemMessageReceived( const QString& messageText )
{
    KMessageBox::information( 0, messageText );
}

void QQAccount::slotGotGroup( unsigned char groupId, const QString& groupName )
{
    kWarning() << groupId << groupName;
    m_groupNameHash.insert( groupId, groupName );
    Kopete::Group* group = Kopete::ContactList::self()->findGroup( groupName );

    /// add contact to list
    QList<unsigned int> list = m_groupQQHash.values( groupId );
    QList<unsigned int>::ConstIterator it = list.constBegin();
    QList<unsigned int>::ConstIterator end = list.constEnd();
    while ( it != end ) {
        QString contactId = QString::number( *it );
        Kopete::Contact* contact = contacts().value( contactId );
        if ( !contact ) {
            Kopete::MetaContact* mc = addContact( contactId, contactId, group );
        }
        if ( contact && contact->property( Kopete::Global::Properties::self()->photo() ).isNull() ) {
            m_uhManager->requestUHImage( *it );
        }
        ++it;
    }

    /// add qun to list
    list = m_groupQunHash.values( groupId );
    it = list.constBegin();
    end = list.constEnd();
    while ( it != end ) {
        QString qunId = QString::number( *it );
        Kopete::Contact* contact = contacts().value( qunId );
        if ( !contact ) {
            Kopete::MetaContact* mc = addContact( qunId, qunId, group );
        }
        /// qun is always online
        contact->setOnlineStatus( Kopete::OnlineStatus::Online );
        ++it;
    }
}

void QQAccount::slotNoMoreGroups()
{
    m_network->requestFriends();
}

void QQAccount::slotGotFriend( unsigned int QQId, unsigned short face, const QString& nickname )
{
    Kopete::Contact* contact = contacts().value( QString::number( QQId ) );
    if ( contact ) {
        contact->setNickName( nickname );
        contact->metaContact()->setDisplayName( nickname );
    }
}

void QQAccount::slotNoMoreFriends()
{
    m_network->requestOnlineFriends();
    m_network->requestSignatures( m_groupQQHash.values() );
}

void QQAccount::slotGotBQEntry( unsigned int QQId, unsigned char type, unsigned char groupId )
{
    kWarning() << QQId << type << groupId;
    if ( type == 0x01 ) {
        if ( groupId == 0x00 ) {
            /// default group
            Kopete::Group* group = Kopete::ContactList::self()->findGroup( "QQ Friends" );
            QString contactId = QString::number( QQId );
            Kopete::Contact* contact = contacts().value( contactId );
            if ( !contact ) {
                Kopete::MetaContact* mc = addContact( contactId, contactId, group );
            }
            if ( contact && contact->property( Kopete::Global::Properties::self()->photo() ).isNull() ) {
                m_uhManager->requestUHImage( QQId );
            }
        }
        m_groupQQHash.insert( groupId, QQId );
    }
    else if ( type == 0x04 ) {
        /// qun
        if ( groupId == 0x00 ) {
            /// default group
            Kopete::Group* group = Kopete::ContactList::self()->findGroup( "QQ Quns" );
            QString qunId = QString::number( QQId );
            Kopete::Contact* contact = contacts().value( qunId );
            if ( !contact ) {
                Kopete::MetaContact* mc = addContact( qunId, qunId, group );
            }
            /// qun is always online
            contact->setOnlineStatus( Kopete::OnlineStatus::Online );
        }
        m_groupQQHash.insert( groupId, QQId );
    }
}

void QQAccount::slotUserStatusChanged( unsigned int QQId, unsigned char status )
{
    Kopete::OnlineStatus s;
    switch ( status ) {
        case 0: /// offline
            s = Kopete::OnlineStatus::Offline;
            break;
        case 10:/// online normal
            s = Kopete::OnlineStatus::Online;
            break;
        case 20:/// change to offline  FIXME: ????
            s = Kopete::OnlineStatus::Away;
            break;
        case 30:/// away
            s = Kopete::OnlineStatus::Away;
            break;
        case 40:/// invisible
            s = Kopete::OnlineStatus::Invisible;
            break;
        case 50:/// busy
            s = Kopete::OnlineStatus::Busy;
            break;
        default:
            break;
    }

    Kopete::Contact* contact = contacts().value( QString::number( QQId ) );
    if ( contact ) {
        contact->setOnlineStatus( s );
    }
}

void QQAccount::slotGotSignature( unsigned int QQId, const QString& signature )
{
    QQContact* contact = qobject_cast<QQContact*>(contacts().value( QString::number( QQId ) ));
    if ( !contact ) {
        qWarning() << "slotGotSignature unknown contact" << QQId;
        return;
    }
    contact->setStatusMessage( signature );
}

void QQAccount::slotUHImageRetrieved( unsigned int QQId, const QImage& uh )
{
    QQContact* contact = qobject_cast<QQContact*>(contacts().value( QString::number( QQId ) ));
    if ( !contact ) {
        qWarning() << "slotUHImageRetrieved unknown contact" << QQId;
        return;
    }

    Kopete::AvatarManager::AvatarEntry newEntry;
    newEntry.name = "kopete-qq" + QString::number( QQId );
    newEntry.image = uh.scaled( 128, 128 );
    newEntry.contact = contact;
    newEntry.category = Kopete::AvatarManager::Contact;

    newEntry = Kopete::AvatarManager::self()->add( newEntry );
    contact->removeProperty( Kopete::Global::Properties::self()->photo() );
    contact->setProperty( Kopete::Global::Properties::self()->photo(), newEntry.path );
}

