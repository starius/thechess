/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "chess/HalfMove.hpp"
#include "utils/utils.hpp"

namespace thechess {

HalfMove::HalfMove(Square from, Square to) :
    from_(from), to_(to), turn_into_(Piece::LETTER_NULL) {
}

// ?? no tests of valideness of letter
HalfMove::HalfMove(Square from, Square packed_to, const Board& board) :
    from_(from), to_(packed_to), turn_into_(Piece::LETTER_NULL) {
    if (board.letter(this->from()) == Piece::PAWN) {
        if (board.order() == Piece::WHITE && this->from().rank() == Square::RANK_7) {
            to_ = Square(packed_to.file(), Square::RANK_8);
            turn_into_ = (Piece::Letter)packed_to.rank();
        }
        if (board.order() == Piece::BLACK && this->from().rank() == Square::RANK_2) {
            to_ = Square(packed_to.file(), Square::RANK_1);
            turn_into_ = (Piece::Letter)packed_to.rank();
        }
    }
}

HalfMove::HalfMove() :
    from_(Square()), to_(Square()), turn_into_(Piece::LETTER_NULL) {
}

int HalfMove::dx() const {
    return to().file_int() - from().file_int();
}

int HalfMove::dy() const {
    return to().rank_int() - from().rank_int();
}

Square HalfMove::packed_to() const {
    return turn_into_ == Piece::LETTER_NULL ? to() : Square(to().file(), (Square::Rank)turn_into_);
}

bool HalfMove::could_turn_into(const Board& board) const {
    return board.letter(from()) == Piece::PAWN && (to().rank() == Square::RANK_1 || to().rank() == Square::RANK_8);
}

std::string HalfMove::san_from_(const Board& board) const {
    Piece::Letter letter = board.letter(from());
    if (letter == Piece::PAWN && board.test_takes(*this)) {
        return char2str(from().file_char());
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
        return char2str(from().rank_char());
    } else if (alt) {
        return char2str(from().file_char());
    }
    return "";
}

bool HalfMove::operator==(const HalfMove& other) const {
    return from() == other.from() && to() == other.to();
}

bool HalfMove::operator!=(const HalfMove& other) const {
    return !(*this == other);
}

HalfMove::operator bool() const {
    return *this != HalfMove();
}

std::string HalfMove::san(const Board& board, const Board& board_after,
                          bool skip_pieces) const {
    std::string result;
    result.reserve(10);
    if (board.test_castling(*this)) {
        result += to().file() == Square::FILE_G ? "O-O" : "O-O-O";
    } else {
        Piece::Letter letter = board.letter(from());
        if (!skip_pieces && letter != Piece::PAWN) {
            result += Piece::piece_char(letter);
        }
        result += san_from_(board);
        if (board.test_takes(*this)) {
            result += 'x';
        }
        result += to().str();
        if (!skip_pieces) {
            if (turn_into() != Piece::LETTER_NULL) {
                (result += '=') += Piece::piece_char(turn_into());
            }
            if (board_after.test_shah()) {
                result += board_after.test_end() == Board::CHECKMATE ? '#' : '+';
            }
        }
    }
    return result;
}

}

