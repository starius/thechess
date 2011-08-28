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

CachedMoves::CachedMoves(int moves_count) :
    Moves(moves_count), boards_(moves_count + 1) {
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

void CachedMoves::push_move(HalfMove half_move) {
    Moves::push_move(half_move);
    Board board = boards_[boards_.size() - 1];
    board.make_move(half_move);
    boards_.push_back(board);
}

void CachedMoves::pop_move() {
    Moves::pop_move();
    boards_.pop_back();
}

void CachedMoves::pop_moves(int number) {
    Moves::pop_moves(number);
    boards_.erase(boards_.end() - number, boards_.end());
}

void CachedMoves::reset_move(int n, HalfMove half_move) {
    pop_moves(size() - n);
    push_move(half_move);
}

HalfMove CachedMoves::move_at(int n) const {
    return Moves::move_at(n, boards_[n]);
}

const Board& CachedMoves::board_at(int n) const {
    return boards_[n];
}

void CachedMoves::foreach(void* func(HalfMove half_move, const Board& board),
                          int from, int to) const {
    if (to == -1) {
        to = size();
    }
    for (int i = from; i < to; i++) {
        func(move_at(i), board_at(i));
    }
}

}

