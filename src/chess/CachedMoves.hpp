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

    /** Const iterator */
    class const_iterator : public Moves::base_iterator {
    public:
        /** Indirection, get current half-move */
        HalfMove operator*() const;

        /** Get board before current half-move */
        const Board& board() const;

        friend class CachedMoves;

    private:
        const_iterator(const CachedMoves& moves, int from = 0);
    };

    /** Return iterator referring to the first half-move */
    const_iterator begin() const;

    /** Return iterator referring to the past-the-end half-move */
    const_iterator end() const;

    /** Return iterator referring to the last half-move */
    const_iterator back() const;

    /** Return iterator referring to the half-move */
    const_iterator iter(int from = 0) const;

private:
    /** Stores boards before current half-move */
    std::vector<Board> boards_;

    void boards_rebuild_();
};

}

#endif

