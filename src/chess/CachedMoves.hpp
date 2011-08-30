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

#include "chess/Moves.hpp"
#include "chess/Board.hpp"
#include <vector>

namespace thechess {
class CachedMoves;
}

namespace thechess {

/** Board-caching version of Moves.
\ingroup chess
*/
class CachedMoves : public Moves {
public:
    /** Default constructor.
    Empty container
    */
    CachedMoves();

    /** Construct from Moves */
    CachedMoves(const Moves& moves);

    /** Push the half-move to the end of container */
    void push(HalfMove half_move);

    /** Pop last half-move */
    void pop();

    /** Return the half-move */
    HalfMove half_move(int n) const;

    /** Return board position before the half-move */
    const Board& board(int n) const;

private:
    /** Stores boards before current half-move */
    std::vector<Board> boards_;

    void boards_rebuild_();
};

}

#endif

