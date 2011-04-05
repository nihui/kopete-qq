#ifndef EVANETWORK_H
#define EVANETWORK_H

#include <QByteArray>
#include <QObject>

class EvaSocket;
class InPacket;
/// FIXME BLOB CLASS
class EvaNetwork : public QObject
{
    Q_OBJECT
    public:
        explicit EvaNetwork();
        virtual ~EvaNetwork();
        void login( const QByteArray& password );
        void logout();
        bool isOffline() const;
        void setOnlineStatus( unsigned char status );
        void sendMessage( unsigned int QQId, const QString& messageText );
        void requestGroups();
        void requestFriends();
        void requestOnlineFriends();
        void requestLevel( unsigned int QQId );
        void requestLevels( const QList<unsigned int>& QQIds );
        void requestSignature( unsigned int QQId );
        void requestSignatures( const QList<unsigned int>& QQIds );
        void requestFriendDetails( unsigned int QQId );
        void requestWeather();
    Q_SIGNALS:
        void loginSuccessed();
        void loginFailed( const QString& reason );
        void logoutSuccessed();
        void messageReceived( unsigned int QQId, const QString& messageText );
        void systemMessageReceived( const QString& messageText );
        void gotGroup( unsigned char groupId, const QString& groupName );
        void noMoreGroups();
        void gotFriend( unsigned int QQId, unsigned short face, const QString& nickname );
        void noMoreFriends();
        void gotBQEntry( unsigned int QQId, unsigned char type, unsigned char groupId );
        void userStatusChanged( unsigned int QQId, unsigned char status );
        void gotLevel( unsigned int QQId, unsigned short level, unsigned short activeDays );
        void gotSignature( unsigned int QQId, const QString& signature );
        void gotWeather( unsigned int time, const QString& weather, const QString& wind, const QString& hint );
    private Q_SLOTS:
        void slotReady();
        void handlePacket( InPacket* in );
    private:
        EvaSocket* m_socket;
        bool m_isOffline;
        QByteArray m_password;

        bool m_redirect;
        unsigned char m_numberRedirected;
        unsigned int m_serverFlag;
        unsigned int m_serverVersion;
};

#endif // EVANETWORK_H
