/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <utility>
#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cctype>

#include "chess/Board.hpp"
#include "chess/Square.hpp"
#include "chess/Piece.hpp"

namespace thechess {

const int ORDER_BYTE = 32;
const int CASTLING_BYTE = 33;

void Board::init_pieces(Square::Rank rank, Piece::Color color) {
    piece(Square(Square::FILE_A, rank), Piece(color, Piece::ROOK));
    piece(Square(Square::FILE_B, rank), Piece(color, Piece::KNIGHT));
    piece(Square(Square::FILE_C, rank), Piece(color, Piece::BISHOP));
    piece(Square(Square::FILE_D, rank), Piece(color, Piece::QUEEN));
    piece(Square(Square::FILE_E, rank), Piece(color, Piece::KING));
    piece(Square(Square::FILE_F, rank), Piece(color, Piece::BISHOP));
    piece(Square(Square::FILE_G, rank), Piece(color, Piece::KNIGHT));
    piece(Square(Square::FILE_H, rank), Piece(color, Piece::ROOK));
}

void Board::init_pawns(Square::Rank rank, Piece::Color color) {
    for (int file = Square::FILE_A; file <= Square::FILE_H; file++) {
        piece(Square((Square::File)file, rank), Piece(color, Piece::PAWN));
    }
}

Board::Board() {
    memset(pieces_, 0x00, sizeof(pieces_));
    init_pieces(Square::RANK_1, Piece::WHITE);
    init_pawns(Square::RANK_2, Piece::WHITE);
    init_pieces(Square::RANK_8, Piece::BLACK);
    init_pawns(Square::RANK_7, Piece::BLACK);
    order(Piece::WHITE);
    castling_reset();
    long_pawn(false);
}

Board::byte Board::q(Square square) const {
    int i = square.i();
    byte b = pieces_[i / 2];
    return (i % 2) ? (b & 0x0F) : (b >> 4);
}

void Board::q(Square square, Board::byte q_) {
    int i = square.i();
    pieces_[i / 2] = i % 2 ?
                     ((pieces_[i / 2] & 0xF0) | (q_ & 0x0F)) :
                     ((pieces_[i / 2] & 0x0F) | (q_ << 4));
}

bool Board::isset(Square square) const {
    return letter(square) ? true : false;
}

void Board::unset(Square square) {
    q(square, 0);
}

Piece::Color Board::color(Square square) const {
    return isset(square) ? (q(square) & 0x08 ? (Piece::Color)1 : (Piece::Color)0) : Piece::COLOR_NULL; // 0000 1000
}

void Board::color(Square square, Piece::Color color) {
    byte Q = q(square);
    Q &= 0xF7; // 1111 0111
    Q |= color << 3;
    q(square, Q);
}

Piece::Letter Board::letter(Square square) const {
    return (Piece::Letter)(q(square) & 0x07); // 0000 0111
}

void Board::letter(Square square, Piece::Letter letter) {
    byte Q = q(square);
    Q &= 0xF8; // 1111 1000
    Q |= letter;
    q(square, Q);
}

Piece Board::piece(Square square) const {
    return Piece(color(square), letter(square));
}

void Board::piece(Square square, Piece piece) {
    byte Q = q(square);
    Q &= 0xF0; // 1111 0000
    Q |= piece.color() << 3;
    Q |= piece.letter();
    q(square, Q);
}

Piece::Color Board::order() const {
    return (Piece::Color)(pieces_[ORDER_BYTE]);
}

void Board::order(Piece::Color color) {
    pieces_[ORDER_BYTE] = color;
}

void Board::change_order() {
    order(Piece::other_color(order()));
}

bool Board::castling(Square rook_square) const {
    if (rook_square == Square(Square::FILE_A, Square::RANK_1)) {
        return pieces_[CASTLING_BYTE] & ~0x7F; // 0111 1111
    } else if (rook_square == Square(Square::FILE_H, Square::RANK_1)) {
        return pieces_[CASTLING_BYTE] & ~0xBF; // 1011 1111
    } else if (rook_square == Square(Square::FILE_A, Square::RANK_8)) {
        return pieces_[CASTLING_BYTE] & ~0xDF; // 1101 1111
    } else if (rook_square == Square(Square::FILE_H, Square::RANK_8)) {
        return pieces_[CASTLING_BYTE] & ~0xEF; // 1110 1111
    }
    return false;
}

void Board::castling_reset() {
    pieces_[CASTLING_BYTE] |= 0xF0;
}

void Board::castling_off(Square rook_square) {
    if (rook_square == Square(Square::FILE_A, Square::RANK_1)) {
        pieces_[CASTLING_BYTE] &= 0x7F; // 0111 1111
    } else if (rook_square == Square(Square::FILE_H, Square::RANK_1)) {
        pieces_[CASTLING_BYTE] &= 0xBF; // 1011 1111
    } else if (rook_square == Square(Square::FILE_A, Square::RANK_8)) {
        pieces_[CASTLING_BYTE] &= 0xDF; // 1101 1111
    } else if (rook_square == Square(Square::FILE_H, Square::RANK_8)) {
        pieces_[CASTLING_BYTE] &= 0xEF; // 1110 1111
    }
}

bool Board::long_pawn() const {
    return (pieces_[CASTLING_BYTE] & 0x08) ? true : false; // 0000 1000
}

Square::File Board::long_pawn_file() const {
    return Square::File(pieces_[CASTLING_BYTE] & 0x07); // 0000 0111
}

void Board::long_pawn(bool value, int file) {
    if (value) {
        pieces_[CASTLING_BYTE] |= 0x08; // 0000 1000
        pieces_[CASTLING_BYTE] |= file & 0x07; // 0000 0111
    } else {
        pieces_[CASTLING_BYTE] &= 0xF0; // 1111 0000
    }
}

int sign(int a) {
    return a > 0 ? 1 : (a < 0 ? -1 : 0);
}

void Board::simple_move(const HalfMove half_move) {
    piece(half_move.to(), piece(half_move.from()));
    unset(half_move.from());
}

void Board::make_move(const HalfMove half_move) {
    long_pawn(false);
    Piece active = piece(half_move.from());
    if (active.letter() == Piece::PAWN) {
        if (!isset(half_move.to()) && half_move.from().file() != half_move.to().file()) {
            // take on the aisle
            unset(Square(half_move.to().file(), half_move.from().rank()));
        }
        if (abs(half_move.dy()) == 2) {
            long_pawn(true, half_move.to().file_int());
        }
    }
    if (active.letter() == Piece::KING && half_move.from().file() == Square::FILE_5) {
        // potential castling -- move rook
        if (half_move.to().file() == Square::FILE_3) {
            simple_move(HalfMove(
                            Square(Square::FILE_1, half_move.from().rank()),
                            Square(Square::FILE_4, half_move.from().rank())));
        }
        if (half_move.to().file() == Square::FILE_7) {
            simple_move(HalfMove(
                            Square(Square::FILE_8, half_move.from().rank()),
                            Square(Square::FILE_6, half_move.from().rank())));
        }
    }
    simple_move(half_move);
    if (half_move.turn_into() != Piece::LETTER_NULL) {
        // turn pawn into ...
        letter(half_move.to(), half_move.turn_into());
    }
    // forbid castling in case king or rook moved
    if (active.letter() == Piece::KING) {
        Square rook = half_move.from();
        rook.file(Square::FILE_1);
        castling_off(rook);
        rook.file(Square::FILE_8);
        castling_off(rook);
    }
    if (active.letter() == Piece::ROOK) {
        castling_off(half_move.from());
    }
    if (letter(half_move.to()) == Piece::ROOK) {
        castling_off(half_move.to());
    }
    // change order of move
    change_order();
}

bool Board::simple_test_move(HalfMove half_move) const {
    int dx = half_move.dx();
    int dy = half_move.dy();
    if (!isset(half_move.from())) {
        return false;
    }
    Piece active = piece(half_move.from());
    if (active.color() == color(half_move.to())) {
        return false;
    }
    if (half_move.from().file() < Square::FILE_A || half_move.from().file() > Square::FILE_H ||
            half_move.to().file() < Square::FILE_A || half_move.to().file() > Square::FILE_H ||
            half_move.from().rank() < Square::RANK_1 || half_move.from().rank() > Square::RANK_8 ||
            half_move.to().rank() < Square::RANK_1 || half_move.to().rank() > Square::RANK_8) {
        return false;
    }
    if (half_move.from() == half_move.to()) {
        return false;
    }
    if (active.letter() == Piece::PAWN) {
        if (!isset(half_move.to()) && half_move.from().file() == half_move.to().file()) {
            // not eat
            if ((active.color() == Piece::WHITE && dy == 1) ||
                    (active.color() == Piece::BLACK && dy == -1)) {
                return true;
            }
            if ((active.color() == Piece::WHITE && dy == 2 && half_move.from().rank() == Square::RANK_2) ||
                    (active.color() == Piece::BLACK && dy == -2 && half_move.from().rank() == Square::RANK_7)) {
                int my = (int)half_move.from().rank_int() + dy / 2;
                if (!isset(Square(half_move.from().file_int(), my))) {
                    return true;
                }
            }
            return false;
        }
        if (isset(half_move.to()) && abs(dx) == 1) {
            if ((active.color() == Piece::WHITE && dy == 1) ||
                    (active.color() == Piece::BLACK && dy == -1)) {
                return true;
            }
        }
        return false;
    }
    if (active.letter() == Piece::KING) {
        if (abs(dx) <= 1 && abs(dy) <= 1) {
            return true;
        }
        return false;
    }
    if (active.letter() == Piece::KNIGHT) {
        if (abs(dx) == 2 && abs(dy) == 1) {
            return true;
        }
        if (abs(dx) == 1 && abs(dy) == 2) {
            return true;
        }
        return false;
    }
    if (active.letter() == Piece::BISHOP && abs(dx) != abs(dy)) {
        return false;
    }
    if (active.letter() == Piece::ROOK && (dx != 0 && dy != 0)) {
        return false;
    }
    if (active.letter() == Piece::QUEEN && (dx != 0 && dy != 0) && abs(dx) != abs(dy)) {
        return false;
    }
    if (active.letter() == Piece::BISHOP ||
            active.letter() == Piece::ROOK ||
            active.letter() == Piece::QUEEN) {
        int vx = sign(dx);
        int vy = sign(dy);
        for (Square p = half_move.from(); p != half_move.to(); p.file(p.file_int() + vx), p.rank(p.rank_int() + vy)) {
            if (p != half_move.from() && isset(p)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool Board::test_attack(Square square) const {
    Piece::Color c = color(square);
    THECHESS_SQUARE_FOREACH (from) {
        if (isset(from) && color(from) != c) {
            if (simple_test_move(HalfMove(from, square))) {
                return true;
            }
        }
    }
    return false;
}

bool Board::test_attack(Square square, Piece::Color c) const {
    if (color(square) == c) {
        return test_attack(square);
    } else {
        Piece previous = piece(square);
        const_cast<Board*>(this)->piece(square, Piece(c, Piece::PAWN));
        bool result = test_attack(square);
        const_cast<Board*>(this)->piece(square, previous);
        return result;
    }
}

bool Board::test_attack(HalfMove half_move) const {
    Piece previous = piece(half_move.from());
    const_cast<Board*>(this)->unset(half_move.from());
    bool result = test_attack(half_move.to(), previous.color());
    const_cast<Board*>(this)->piece(half_move.from(), previous);
    if (!result && previous.letter() == Piece::PAWN && abs(half_move.dy()) == 2) {
        // take on the aisle
        const_cast<Board*>(this)->simple_move(half_move);
        Square middle(half_move.from().file(), (half_move.from().rank() + half_move.to().rank()) / 2);
        const_cast<Board*>(this)->change_order();
        result |= test_move(HalfMove(Square(half_move.to().file() - 1, half_move.to().rank()), middle));
        result |= test_move(HalfMove(Square(half_move.to().file() + 1, half_move.to().rank()), middle));
        const_cast<Board*>(this)->change_order();
        const_cast<Board*>(this)->simple_move(HalfMove(half_move.to(), half_move.from()));
    }
    return result;
}

Board::FinishState Board::test_end() const {
    FinishState state = NOTHING;
    if (!some_move()) {
        state = test_shah() ? CHECKMATE : STALEMATE;
    }
    return state;
}

bool Board::test_shah(Piece::Color c) const {
    Square king_square = find_king(c);
    if (king_square) {
        return test_attack(king_square);
    } else {
        return true; // no king
    }
}

bool Board::test_move(const HalfMove half_move) const {
    Board board_copy = *this;
    bool castling_test = false;
    Piece active = piece(half_move.from());
    if (active.color() != order()) {
        return false;
    }
    if (active.letter() == Piece::KING && half_move.from().file() == Square::FILE_5 &&
            (half_move.to().file() == Square::FILE_3 || half_move.to().file() == Square::FILE_7)) {
        // only castling
        if (half_move.from().rank() != Square::RANK_1 && half_move.from().rank() != Square::RANK_8) {
            return false;
        }
        if (half_move.from().rank() != half_move.to().rank()) {
            return false;
        }
        Square::File rook_from_file = half_move.to().file() == Square::FILE_3 ? Square::FILE_1 : Square::FILE_8;
        Square rook_from = Square(rook_from_file, half_move.to().rank());
        if (!castling(rook_from)) {
            return false;
        }
        if (half_move.to().file() == Square::FILE_3) {
            // check piece crossed by Piece::ROOK
            if (isset(Square(Square::FILE_2, half_move.to().rank()))) {
                return false;
            }
        }
        if (isset(half_move.to())) {
            return false;
        }
        Square::File rook_to_file = half_move.to().file() == Square::FILE_3 ? Square::FILE_4 : Square::FILE_6;
        Square rook_to = Square(rook_to_file, half_move.to().rank());
        if (isset(rook_to)) {
            return false;
        }
        if (test_attack(rook_to, active.color())) {
            return false;
        }
        if (test_attack(half_move.from(), active.color())) {
            return false;
        }
        castling_test = true;
    }
    if (active.letter() == Piece::PAWN &&
            abs(half_move.dx()) == 1 && abs(half_move.dy()) == 1 && !isset(half_move.to())) {
        // take on the aisle
        Square eaten_pawn_square = Square(half_move.to().file(), half_move.from().rank());
        if (letter(eaten_pawn_square) != Piece::PAWN) {
            return false;
        }
        if (color(eaten_pawn_square) == active.color()) {
            return false;
        }
        if (!long_pawn() || long_pawn_file() != eaten_pawn_square.file()) {
            return false;
        }
        // imagine this pawn to be at half_move.to()
        board_copy.simple_move(HalfMove(eaten_pawn_square, half_move.to()));
    }
    //~ Square::Rank move_to_rank = real_to_rank(half_move);
    //~ half_move.to().rank(move_to_rank);
    if (!castling_test && !board_copy.simple_test_move(half_move)) {
        return false;
    }
    board_copy.make_move(half_move);
    // shah test
    if (board_copy.test_shah(active.color())) {
        return false;
    }
    return true;
}

Square Board::some_target(Square from) const {
    if (color(from) == order()) { // optimization
        THECHESS_SQUARE_FOREACH (to) {
            if (test_move(HalfMove(from, to))) {
                return to;
            }
        }
    }
    return Square();
}

Square Board::some_source(Square to) const {
    THECHESS_SQUARE_FOREACH (from) {
        if (test_move(HalfMove(from, to))) {
            return from;
        }
    }
    return Square();
}

HalfMove Board::some_move() const {
    THECHESS_SQUARE_FOREACH (from) {
        Square to = some_target(from);
        if (to) {
            return HalfMove(from, to);
        }
    }
    return HalfMove();
}

bool Board::can_move(Square from) const {
    return some_target(from) != Square();
}

bool Board::test_shah() const {
    return test_shah(order());
}

Square Board::find_king(Piece::Color c) const {
    THECHESS_SQUARE_FOREACH (square) {
        if (piece(square) == Piece(c, Piece::KING)) {
            return square;
        }
    }
    return Square();
}

bool Board::test_takes(const HalfMove half_move) const {
    return isset(half_move.to()) ||
           (letter(half_move.from()) == Piece::PAWN && half_move.from().file() != half_move.to().file());
}

bool Board::test_castling(const HalfMove half_move) const {
    return letter(half_move.from()) == Piece::KING && abs(half_move.dx()) == 2;
}

void Board::fen_pieces(std::ostream& out) const {
    for (Square::Rank rank = Square::RANK_8; rank >= Square::RANK_1; rank = (Square::Rank)((int)rank - 1)) {
        if (rank < Square::RANK_8) {
            out << '/';
        }
        int empty_pieces = 0;
        THECHESS_FILE_FOREACH (file) {
            Square square(file, rank);
            if (isset(square)) {
                if (empty_pieces) {
                    out << empty_pieces;
                    empty_pieces = 0;
                }
                char pc = Piece::piece_char(letter(square));
                if (color(square) == Piece::BLACK) {
                    pc = tolower(pc);
                }
                out << pc;
            } else {
                empty_pieces += 1;
            }
        }
        if (empty_pieces) {
            out << empty_pieces;
        }
    }
}

void Board::fen_castling(std::ostream& out) const {
    bool castling_availability = false;
    if (castling(Square(Square::FILE_H, Square::RANK_1))) {
        out << 'K';
        castling_availability = true;
    }
    if (castling(Square(Square::FILE_A, Square::RANK_1))) {
        out << 'Q';
        castling_availability = true;
    }
    if (castling(Square(Square::FILE_H, Square::RANK_8))) {
        out << 'k';
        castling_availability = true;
    }
    if (castling(Square(Square::FILE_A, Square::RANK_8))) {
        out << 'q';
        castling_availability = true;
    }
    if (!castling_availability) {
        out << '-';
    }
}

void Board::fen(std::ostream& out, int halfmove, int fullmove) const {
    fen_pieces(out);
    out << ' ';
    out << (order() == Piece::WHITE ? 'w' : 'b');
    out << ' ';
    fen_castling(out);
    out << ' ';
    if (long_pawn()) {
        out << Square::file_char(long_pawn_file()) << (order() == Piece::WHITE ? '6' : '3');
    } else {
        out << '-';
    }
    out << ' ';
    out << halfmove;
    out << ' ';
    out << fullmove;
}

}

