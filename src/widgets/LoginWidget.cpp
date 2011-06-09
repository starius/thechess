
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WLineEdit>
#include <Wt/WLabel>
#include <Wt/WTable>
#include <Wt/WRegExpValidator>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

#include "ThechessApplication.hpp"
#include "widgets/LoginWidget.hpp"
#include "config.hpp"
#include "model/User.hpp"

namespace thechess {
using namespace model;
namespace widgets {

LoginWidget::LoginWidget(Wt::WContainerWidget* parent) :
Wt::WContainerWidget(parent)
{
    table_ = new Wt::WTable(this);
    Wt::WLabel* username_label = new Wt::WLabel(tr("thechess.username"),
        table_->elementAt(0,0));
    username_ = new Wt::WLineEdit(table_->elementAt(0, 1));
    Wt::WLabel* password_label = new Wt::WLabel(tr("thechess.password"),
        table_->elementAt(1,0));
    password_ = new Wt::WLineEdit(table_->elementAt(1, 1));
    password_->setEchoMode(Wt::WLineEdit::Password);

    username_label->setBuddy(username_);
    password_label->setBuddy(password_);

    error_ = new Wt::WText(this);

    button_ = new Wt::WPushButton(tr("thechess.login"), this);
    button_->clicked().connect(this, &LoginWidget::try_);
}

void LoginWidget::try_()
{
    error_->setText(tr(""));
    dbo::Transaction t(tApp->session());
    UserPtr user = tApp->session().find<User>()
    .where("username = ?")
    .bind(username_->text());
    if (!user || !user->test_password(password_->text().toUTF8()))
    {
        error_->setText(tr("thechess.wrong_username_or_password"));
    }
    else
    {
        tApp->set_user(user);
    }

    t.commit();
}

}
}
