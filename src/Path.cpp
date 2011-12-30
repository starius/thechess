/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/assert.hpp>

#include "Path.hpp"
#include "Application.hpp"
#include "model/all.hpp"
#include "widgets/MainWidget.hpp"

namespace thechess {

using namespace Wt::Wc::url;

Path::Path(Wt::WObject* parent):
    Parser(parent), main_widget_(0) {
    login_form_ = new PredefinedNode("login", this);
    register_form_ = new PredefinedNode("register", this);
    user_list_ = new PredefinedNode("user", this);
    user_view_ = new IntegerNode(user_list_);
    game_list_ = new PredefinedNode("game", this);
    game_view_ = new IntegerNode(game_list_);
    game_new_ = new PredefinedNode("new", game_list_);
    competition_list_ = new PredefinedNode("competition", this);
    competition_view_ = new IntegerNode(competition_list_);
    competition_new_ = new PredefinedNode("new", competition_list_);
    tApp->internalPathChanged().connect(this, &Parser::open);
}

void Path::connect_main_widget(MainWidget* mw) {
    main_widget_ = mw;
    // TODO user list
    login_form_->opened().connect(mw, &MainWidget::login_form);
    register_form_->opened().connect(mw, &MainWidget::register_form);
    user_view_->opened().connect(this, &Path::open_user);
    game_list_->opened().connect(mw, &MainWidget::game_list);
    game_view_->opened().connect(this, &Path::open_game);
    game_new_->opened().connect(mw, &MainWidget::game_new);
    competition_list_->opened().connect(mw, &MainWidget::competition_list);
    competition_view_->opened().connect(this, &Path::open_competition);
    competition_new_->opened().connect(mw, &MainWidget::competition_new);
}

void Path::open_user() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->user_view(tApp->session().load<User>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_game() {
    BOOST_ASSERT(main_widget_);
    long long id = game_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->game_view(tApp->session().load<Game>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_competition() {
    BOOST_ASSERT(main_widget_);
    long long id = competition_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->competition_view(tApp->session().load<Competition>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

}

