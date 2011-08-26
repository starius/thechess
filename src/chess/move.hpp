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
namespace chess {
class Move;
}
}

#include "xy.hpp"
#include "field.hpp"
#include "board.hpp"

namespace thechess {
namespace chess {

/** Class representing a single half-move.
A move consists of source and destination squares, and promoted piece letter
for pawn promotion moves.

The size of this class is 4 bytes.
Inside Moves, it spends only 1.5 bytes per half-move.
*/
class Move {
public:
    /** Construct a half-move using source, destination and promoted piece letter */
    Move(Xy from, Xy to, Chessman turn_into)
        : from_(from), to_(to), turn_into_(turn_into) {}

    /** Construct a half-move using source and destination squares.
    turn_into() of a returned move returns \ref chessman_null.
    */
    Move(Xy from, Xy to);

    /** Construct a half-move using source, packed destination and board.
    This is used by Moves while reading.
    In case of a pawn promotion packed_to is Xy(to.x(), (Yname)turn_into).
    Otherwise normal destination is used.

    This optimization let Moves spent only 1.5 bytes per half-move.
    */
    Move(Xy from, Xy packed_to, const Board& board);

    /** Default constructor.
    The result is \code Move(xy_null, xy_null, chessman_null) \endcode
    */
    Move();

    /** Get the source */
    Xy from() const {
        return from_;
    }

    /** Get the destination */
    Xy to() const {
        return to_;
    }

    /** Get the promoted piece letter */
    Chessman turn_into() const {
        return turn_into_;
    }

    /** Set the source */
    void from(Xy v) {
        from_ = v;
    }

    /** Set the destination */
    void to(Xy v) {
        to_ = v;
    }

    /** Set the promoted piece letter */
    void turn_into(Chessman v) {
        turn_into_ = v;
    }

    /** Return the difference of the files */
    int dx() const {
        return to().x_() - from().x_();
    }

    /** Return the difference of the ranks */
    int dy() const {
        return to().y_() - from().y_();
    }

    /** Return packed_to of this move.
    This is used by Moves while writing.
    In case of a pawn promotion packed_to is Xy(to.x(), (Yname)turn_into).
    Otherwise normal destination is returned.

    This optimization let Moves spent only 1.5 bytes per half-move.
    */
    Xy packed_to() const;

    /** Return if the move is pawn promotion */
    bool could_turn_into(const Board& board) const;

    /** Comparison operator.
    Moves with same source and destination are considered equal,
    even if piece promotion letters differ.
    */
    bool operator==(const Move& other) const {
        return from() == other.from() && to() == other.to();
    }

    /** Comparison operator.
    \sa Move::operator==
    */
    bool operator!=(const Move& other) const {
        return !(*this == other);
    }

    /** Return SAN (Standard Algebraic Notation) of the move.
    \param skip_chessmen if specified, all piece letters will be skipped.
        This is usefull when these letters are rendered as images.
    */
    std::string san(const Board& board, const Board& board_after,
                    bool skip_chessmen = false) const;

private:
    unsigned from_ : 7;
    unsigned to_ : 7;
    Chessman turn_into_ : 3;

    std::string san_from_(const Board& board) const;
};

const Move move_null(xy_null, xy_null);

}
}

#endif // THECHESS_MOVE_H_

