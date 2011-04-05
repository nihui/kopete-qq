#ifndef QQPROTOCOL_H
#define QQPROTOCOL_H

#include <kopeteprotocol.h>
#include <QVariantList>

class QQProtocol : public Kopete::Protocol
{
    Q_OBJECT
    public:
        explicit QQProtocol( QObject* parent, const QVariantList& args );
        virtual ~QQProtocol();
        /** Generate the widget needed to add QQAccount */
        virtual AddContactPage* createAddContactWidget( QWidget* parent, Kopete::Account* account );
        /** Generate the widget needed to add/edit accounts for this protocol */
        virtual KopeteEditAccountWidget* createEditAccountWidget( Kopete::Account* account, QWidget* parent);
        /** Generate a QQAccount */
        virtual Kopete::Account* createNewAccount( const QString& accountId );
        /** Convert the serialized data back into a QQAccount and add this to its Kopete::MetaContact */
        virtual Kopete::Contact* deserializeContact( Kopete::MetaContact* metaContact,
                                                     const QMap<QString, QString>& serializedData,
                                                     const QMap<QString, QString>& addressBookData );
        /** Access the instance of this protocol */
        static QQProtocol* protocol();
        /** Validate whether userId is a legal QQ account */
        static bool validContactId( const QString& userId );

        const Kopete::OnlineStatus qqOffline;
        const Kopete::OnlineStatus qqInvisible;
        const Kopete::OnlineStatus qqAway;
        const Kopete::OnlineStatus qqOutForLaunch;
        const Kopete::OnlineStatus qqRightBack;
        const Kopete::OnlineStatus qqOnline;
        const Kopete::OnlineStatus qqOnThePhone;
        const Kopete::OnlineStatus qqBusy;
        const Kopete::OnlineStatus qqDoNotDisturb;
        const Kopete::OnlineStatus qqMeeting;
    protected:
        static QQProtocol* s_protocol;
};

#endif // QQPROTOCOL_H
