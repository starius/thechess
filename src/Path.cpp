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

}

