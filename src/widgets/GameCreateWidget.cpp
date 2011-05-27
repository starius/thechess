
#include <Wt/WText>
#include <Wt/WComboBox>
#include <Wt/WLabel>
#include <Wt/WBreak>
#include <Wt/WPushButton>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;

#include "widgets/GameCreateWidget.hpp"
#include "widgets/PleaseLoginWidget.hpp"
#include "widgets/GameParametersWidget.hpp"
#include "model/Game.hpp"
#include "ThechessApplication.hpp"

namespace thechess {
namespace widgets {

using namespace model;

GameCreateWidget::GameCreateWidget(UserPtr user,
    Wt::WContainerWidget* p) :
Wt::WContainerWidget(p), with_user_(true), user_(user)
{
    dbo::Transaction t(tApp->session());
    new Wt::WText(tr("thechess.format.competitor")
        .arg(user->username()), this);
    if (!tApp->user())
    {
        new PleaseLoginWidget(this);
    }
    else
    {
        print_();
    }
    t.commit();
}

GameCreateWidget::GameCreateWidget(Wt::WContainerWidget* p) :
Wt::WContainerWidget(p), with_user_(false)
{
    new Wt::WText(tr("thechess.challenge"), this);
    if (!tApp->user())
    {
        new PleaseLoginWidget(this);
    }
    else
    {
        print_();
    }
}

void GameCreateWidget::print_()
{
    GameParameters* gp = GameParameters::create_new();
    gpw_ = new GameParametersWidget(gp, this);
    delete gp;

    Wt::WLabel* color_label =
        new Wt::WLabel(tr("thechess.your_color"), this);
    color_ = new Wt::WComboBox(this);
    color_->addItem(tr("thechess.random"));
    color_->addItem(tr("thechess.white"));
    color_->addItem(tr("thechess.black"));
    color_label->setBuddy(color_);
    new Wt::WBreak(this);
    Wt::WPushButton* ok_ = new Wt::WPushButton(tr("thechess.create"), this);
    ok_->clicked().connect(this, &GameCreateWidget::button_handler_);
}

void GameCreateWidget::button_handler_()
{
    dbo::Transaction t(tApp->session());
    GamePtr game = tApp->session().add(Game::create_new());
    gpw_->apply_parameters(game.modify());
    chess::Color color = selected_color_();
    if (with_user_)
    {
        game.modify()->propose_game(tApp->user(), user_, color);
    }
    else
    {
        game.modify()->propose_challenge(tApp->user(), color);
    }
    t.commit();
    tApp->view(game);
}

chess::Color GameCreateWidget::selected_color_() const
{
    chess::Color color = chess::color_null;
    if (color_->currentIndex() == 1)
    {
        color = chess::white;
    }
    if (color_->currentIndex() == 2)
    {
        color = chess::black;
    }
    return color;
}


}
}
