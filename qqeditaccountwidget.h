#ifndef QQEDITACCOUNTWIDGET_H
#define QQEDITACCOUNTWIDGET_H

#include <editaccountwidget.h>
#include <QWidget>

namespace Ui { class QQEditAccount; };
class QQEditAccountWidget : public QWidget, public KopeteEditAccountWidget
{
    Q_OBJECT
    public:
        explicit QQEditAccountWidget( QWidget* parent, Kopete::Account* account );
        virtual ~QQEditAccountWidget();
        virtual Kopete::Account* apply();
        virtual bool validateData();
    private Q_SLOTS:
        void slotOpenRegister();
    private:
        Ui::QQEditAccount* m_widget;
};

#endif // QQEDITACCOUNTWIDGET_H
