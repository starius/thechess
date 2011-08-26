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

#include "chess/moves.hpp"
#include "chess/move.hpp"
#include "chess/xy.hpp"
#include "utils.hpp"

namespace thechess {

MovesIterator::MovesIterator(const Moves* moves, Board& board, int from)
    :  moves_(moves), board_(board), n(from) {
}

Moves::Moves(int moves_count) :
    svuc(moves_count, '\0') {
}

Moves::Moves(Move moves[], int moves_count) :
    svuc(moves_count, '\0') {
    for (int i = 0; i < moves_count; i++) {
        set_move(i, moves[i]);
    }
}

Xy Moves::xy_(int i) const {
    /*
     0000 0000  0000 0000  0000 0000
     0      1        2       3
    */
    int first = (i * 6) / 8;
    int mod_i = i % 4;
    if (mod_i == 0) {
        return Xy((int)(q(first) >> 2));
    } else if (mod_i == 1) {
        // 0011 1111
        return Xy((int)(((q(first) << 4) & 0x3F) | (q(first + 1) >> 4)));
    } else if (mod_i == 2) {
        // 0011 1111
        return Xy((int)(((q(first) << 2) & 0x3F) | (q(first + 1) >> 6)));
    } else if (mod_i == 3) {
        return Xy((int)(q(first) & 0x3F)); // 0011 1111
    }
    return Xy(0); // newer happened
}

void Moves::xy_(int i, Xy xy) {
    int first = (i * 6) / 8;
    int mod_i = i % 4;
    if (mod_i == 0) {
        (*this)[first] &= 0x03; // 0000 0011
        (*this)[first] |= (byte)xy.i() << 2;
    } else if (mod_i == 1) {
        (*this)[first] &= 0xFC; // 1111 1100
        (*this)[first] |= (byte)xy.i() >> 4;
        (*this)[first + 1] &= 0x0F; // 0000 1111
        (*this)[first + 1] |= (byte)xy.i() << 4;
    } else if (mod_i == 2) {
        (*this)[first] &= 0xF0; // 1111 0000
        (*this)[first] |= (byte)xy.i() >> 2;
        (*this)[first + 1] &= 0x3F; //  0011 1111
        (*this)[first + 1] |= (byte)xy.i() << 6;
    } else if (mod_i == 3) {
        (*this)[first] &= 0xC0; // 1100 0000
        (*this)[first] |= (byte)xy.i();
    }
}

void Moves::set_move(int n, Move move) {
    xy_(n, xy_from, move.from());
    xy_(n, xy_to, move.packed_to());
}

void Moves::push_move(Move move) {
    int current_size = size();
    if (current_size % 2 == 0) {
        // adding move by white
        push_back('\0');
        push_back('\0');
    } else {
        // adding move by black
        push_back('\0');
    }
    set_move(current_size, move);
}

void Moves::pop_move() {
    int current_size = size();
    if (current_size % 2 == 0) {
        // delete one byte
        pop_back();
    } else {
        // delete two bytes
        pop_back();
        pop_back();
    }
}

void Moves::pop_moves(int number) {
    for (int i = 0; i < number; i++) {
        pop_move();
    }
}

Board Moves::board_at(int n) const {
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

void Moves::pgn(std::ostream& out, const std::string& result, bool reduced) const {
    PlainTextWritter ptw(out);
    Board board;
    Board board_after;
    THECHESS_MOVES_FOREACH (move_it, this, board) {
        Move move = *move_it;
        board_after.make_move(move);
        std::string move_str = move.san(board, board_after);
        if (order(move_it.n) == white) {
            move_str = str(boost::format("%i. ") % n_to_human(move_it.n)) + move_str;
        }
        ptw.write_word(move_str);
    }
    if (result != "*") {
        ptw.write_word(result);
    }
}

}

