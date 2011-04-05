#ifndef EVAUHSOCKET_H
#define EVAUHSOCKET_H

#include <QObject>

class QUdpSocket;
class UHInPacket;
class UHOutPacket;

class EvaUHSocket : public QObject
{
    Q_OBJECT
    public:
        explicit EvaUHSocket();
        virtual ~EvaUHSocket();
        void sendPacket( UHOutPacket* out );
    public Q_SLOTS:
        void connect();
        void close();
    Q_SIGNALS:
        void packetReceived( UHInPacket* in );
        void ready();
    private Q_SLOTS:
        void slotReadyRead();
    private:
        QUdpSocket* m_socket;
};

#endif // EVAUHSOCKET_H
