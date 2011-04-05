#ifndef QQSOCKET_H
#define QQSOCKET_H

#include <QObject>

class QAbstractSocket;
class QBasicTimer;

class InPacket;
class OutPacket;

class EvaSocket : public QObject
{
    Q_OBJECT
    public:
        typedef enum { TCP, UDP } ConnectionType;
        explicit EvaSocket( QObject* parent = 0 );
        virtual ~EvaSocket();
        void setConnectionType( ConnectionType type );
        void setHost( const QString& host, int port );
        /// here send the packet
        void sendPacket( OutPacket* out );
        void startKeepAlive();
        void stopKeepAlive();
    public Q_SLOTS:
        void connect();
        void close();
    Q_SIGNALS:
        void ready();
        /// here receive the packet
        void packetReceived( InPacket* in );
    protected:
        virtual void timerEvent( QTimerEvent* event );
    private Q_SLOTS:
        void slotReadyRead();
    private:
        ConnectionType m_connectionType;
        QString m_host;
        int m_port;

        QBasicTimer* m_keepAliveTimer;

        /// tcp or udp
        QAbstractSocket* m_socket;
};

#endif // QQSOCKET_H
