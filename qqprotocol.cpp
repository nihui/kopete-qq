#include "qqprotocol.h"

#include "qqaccount.h"
#include "qqaddcontactpage.h"
#include "qqcontact.h"
#include "qqeditaccountwidget.h"

#include <kopeteaccountmanager.h>

#include <kgenericfactory.h>
#include <KDebug>
#include <QList>

K_PLUGIN_FACTORY( QQProtocolFactory, registerPlugin<QQProtocol>(); )
K_EXPORT_PLUGIN( QQProtocolFactory( "kopete_qq" ) )

QQProtocol* QQProtocol::s_protocol = 0L;

QQProtocol::QQProtocol( QObject* parent, const QVariantList& args )
: Kopete::Protocol( QQProtocolFactory::componentData(), parent ),
qqOffline( Kopete::OnlineStatus::Offline, 25, this, 1,
               QStringList(), i18n( "Offline" ), i18n( "O&ffline" ),
               Kopete::OnlineStatusManager::Offline, Kopete::OnlineStatusManager::DisabledIfOffline ),
qqInvisible( Kopete::OnlineStatus::Invisible, 25, this, 2,
                 QStringList( "qq_invisible" ), i18n( "Invisible" ), i18n( "&Invisible" ),
                 Kopete::OnlineStatusManager::Invisible ),
qqAway( Kopete::OnlineStatus::Away, 25, this, 3,
            QStringList( "qq_away" ), i18n( "Away" ), i18n( "&Away" ),
            Kopete::OnlineStatusManager::Away ),
qqOutForLaunch( Kopete::OnlineStatus::Away, 25, this, 4,
                    QStringList( "qq_outforlaunch" ), i18n( "Out for launch" ), i18n( "O&ut for launch" ),
                    Kopete::OnlineStatusManager::Away ),
qqRightBack( Kopete::OnlineStatus::Away, 25, this, 5,
                 QStringList( "qq_rightback" ), i18n( "Right back" ), i18n( "&Right back" ),
                 Kopete::OnlineStatusManager::Away ),
qqOnline( Kopete::OnlineStatus::Online, 25, this, 6,
              QStringList(), i18n( "Online" ), i18n( "O&nline" ),
              Kopete::OnlineStatusManager::Online ),
qqOnThePhone( Kopete::OnlineStatus::Busy, 25, this, 7,
                  QStringList( "qq_onthephone" ), i18n( "On the phone" ), i18n( "On &the phone" ),
                  Kopete::OnlineStatusManager::Busy ),
qqBusy( Kopete::OnlineStatus::Busy, 25, this, 8,
            QStringList( "qq_busy" ), i18n( "Busy" ), i18n( "&Busy" ),
            Kopete::OnlineStatusManager::Busy ),
qqDoNotDisturb( Kopete::OnlineStatus::Busy, 25, this, 9,
                    QStringList( "qq_donotdisturb" ), i18n( "Do not disturb" ), i18n( "&Do not disturb" ),
                    Kopete::OnlineStatusManager::Busy ),
qqMeeting( Kopete::OnlineStatus::Busy, 25, this, 10,
               QStringList( "qq_meeting" ), i18n( "Meeting" ), i18n( "&Meeting" ),
               Kopete::OnlineStatusManager::Busy )
{
    Q_UNUSED(args)
    s_protocol = this;
}

QQProtocol::~QQProtocol()
{
}

AddContactPage* QQProtocol::createAddContactWidget( QWidget* parent, Kopete::Account* account )
{
    Q_UNUSED(account)
    return new QQAddContactPage( parent );
}

KopeteEditAccountWidget* QQProtocol::createEditAccountWidget( Kopete::Account* account, QWidget* parent )
{
    return new QQEditAccountWidget( parent, account );
}

Kopete::Account *QQProtocol::createNewAccount( const QString &accountId )
{
    return new QQAccount( this, accountId );
}

Kopete::Contact* QQProtocol::deserializeContact( Kopete::MetaContact* metaContact,
                                                     const QMap<QString, QString>& serializedData,
                                                     const QMap<QString, QString>& addressBookData )
{
    QString contactId = serializedData[ "contactId" ];
    QString accountId = serializedData[ "accountId" ];
    QString displayName = serializedData[ "displayName" ];

    QList<Kopete::Account*> accounts = Kopete::AccountManager::self()->accounts( this );
    Kopete::Account* account = 0;
    foreach (Kopete::Account * acct, accounts) {
        if ( acct->accountId () == accountId )
            account = acct;
    }
    if ( !account ) {
        kDebug() << "Account doesn't exist, skipping";
        return 0;
    }

    return new QQContact( account, contactId, metaContact );
}

bool QQProtocol::validContactId( const QString& userId )
{
    QRegExp re("[1-9][0-9]*");
    return re.exactMatch( userId );
}

QQProtocol* QQProtocol::protocol()
{
    return s_protocol;
}

