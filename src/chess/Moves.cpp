/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <boost/format.hpp>

#include "chess/Moves.hpp"
#include "chess/HalfMove.hpp"
#include "chess/Square.hpp"
#include "utils.hpp"

namespace thechess {

MovesIterator::MovesIterator(const Moves* moves, Board& board, int from)
    :  moves_(moves), board_(board), n(from) {
}

Moves::Moves()
{ }

Moves::Moves(HalfMove moves[], int size) :
    svuc_(size, '\0') {
    for (int i = 0; i < size; i++) {
        set_half_move_(i, moves[i]);
    }
}

Moves::byte Moves::q(int i) const {
    return svuc_[i];
}

Square Moves::square_(int i) const {
    /*
     0000 0000  0000 0000  0000 0000
     0      1        2       3
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
    } else if (mod_i == 3) {
        return Square((int)(q(first) & 0x3F)); // 0011 1111
    }
    return Square(0); // newer happened
}

void Moves::square_(int i, Square square) {
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

Square Moves::square_(int n, SquareType square_type) const {
    return square_(n * 2 + (int)square_type);
}

void Moves::square_(int n, Moves::SquareType square_type, Square square) {
    square_(n * 2 + (int)square_type, square);
}

void Moves::set_half_move_(int n, HalfMove half_move) {
    square_(n, SQUARE_FROM, half_move.from());
    square_(n, SQUARE_TO, half_move.packed_to());
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
    set_half_move_(current_size, half_move);
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
    return HalfMove(square_(n, SQUARE_FROM), square_(n, SQUARE_TO), board);
}

HalfMove Moves::half_move(int n) const {
    return HalfMove(square_(n, SQUARE_FROM), square_(n, SQUARE_TO));
}

Board Moves::board(int n) const {
    Board board;
    THECHESS_MOVES_TO (move_it, this, board, n) {
    }
    return board;
}

int Moves::check() const {
    Board board;
    THECHESS_MOVES_FOREACH (move_it, this, board) {
        if (!board.test_move(*move_it)) {
            return move_it.n;
        }
    }
    return -1;
}

void Moves::pgn(std::ostream& out, const std::string& result, bool /*reduced*/) const {
    PlainTextWritter ptw(out);
    Board board;
    Board board_after;
    THECHESS_MOVES_FOREACH (move_it, this, board) {
        HalfMove half_move = *move_it;
        board_after.make_move(half_move);
        std::string move_str = half_move.san(board, board_after);
        if (order(move_it.n) == Piece::WHITE) {
            move_str = str(boost::format("%i. ") % n_to_human(move_it.n)) + move_str;
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

int Moves::n_to_human(int move_n) {
    return (move_n + 2) / 2;
}

int Moves::size_to_human(int size) {
    return n_to_human(size - 1);
}

int Moves::n_from_human(int human_i, Piece::Color color) {
    return (human_i - 1) * 2 + ((color == Piece::WHITE) ? 0 : 1);
}

int Moves::human_size() const {
    return size_to_human(size());
}

int Moves::order_int(int move_n) {
    return move_n % 2;
}

Piece::Color Moves::order(int move_n) {
    return order_int(move_n) == 0 ? Piece::WHITE : Piece::BLACK;
}

}

