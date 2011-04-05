#ifndef EVAUHMANAGER_H
#define EVAUHMANAGER_H

#include <QObject>
#include <QHash>

class QImage;
class EvaUHSocket;
class UHInPacket;

class EvaUHManager : public QObject
{
    Q_OBJECT
    public:
        explicit EvaUHManager();
        virtual ~EvaUHManager();
        void requestUHImage( unsigned int QQId );
        void requestUHImages( const unsigned int* QQIds, int count );
    Q_SIGNALS:
        void UHImageRetrieved( unsigned int QQId, const QImage& uh );
    private Q_SLOTS:
        void handlePacket( UHInPacket* in );
    private:
        EvaUHSocket* m_socket;
        QHash<unsigned int, unsigned char*> m_QQUHs;
};

#endif // EVAUHMANAGER_H
