#include "qqchatsession.h"
#include "qqaccount.h"

#include <kopetechatsessionmanager.h>
#include <kopetecontact.h>
#include <kopetemessage.h>
#include <kopeteprotocol.h>

#include <KAction>
#include <KActionCollection>
#include <KDebug>
#include <KIcon>
#include <KLocale>
#include <KToggleAction>

QQChatSession::QQChatSession( const Kopete::Contact* user,
                                      Kopete::ContactPtrList others,
                                      Kopete::Protocol* protocol,
                                      Kopete::ChatSession::Form form )
: Kopete::ChatSession(user,others,protocol,form)
{
    Kopete::ChatSessionManager::self()->registerChatSession( this );
    setComponentData( protocol->componentData() );

    QObject::connect( this, SIGNAL(messageSent(Kopete::Message&,Kopete::ChatSession*)),
                      this, SLOT(slotMessageSent(Kopete::Message&,Kopete::ChatSession*)) );

    m_sendNudge = new KAction( this );
    m_sendNudge->setIcon( KIcon( "preferences-desktop-notification-bell" ) );
    m_sendNudge->setText( i18n( "send nudge" ) );
    actionCollection()->addAction( "sendNudge", m_sendNudge );
//     connect( m_sendNudge, SIGNAL(triggered()), this, SLOT(slotSendNudge()) );

    setXMLFile( "qqchatui.rc" );
}

QQChatSession::~QQChatSession()
{
    qWarning() << "QQChatSession::~QQChatSession";
}

void QQChatSession::slotMessageSent( Kopete::Message& message, Kopete::ChatSession* chatSession )
{
    QQAccount* acc = static_cast<QQAccount*>(account());
    acc->sendMessage( members().first()->contactId().toUInt(), message.plainBody() );
    appendMessage( message );
    messageSucceeded();
}
