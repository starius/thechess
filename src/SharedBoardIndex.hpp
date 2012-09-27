/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_SHARED_BOARD_INDEX_HPP_
#define THECHESS_SHARED_BOARD_INDEX_HPP_

#include <boost/thread/shared_mutex.hpp>

#include "chess/BoardIndex.hpp"
#include "thechess-global.hpp"

namespace thechess {

/** Shared index of all the games.
Many concurrent calls to methods rebuild(), search_board() and search_moves()
are allowed.

\ingroup server
*/
class SharedBoardIndex {
public:
    /** Rebuild BoardIndex.
    Does not require active transaction.
    */
    void rebuild();

    /** Find all games including this board state.
    Requires tApp (for session and transaction).
    If called while rebuild(), empty result is returned.
    */
    void search_board(Session& session, const Board& board,
                      std::vector<int>& games);

    /** Find games starting from this moves.
    \param max_boards Max number of board states (or half-moves) searched.
    \param stop_games Stop filtering candidate games if its size <= stop_games.
    May not produce false positives.
    If called while rebuild(), empty result is returned.
    */
    void search_moves(Session& session, const Moves& moves,
                      std::vector<int>& games,
                      int max_boards = 20, int stop_games = 5);

    /** Return global instance of SharedBoardIndex */
    static SharedBoardIndex* instance();

private:
    BoardIndex index_;
    boost::shared_mutex mutex_;
};

}

#endif

