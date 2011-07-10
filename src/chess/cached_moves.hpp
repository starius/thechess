/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

//
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

class CachedMoves : public Moves
{
public:
    CachedMoves(int moves_count=0);
    //CachedMoves(std::string moves);
    CachedMoves(const Moves& moves);

    void push_move(Move move);
    void pop_move();
    void pop_moves(int number);
    void reset_move(int n, Move move);

    Move move_at(int n) const;
    const Board& board_at(int n) const;

    void foreach(void *func(Move move, const Board& board),
        int from=0, int to=-1) const;

    std::vector<Board> boards_; // stores boards before this move
private:

    void boards_rebuild_();
    void set_move(int n, Move move); // to forbid this
};

}
}


#endif // THECHESS_CACHED_MOVES_H_
