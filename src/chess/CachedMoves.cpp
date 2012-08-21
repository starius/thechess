/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/Wc/util.hpp>

#include "chess/CachedMoves.hpp"
#include "chess/Piece.hpp"

namespace thechess {

CachedMoves::CachedMoves() :
    boards_(1) {
}

CachedMoves::CachedMoves(const Moves& moves) :
    Moves(moves), boards_(moves.size() + 1) {
    boards_rebuild();
}

void CachedMoves::boards_rebuild() {
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

bool CachedMoves::is_draw_3() const {
    typedef std::map<Board, int> B2C;
    B2C board2count;
    for (const_iterator i = begin(); i != end(); ++i) {
        const Board& board = i.board();
        board2count[board] += 1;
    }
    for (B2C::iterator i = board2count.begin(); i != board2count.end(); ++i) {
        int count = i->second;
        if (count >= 3) {
            return true;
        }
    }
    return false;
}

bool CachedMoves::is_draw_50() const {
    if (moves_number() < 50) {
        return false;
    }
    for (const_iterator i = iter(size() - 50 * 2); i != end(); ++i) {
        const HalfMove& half_move = *i;
        const Board& board = i.board();
        if (board.letter(half_move.from()) == Piece::PAWN) {
            return false;
        }
        if (board.test_takes(half_move)) {
            return false;
        }
    }
    return true;
}

CachedMoves::const_iterator& CachedMoves::const_iterator::operator --() {
    --n_;
    return *this;
}

HalfMove CachedMoves::const_iterator::operator*() const {
    return downcast<const CachedMoves*>(moves_)->half_move(n_);
}

const Board& CachedMoves::const_iterator::board() const {
    return downcast<const CachedMoves*>(moves_)->board(n_);
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

