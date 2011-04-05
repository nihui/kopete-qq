#ifndef QQCONTACT_H
#define QQCONTACT_H

#include <kopetecontact.h>
#include <kopetemessage.h>
#include <QList>

class QQContact : public Kopete::Contact
{
    Q_OBJECT
    public:
        explicit QQContact( Kopete::Account* account, const QString& id, Kopete::MetaContact* parent );
        virtual ~QQContact();
        virtual bool isReachable();
//         virtual QList<KAction*>* customContextMenuActions();
//         virtual QList<KAction*>* customContextMenuActions( Kopete::ChatSession* chatSession );
        virtual Kopete::ChatSession* manager( Kopete::Contact::CanCreateFlags canCreate = Kopete::Contact::CannotCreate );
        virtual void serialize( QMap<QString, QString>& serializedData,
                                QMap<QString, QString>& addressBookData );
    public Q_SLOTS:
        virtual void slotUserInfo();
        void slotMessageReceived( const QString& message );
    private:
};

#endif // QQCONTACT_H
