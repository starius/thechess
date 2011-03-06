#include <boost/bind.hpp>
#include <iostream>

#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WText>

#include "widgets/UserWidget.hpp"
#include "widgets/RatingChanges.hpp"
#include "widgets/GameCreateWidget.hpp"
#include "ThechessApplication.hpp"
#include "model/Game.hpp"


namespace thechess {
namespace widgets {

using model::UserPtr;
using model::GamePtr;

class UserWidgetImpl : public Wt::WContainerWidget
{
public:
    UserWidgetImpl(UserPtr user) :
    Wt::WContainerWidget(), user_(user)
    {
        dbo::Transaction t(tApp->session());
        new Wt::WText(user_->username(), this);
        new Wt::WBreak(this);
        if (tApp->user() && tApp->user() != user_)
        {
            start_button_ = new Wt::WPushButton(
                Wt::WString::tr("thechess.start_game"), this);
            start_button_->clicked()
                .connect(this, &UserWidgetImpl::game_form_);
        }
        if (user->number_of_games() > 0)
        {
            rating_button_ = new Wt::WPushButton(
                Wt::WString::tr("thechess.rating_changes"), this);
            rating_button_->clicked()
                .connect(this, &UserWidgetImpl::rating_changes_);
            if (tApp->user() && tApp->user() != user_)
            {
                rating_and_me_button_ = new Wt::WPushButton(
                    Wt::WString::tr("thechess.rating_changes_and_me"), this);
                rating_and_me_button_->clicked()
                    .connect(this, &UserWidgetImpl::rating_changes_and_me_);
            }
        }
        t.commit();
    }

private:
    UserPtr user_;
    Wt::WPushButton* start_button_;
    Wt::WPushButton* rating_button_;
    Wt::WPushButton* rating_and_me_button_;

    void game_form_()
    {
        start_button_->hide();
        new GameCreateWidget(user_, this);
    }

    void rating_changes_()
    {
        rating_button_->hide();
        rating_and_me_button_->hide();
        new RatingChanges(user_, this);
    }

    void rating_changes_and_me_()
    {
        rating_button_->hide();
        rating_and_me_button_->hide();
        RatingChanges* rc = new RatingChanges(user_, this);
        rc->add_user(tApp->user());
    }
};


UserWidget::UserWidget(UserPtr user, Wt::WContainerWidget* parent) :
WCompositeWidget(parent)
{
    impl_ = new UserWidgetImpl(user);
    setImplementation(impl_);
}

}
}
