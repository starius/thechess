/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMECREATEWIDGET_HPP_
#define THECHESS_WIDGETS_GAMECREATEWIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/Dbo/ptr>

#include "model/all.hpp"
#include "widgets/global.hpp"
#include "chess/Piece.hpp"

namespace thechess {

class GameCreateWidget : public Wt::WContainerWidget {
public:
    GameCreateWidget(const UserPtr& user, Wt::WContainerWidget* p = 0);

    GameCreateWidget(const GPPtr& gp, Wt::WContainerWidget* p = 0);

    GameCreateWidget(Wt::WContainerWidget* p = 0);

private:
    GPSelector* selector_;
    bool with_user_;
    UserPtr user_;
    GPPtr gp_;
    Wt::WComboBox* color_;
    Wt::WText* error_message_;

    void print();
    void button_handler();
    Piece::Color selected_color() const;
};

}

#endif

