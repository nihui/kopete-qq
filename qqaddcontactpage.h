#ifndef QQADDCONTACTPAGE_H
#define QQADDCONTACTPAGE_H

#include <addcontactpage.h>

class KLineEdit;

class QQAddContactPage : public AddContactPage
{
    Q_OBJECT
    public:
        explicit QQAddContactPage( QWidget* parent );
        virtual ~QQAddContactPage();
        virtual bool apply( Kopete::Account* account, Kopete::MetaContact* metaContact );
        virtual bool validateData();
    private:
        KLineEdit* m_contactEdit;
};

#endif // QQADDCONTACTPAGE_H
