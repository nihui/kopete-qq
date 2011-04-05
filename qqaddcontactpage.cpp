#include "qqaddcontactpage.h"

#include "qqprotocol.h"

#include <KLineEdit>
#include <KLocale>
#include <QFormLayout>

QQAddContactPage::QQAddContactPage( QWidget* parent )
: AddContactPage(parent)
{
    m_contactEdit = new KLineEdit;

    QFormLayout* layout = new QFormLayout;
    setLayout( layout );

    layout->addRow( i18n( "Contact:" ), m_contactEdit );
}

QQAddContactPage::~QQAddContactPage()
{
}

bool QQAddContactPage::apply( Kopete::Account* account, Kopete::MetaContact* metaContact )
{
    /// TODO
    return false;
}

bool QQAddContactPage::validateData()
{
    if ( QQProtocol::validContactId( m_contactEdit->text() ) )
        return true;
    return false;
}
