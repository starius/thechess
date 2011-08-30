/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include "chess/CachedMoves.hpp"

namespace thechess {

CachedMoves::CachedMoves(int size) :
    Moves(size), boards_(size + 1) {
}

CachedMoves::CachedMoves(const Moves& moves) :
    Moves(moves), boards_(moves.size() + 1) {
    boards_rebuild_();
}

void CachedMoves::boards_rebuild_() {
    Board board;
    int i = 0;
    THECHESS_MOVES_FOREACH (move_it, this, board) {
        boards_[i] = board;
        i++;
    }
    boards_[size()] = board;
}

void CachedMoves::push(HalfMove half_move) {
    Moves::push(half_move);
    Board board = boards_[boards_.size() - 1];
    board.make_move(half_move);
    boards_.push_back(board);
}

void CachedMoves::pop() {
    Moves::pop();
    boards_.pop_back();
}

HalfMove CachedMoves::half_move(int n) const {
    return Moves::half_move(n, boards_[n]);
}

const Board& CachedMoves::board(int n) const {
    return boards_[n];
}

}

