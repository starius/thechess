/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include "chess/HalfMove.hpp"
#include "utils.hpp"

namespace thechess {

HalfMove::HalfMove(Square from, Square to) :
    from_(from), to_(to), turn_into_(LETTER_NULL) {
}

// ?? no tests of valideness of letter
HalfMove::HalfMove(Square from, Square packed_to, const Board& board) :
    from_(from), to_(packed_to), turn_into_(LETTER_NULL) {
    if (board.letter(this->from()) == PAWN) {
        if (board.order() == WHITE && this->from().rank() == RANK_7) {
            to_ = Square(packed_to.file(), RANK_8);
            turn_into_ = (Letter)packed_to.rank();
        }
        if (board.order() == BLACK && this->from().rank() == RANK_2) {
            to_ = Square(packed_to.file(), RANK_1);
            turn_into_ = (Letter)packed_to.rank();
        }
    }
}

HalfMove::HalfMove() :
    from_(SQUARE_NULL), to_(SQUARE_NULL), turn_into_(LETTER_NULL) {
}

Square HalfMove::packed_to() const {
    return turn_into_ == LETTER_NULL ? to() : Square(to().file(), (Rank)turn_into_);
}

bool HalfMove::could_turn_into(const Board& board) const {
    return board.letter(from()) == PAWN && (to().rank() == RANK_1 || to().rank() == RANK_8);
}

std::string HalfMove::san_from_(const Board& board) const {
    Letter letter = board.letter(from());
    if (letter == PAWN && board.test_takes(*this)) {
        return char2str(file_char(from().file()));
    }
    bool alt = false, alt_file = false, alt_rank = false;
    THECHESS_SQUARE_FOREACH (square) {
        HalfMove alt_move(square, to(), board);
        if (board.letter(square) == letter && square != from() &&
                board.test_move(alt_move) && !board.test_castling(alt_move)) {
            alt = true;
            if (square.file() == from().file()) {
                alt_file = true;
            }
            if (square.rank() == from().rank()) {
                alt_rank = true;
            }
        }
    }
    if (alt_file && alt_rank) {
        return from().str();
    } else if (alt_file) {
        return char2str(rank_char(from().rank()));
    } else if (alt) {
        return char2str(file_char(from().file()));
    }
    return "";
}

std::string HalfMove::san(const Board& board, const Board& board_after,
                          bool skip_pieces) const {
    std::string result;
    result.reserve(10);
    if (board.test_castling(*this)) {
        result += to().file() == FILE_G ? "O-O" : "O-O-O";
    } else {
        Letter letter = board.letter(from());
        if (!skip_pieces && letter != PAWN) {
            result += piece_char(letter);
        }
        result += san_from_(board);
        if (board.test_takes(*this)) {
            result += 'x';
        }
        result += to().str();
        if (!skip_pieces) {
            if (turn_into() != LETTER_NULL) {
                (result += '=') += piece_char(turn_into());
            }
            if (board_after.test_shah()) {
                result += board_after.test_end() == CHECKMATE ? '#' : '+';
            }
        }
    }
    return result;
}

}

