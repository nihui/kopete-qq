#include <QApplication>

#include "evapacket.h"
#include "evanetwork.h"

#include "libuh/evauhmanager.h"

#include <QDebug>
#include <QString>
#include <QByteArray>

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );

    /// qq 2010 sp3.1
    EvaPacket::setVersion( 0x1f57 );
    EvaPacket::setQQNumber( 1234/** FIXME */ );

//     EvaNetwork* s = new EvaNetwork;
//     s->login();

    return app.exec();
}
