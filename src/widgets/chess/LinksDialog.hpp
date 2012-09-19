/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_LINKS_DIALOG_HPP_
#define THECHESS_WIDGETS_LINKS_DIALOG_HPP_

#include <Wt/WDialog>
#include <Wt/Wc/global.hpp>

#include "chess/Moves.hpp"
#include "chess/Board.hpp"

namespace thechess {

/** Dialog with links */
class LinksDialog : public Wt::WDialog {
public:
    /** Constructor */
    LinksDialog();

    /** Add links to board */
    void add_board(const Board& board);

    /** Add links to moves */
    void add_moves(const Moves& moves);

    /** Add link to game/move */
    void add_game_move(int game, int move_n);

    /** Add links to game */
    void add_game(int game);

private:
    Wt::Wc::TableForm* f_;

    void add_url(const std::string& url, const Wt::WString& name);
};

}

#endif

