/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

//
#ifndef THECHESS_MOVE_H_
#define THECHESS_MOVE_H_

namespace thechess {
class HalfMove;
}

#include "Square.hpp"
#include "Piece.hpp"
#include "Board.hpp"

namespace thechess {

/** Class representing a single half-move.
A half-move consists of source and destination squares, and promoted piece letter
for pawn promotion moves.

The size of this class is 4 bytes.
Inside Moves, it spends only 1.5 bytes per half-move.

\ingroup chess
*/
class HalfMove {
public:
    /** Construct a half-move using source, destination and promoted piece letter */
    HalfMove(Square from, Square to, Piece::Letter turn_into)
        : from_(from), to_(to), turn_into_(turn_into) {}

    /** Construct a half-move using source and destination squares.
    turn_into() of a constructed half-move returns \ref Piece::LETTER_NULL.
    */
    HalfMove(Square from, Square to);

    /** Construct a half-move using source, packed destination and board.
    This is used by Moves while reading.
    In case of a pawn promotion packed_to is Square(to.file(), (Square::Rank)turn_into).
    Otherwise normal destination is used.

    This optimization let Moves spent only 1.5 bytes per half-move.
    */
    HalfMove(Square from, Square packed_to, const Board& board);

    /** Default constructor.
    The result is \code HalfMove(Square(), Square(), Piece::LETTER_NULL) \endcode
    */
    HalfMove();

    /** Get the source */
    Square from() const {
        return from_;
    }

    /** Get the destination */
    Square to() const {
        return to_;
    }

    /** Get the promoted piece letter */
    Piece::Letter turn_into() const {
        return turn_into_;
    }

    /** Set the source */
    void from(Square v) {
        from_ = v;
    }

    /** Set the destination */
    void to(Square v) {
        to_ = v;
    }

    /** Set the promoted piece letter */
    void turn_into(Piece::Letter v) {
        turn_into_ = v;
    }

    /** Return the difference of the files */
    int dx() const;

    /** Return the difference of the ranks */
    int dy() const;

    /** Return packed_to.
    This is used by Moves while writing.
    In case of a pawn promotion packed_to is Square(to.file(), (Square::Rank)turn_into).
    Otherwise normal destination is returned.

    This optimization let Moves spent only 1.5 bytes per half-move.
    */
    Square packed_to() const;

    /** Return if the half-move is pawn promotion */
    bool could_turn_into(const Board& board) const;

    /** Comparison operator.
    Moves with same source and destination are considered equal,
    even if piece promotion letters differ.
    */
    bool operator==(const HalfMove& other) const;

    /** Comparison operator.
    \sa HalfMove::operator==
    */
    bool operator!=(const HalfMove& other) const;

    /** Check for null */
    operator bool() const;

    /** Return SAN (Standard Algebraic Notation) of the half-move.
    \param skip_pieces if specified, all piece letters will be skipped.
        This is usefull when these letters are rendered as images.
    */
    std::string san(const Board& board, const Board& board_after,
                    bool skip_pieces = false) const;

private:
    unsigned from_ : 7;
    unsigned to_ : 7;
    Piece::Letter turn_into_ : 3;

    std::string san_from_(const Board& board) const;
};

}

#endif

