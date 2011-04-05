#ifndef QQACCOUNT_H
#define QQACCOUNT_H

#include <kopetemessage.h>
#include <kopetepasswordedaccount.h>

class KActionMenu;
class QQProtocol;
class QQContact;

class EvaNetwork;
class EvaUHManager;

class QQAccount : public Kopete::PasswordedAccount
{
    Q_OBJECT
    public:
        explicit QQAccount( QQProtocol* parent, const QString& accountId );
        virtual ~QQAccount();
        virtual void connectWithPassword( const QString& password );
        virtual void disconnect();
        virtual void fillActionMenu( KActionMenu* actionMenu );
        virtual void setOnlineStatus( const Kopete::OnlineStatus& status,
                                      const Kopete::StatusMessage& reason = Kopete::StatusMessage(),
                                      const OnlineStatusOptions& options = None );
        virtual void setStatusMessage( const Kopete::StatusMessage& statusMessage );
        void sendMessage( unsigned int QQId, const QString& messageText );
        void requestFriendDetails( unsigned int QQId );
    protected:
        virtual bool createContact( const QString& contactId, Kopete::MetaContact* parentContact );
    private Q_SLOTS:
        void slotLoginSuccessed();
        void slotLoginFailed( const QString& reason );
        void slotLogoutSuccessed();
        void slotMessageReceived( unsigned int QQId, const QString& messageText );
        void slotSystemMessageReceived( const QString& messageText );
        void slotGotGroup( unsigned char groupId, const QString& groupName );
        void slotNoMoreGroups();
        void slotGotFriend( unsigned int QQId, unsigned short face, const QString& nickname );
        void slotNoMoreFriends();
        void slotGotBQEntry( unsigned int QQId, unsigned char type, unsigned char groupId );
        void slotUserStatusChanged( unsigned int QQId, unsigned char status );
        void slotGotSignature( unsigned int QQId, const QString& signature );
        void slotUHImageRetrieved( unsigned int QQId, const QImage& uh );
    private:
        EvaNetwork* m_network;
        EvaUHManager* m_uhManager;
        QHash<unsigned char, QString> m_groupNameHash;
        QMultiHash<unsigned char, unsigned int> m_groupQQHash;
        QMultiHash<unsigned char, unsigned int> m_groupQunHash;
};

#endif // QQACCOUNT_H
