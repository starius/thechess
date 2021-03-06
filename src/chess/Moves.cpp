/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/Wc/PlainTextWritter.hpp>

#include "chess/Moves.hpp"
#include "chess/HalfMove.hpp"
#include "chess/Square.hpp"
#include "chess/base64.hpp"

namespace thechess {

Moves::Moves()
{ }

Moves::Moves(const Moves& other, int size) {
    if (size == -1) {
        size = other.size();
    }
    int bytes_size = (size * 3 + 1) / 2;
    svuc_.assign(other.svuc_.begin(), other.svuc_.begin() + bytes_size);
    if (svuc_.size() % 3 == 2) {
        // only first half-move of move done
        // last 4 bits of last byte are random
        svuc_.back() &= 0xF0;
    }
}

Moves::Moves(HalfMove moves[], int size) :
    svuc_(size, '\0') {
    for (int i = 0; i < size; i++) {
        set_half_move(i, moves[i]);
    }
}

Moves::Moves(const std::string& data, bool check) {
    std::string data1(data);
    boost::algorithm::replace_all(data1, "-", "+");
    boost::algorithm::replace_all(data1, ".", "/");
    data1 += std::string((4 - data1.size() % 4) % 4, '=');
    std::string moves = base64_decode(data1);
    svuc_.assign(moves.begin(), moves.end());
    if (check) {
        if (svuc_.size() % 3 == 2) {
            // only first half-move of move done
            // last 4 bits of last byte are random
            svuc_.back() &= 0xF0;
        }
        check_and_fix();
    }
}

std::string Moves::to_string() const {
    std::string result = base64_encode(&svuc_[0], svuc_.size());
    boost::algorithm::replace_all(result, "+", "-");
    boost::algorithm::replace_all(result, "/", ".");
    boost::algorithm::replace_all(result, "=", "");
    return result;
}

Moves::byte Moves::q(int i) const {
    return svuc_[i];
}

Square Moves::square(int i) const {
    /*
     0000 0000    0000 0000    0000 0000
     0      1          2         3
    */
    int first = (i * 6) / 8;
    int mod_i = i % 4;
    if (mod_i == 0) {
        return Square((int)(q(first) >> 2));
    } else if (mod_i == 1) {
        // 0011 1111
        return Square((int)(((q(first) << 4) & 0x3F) | (q(first + 1) >> 4)));
    } else if (mod_i == 2) {
        // 0011 1111
        return Square((int)(((q(first) << 2) & 0x3F) | (q(first + 1) >> 6)));
    } else { /* if (mod_i == 3) */
        return Square((int)(q(first) & 0x3F)); // 0011 1111
    }
}

void Moves::square(int i, Square square) {
    int first = (i * 6) / 8;
    int mod_i = i % 4;
    if (mod_i == 0) {
        svuc_[first] &= 0x03; // 0000 0011
        svuc_[first] |= (byte)square.i() << 2;
    } else if (mod_i == 1) {
        svuc_[first] &= 0xFC; // 1111 1100
        svuc_[first] |= (byte)square.i() >> 4;
        svuc_[first + 1] &= 0x0F; // 0000 1111
        svuc_[first + 1] |= (byte)square.i() << 4;
    } else if (mod_i == 2) {
        svuc_[first] &= 0xF0; // 1111 0000
        svuc_[first] |= (byte)square.i() >> 2;
        svuc_[first + 1] &= 0x3F; //  0011 1111
        svuc_[first + 1] |= (byte)square.i() << 6;
    } else if (mod_i == 3) {
        svuc_[first] &= 0xC0; // 1100 0000
        svuc_[first] |= (byte)square.i();
    }
}

Square Moves::square(int n, SquareType square_type) const {
    return square(n * 2 + (int)square_type);
}

void Moves::square(int n, Moves::SquareType square_type, Square square) {
    this->square(n * 2 + (int)square_type, square);
}

void Moves::set_half_move(int n, HalfMove half_move) {
    square(n, SQUARE_FROM, half_move.from());
    square(n, SQUARE_TO, half_move.packed_to());
}

int Moves::size() const {
    return (svuc_.size() * 8) / 12;
}

void Moves::push(HalfMove half_move) {
    int current_size = size();
    if (current_size % 2 == 0) {
        // adding half_move by white
        svuc_.push_back('\0');
        svuc_.push_back('\0');
    } else {
        // adding half_move by black
        svuc_.push_back('\0');
    }
    set_half_move(current_size, half_move);
}

void Moves::pop() {
    int current_size = size();
    if (current_size % 2 == 0) {
        // delete one byte
        svuc_.pop_back();
    } else {
        // delete two bytes
        svuc_.pop_back();
        svuc_.pop_back();
    }
}

void Moves::pop(int number) {
    for (int i = 0; i < number; i++) {
        pop();
    }
}

void Moves::reset_half_move(int n, HalfMove half_move) {
    pop(size() - n);
    push(half_move);
}

HalfMove Moves::half_move(int n, const Board& board) const {
    return HalfMove(square(n, SQUARE_FROM), square(n, SQUARE_TO), board);
}

HalfMove Moves::half_move(int n) const {
    return HalfMove(square(n, SQUARE_FROM), square(n, SQUARE_TO));
}

Board Moves::board(int n) const {
    return iter(n).board();
}

int Moves::check() const {
    for (const_iterator i = begin(); i != end(); ++i) {
        const Board& board = i.board();
        HalfMove half_move = *i;
        if (!board.test_move(half_move)) {
            return i.n();
        }
    }
    return -1;
}

bool Moves::check_and_fix() {
    int bad_move = check();
    if (bad_move != -1) {
        pop(size() - bad_move);
    }
    return bad_move != -1;
}

void Moves::fen(std::ostream& out) const {
    int halfmove_clock = 0;
    const_iterator i = begin();
    for (; i < end(); ++i) {
        HalfMove half_move = *i;
        const Board& board = i.board();
        if (board.letter(half_move.from()) == Piece::PAWN ||
                board.test_takes(half_move)) {
            halfmove_clock = 0;
        } else {
            halfmove_clock += 1;
        }
    }
    int fullmove_number = Moves::moves_number(size() + 1);
    const Board& board = i.board();
    board.fen(out, halfmove_clock, fullmove_number);
}

void Moves::pgn(std::ostream& out, const std::string& result,
                bool /*reduced*/) const {
    Wt::Wc::PlainTextWritter ptw(out);
    const_iterator after = begin();
    for (const_iterator i = begin(); i != end(); ++i) {
        const Board& board = i.board();
        HalfMove half_move = *i;
        ++after;
        const Board& board_after = after.board();
        std::string move_str = half_move.san(board, board_after);
        if (order(i.n()) == Piece::WHITE) {
            move_str = str(boost::format("%i. ") % move_number(i.n())) +
                       move_str;
        }
        ptw.write_word(move_str);
    }
    if (result != "*") {
        ptw.write_word(result);
    }
}

void Moves::foreach(void* func(HalfMove half_move, const Board& board),
                    int from, int to) const {
    if (to == -1) {
        to = size();
    }
    for (int i = from; i < to; i++) {
        func(half_move(i), board(i));
    }
}

bool Moves::has_board(const Board& board) const {
    for (Moves::const_iterator it = begin(); it <= end(); ++it) {
        if (it.board() == board) {
            return true;
        }
    }
    return false;
}

bool Moves::starts_with(const Moves& beginning) const {
    if (beginning.size() == 0) {
        return true;
    }
    if (beginning.size() > size()) {
        return false;
    }
    int last_half_move = beginning.size() - 1;
    if (last_half_move % 2 == 1 && // odd half-move
            beginning.Moves::half_move(last_half_move) !=
            Moves::half_move(last_half_move)) {
        return false;
    }
    int count3 = beginning.svuc_.size() / 3;
    int count4 = beginning.svuc_.size() / sizeof(int);
    for (int i = count4 * sizeof(int); i < count3 * 3; i++) {
        if (beginning.svuc_[i] != svuc_[i]) {
            return false;
        }
    }
    const int* beginning_r = reinterpret_cast<const int*>(&beginning.svuc_[0]);
    const int* r = reinterpret_cast<const int*>(&svuc_[0]);
    for (int i = count4 - 1; i >= 0; i--) {
        if (beginning_r[i] != r[i]) {
            return false;
        }
    }
    return true;
}

int Moves::move_number(int move_n) {
    return (move_n + 2) / 2;
}

int Moves::moves_number(int size) {
    return move_number(size - 1);
}

int Moves::half_move_index(int moves_number, Piece::Color color) {
    return (moves_number - 1) * 2 + ((color == Piece::WHITE) ? 0 : 1);
}

int Moves::moves_number() const {
    return moves_number(size());
}

int Moves::order_int(int move_n) {
    return move_n % 2;
}

Piece::Color Moves::order(int move_n) {
    return order_int(move_n) == 0 ? Piece::WHITE : Piece::BLACK;
}

bool Moves::base_iterator::operator ==(const base_iterator& other) const {
    return n_ == other.n_;
}

bool Moves::base_iterator::operator !=(const base_iterator& other) const {
    return !(*this == other);
}

bool Moves::base_iterator::operator <(const base_iterator& other) const {
    return n_ < other.n_;
}

bool Moves::base_iterator::operator <=(const base_iterator& other) const {
    return n_ <= other.n_;
}

Moves::base_iterator& Moves::base_iterator::operator ++() {
    ++n_;
    return *this;
}

Moves::base_iterator::base_iterator(const Moves& moves, int from):
    moves_(&moves), n_(from)
{ }

HalfMove Moves::const_iterator::operator*() const {
    sync_board();
    return half_move();
}

const Board& Moves::const_iterator::board() const {
    sync_board();
    return board_;
}

Moves::const_iterator::const_iterator(const Moves& moves, int from):
    base_iterator(moves, from), board_n_(0)
{ }

void Moves::const_iterator::sync_board() const {
    for (; board_n_ < n_; board_n_++) {
        board_.make_move(half_move());
    }
}

HalfMove Moves::const_iterator::half_move() const {
    return moves_->half_move(board_n_, board_);
}

Moves::const_iterator Moves::begin() const {
    return iter(0);
}

Moves::const_iterator Moves::end() const {
    return iter(size());
}

Moves::const_iterator Moves::back() const {
    return iter(size() - 1);
}

Moves::const_iterator Moves::iter(int from) const {
    return const_iterator(*this, from);
}

}

