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

CachedMoves::CachedMoves() :
    boards_(1) {
}

CachedMoves::CachedMoves(const Moves& moves) :
    Moves(moves), boards_(moves.size() + 1) {
    boards_rebuild_();
}

void CachedMoves::boards_rebuild_() {
    for (Moves::const_iterator i = Moves::begin(); i <= Moves::end(); ++i) {
        boards_[i.n()] = i.board();
    }
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

HalfMove CachedMoves::const_iterator::operator*() const {
    return static_cast<const CachedMoves*>(moves_)->half_move(n_);
}

const Board& CachedMoves::const_iterator::board() const {
    return static_cast<const CachedMoves*>(moves_)->board(n_);
}

CachedMoves::const_iterator::const_iterator(const CachedMoves& moves, int from):
    base_iterator(moves, from)
{ }

CachedMoves::const_iterator CachedMoves::begin() const {
    return iter(0);
}

CachedMoves::const_iterator CachedMoves::end() const {
    return iter(size());
}

CachedMoves::const_iterator CachedMoves::back() const {
    return iter(size() - 1);
}

CachedMoves::const_iterator CachedMoves::iter(int from) const {
    return const_iterator(*this, from);
}

}

