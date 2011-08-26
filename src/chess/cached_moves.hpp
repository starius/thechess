/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_CACHED_MOVES_H_
#define THECHESS_CACHED_MOVES_H_

#include "chess/moves.hpp"
#include "chess/board.hpp"
#include <vector>

namespace thechess {
namespace chess {
class CachedMoves;
}
}

namespace thechess {
namespace chess {

/** Board-caching version of Moves */
class CachedMoves : public Moves {
public:
    /** Constructor.
    \sa Moves::Moves(int moves_count)
    */
    CachedMoves(int moves_count = 0);

    /** Construct from Moves */
    CachedMoves(const Moves& moves);

    /** Push the move to the end of container */
    void push_move(Move move);

    /** Pop last move */
    void pop_move();

    /** Pop several moves from the ending */
    void pop_moves(int number);

    /** Strip replace half-move and all its descendants.
    This method pops several moves (see pop_moves()) and
    pushes the move (see push_move()).
    */
    void reset_move(int n, Move move);

    /** Return the half-move */
    Move move_at(int n) const;

    /** Return board position before the half-move */
    const Board& board_at(int n) const;

    /** Apply func to range of half-moves.
    \param func function to apply
    \param from index of half-move to begin iteration
    \param to index of half-move to stop before.
        If to==-1, iterates from \c from to the end of the container.
    */
    void foreach(void* func(Move move, const Board& board),
                 int from = 0, int to = -1) const;

private:
    std::vector<Board> boards_; // stores boards before this move

    void boards_rebuild_();
    void set_move(int n, Move move); // to forbid this
};

}
}

#endif // THECHESS_CACHED_MOVES_H_
