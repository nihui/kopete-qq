#include "qqcontact.h"

#include "qqaccount.h"
#include "qqchatsession.h"
#include "qqprotocol.h"
// #include "ui_qqcontactinfo.h"

#include <kopeteaccount.h>
#include <kopetechatsessionmanager.h>

#include <KDialog>
#include <KLocale>
#include <QDebug>

QQContact::QQContact( Kopete::Account* account, const QString &id, Kopete::MetaContact* parent )
: Kopete::Contact( account, id, parent )
{
    setOnlineStatus( QQProtocol::protocol()->qqOffline );
}

QQContact::~QQContact()
{
}

bool QQContact::isReachable()
{
    return true;
}

// QList<KAction*>* QQContact::customContextMenuActions()
// {
//     QList<KAction*>* actions = new QList<KAction*>;
//     KAction* a = new KAction( this );
//     a->setText( i18n( "&Reload avatar" ) );
//     connect( a, SIGNAL(triggered()), this, SLOT(reloadAvatar()) );
//     actions->append( a );
//     return actions;
// }
//
// QList<KAction*>* QQContact::customContextMenuActions( Kopete::ChatSession* chatSession )
// {
//     QList<KAction*>* actions = new QList<KAction*>;
//     KAction* a = new KAction( this );
//     a->setText( i18n( "&Reload avatar" ) );
//     connect( a, SIGNAL(triggered()), this, SLOT(reloadAvatar()) );
//     actions->append( a );
//     return actions;
// }

Kopete::ChatSession* QQContact::manager( Kopete::Contact::CanCreateFlags canCreate )
{
    Kopete::ContactPtrList chatmembers;
    chatmembers.append( this );
    Kopete::ChatSession* _m = Kopete::ChatSessionManager::self()->findChatSession( account()->myself(),
                                                                                         chatmembers, protocol() );
    QQChatSession* manager = dynamic_cast<QQChatSession*>(_m);
    if( !manager && canCreate == Kopete::Contact::CanCreate ) {
        manager = new QQChatSession( account()->myself(), chatmembers, protocol() );
    }
    return manager;
}

void QQContact::serialize( QMap<QString, QString>& serializedData,
                               QMap<QString, QString>& addressBookData )
{
    serializedData["avatar"] = property( Kopete::Global::Properties::self()->photo() ).value().toString();
}

void QQContact::slotUserInfo()
{
    QQAccount* acc = static_cast<QQAccount*>(account());
    acc->requestFriendDetails( contactId().toUInt() );
}

void QQContact::slotMessageReceived( const QString& message )
{
    Kopete::Message m = Kopete::Message( this, account()->myself() );
    m.setPlainBody( message );
    m.setDirection( Kopete::Message::Inbound );
    manager( Kopete::Contact::CanCreate )->appendMessage( m );
}
