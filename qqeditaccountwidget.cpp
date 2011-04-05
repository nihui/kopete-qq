#include "qqeditaccountwidget.h"

#include "qqaccount.h"
#include "qqprotocol.h"

#include "ui_qqeditaccount.h"

#include <kopeteuiglobal.h>

#include <KLocale>
#include <KMessageBox>
#include <KToolInvocation>

QQEditAccountWidget::QQEditAccountWidget( QWidget* parent, Kopete::Account* account )
: QWidget(parent),
KopeteEditAccountWidget(account)
{
    m_widget = new Ui::QQEditAccount;
    m_widget->setupUi( this );

    if ( account ) {
        m_widget->m_login->setText( account->accountId() );
        m_widget->m_login->setReadOnly( true );
        m_widget->m_password->load( &static_cast<QQAccount*>(account)->password() );
        m_widget->m_autologin->setChecked( account->excludeConnect() );
    }

    connect( m_widget->buttonRegister, SIGNAL(clicked()), this, SLOT(slotOpenRegister()) );
}

QQEditAccountWidget::~QQEditAccountWidget()
{
    delete m_widget;
}

Kopete::Account* QQEditAccountWidget::apply()
{
    if ( !account() )
        setAccount( new QQAccount( QQProtocol::protocol(), m_widget->m_login->text().trimmed() ) );

    account()->setExcludeConnect( m_widget->m_autologin->isChecked() );
    m_widget->m_password->save( &static_cast<QQAccount*>(account())->password() );

    /// TODO: save account information into config file

    return account();
}

bool QQEditAccountWidget::validateData()
{
    if ( QQProtocol::validContactId( m_widget->m_login->text().trimmed() ) )
        return true;

    KMessageBox::queuedMessageBox( Kopete::UI::Global::mainWidget(), KMessageBox::Sorry,
                                   i18n( "<qt>You must enter a valid QQ account ID.</qt>" ), i18n( "QQ Plugin" ) );
    return false;
}

void QQEditAccountWidget::slotOpenRegister()
{
    KToolInvocation::invokeBrowser( "http://freereg.qq.com/" );
}
