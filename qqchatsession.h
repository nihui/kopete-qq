#ifndef QQCHATSESSION_H
#define QQCHATSESSION_H

#include <kopetechatsession.h>
class KAction;
class KToggleAction;
class QQSipNotifier;

class QQChatSession : public Kopete::ChatSession
{
    Q_OBJECT
    public:
        explicit QQChatSession( const Kopete::Contact* user,
                                     Kopete::ContactPtrList others,
                                     Kopete::Protocol* protocol,
                                     Kopete::ChatSession::Form form = Small );
        virtual ~QQChatSession();
    private Q_SLOTS:
        void slotMessageSent( Kopete::Message& message, Kopete::ChatSession* chatSession );
//         void slotSendPending();
    private:
        KAction* m_sendNudge;
};

#endif // QQCHATSESSION_H
