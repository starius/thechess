/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMECOUNTDOWN_HPP_
#define THECHESS_WIDGETS_GAMECOUNTDOWN_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

/** A widget displaying game countdowns */
class GameCountdown : public Wt::WContainerWidget {
public:
    /** Constructor */
    GameCountdown(const GamePtr& game, Wt::WContainerWidget* parent = 0);

    /** Update (or add) game countdowns.
    Be sure to reread game before calling this.
    */
    void reread();

    /** Get the game */
    const GamePtr& game() const {
        return game_;
    }

private:
    class SingleTimeout;

    GamePtr game_;
    SingleTimeout* timeout1_;
    SingleTimeout* timeout2_;

    void single_countdown(SingleTimeout*& timeout, const UserPtr& user);
};

}

#endif

