/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef FIELD_H_
#define FIELD_H_

#include <boost/format.hpp>

#include <Wt/WString>

namespace thechess {

/** The enum for piece color */
enum Color {
    WHITE = 0, /**< White color */
    BLACK = 1, /**< Black color */
    COLOR_NULL = 2 /**< Undetermined color, can be the return value */
};

/** The number of Color members */
const int COLOR_COUNT = 3;

/** Piece letter */
enum Letter {
    LETTER_NULL = 0, /**< Undefined piece letter, can be the return value */
    BISHOP = 1, /**< Bishop */
    KING   = 2, /**< King */
    KNIGHT = 3, /**< Knight */
    PAWN   = 4, /**< Pawn */
    QUEEN  = 5, /**< Queen */
    ROCK   = 6  /**< Rock */
};

/** The number of Letter members */
const int LETTER_COUNT = 7;

const char* const PIECE_LETTERS = "-BKNPQR";

/** Return l9n string for the color */
Wt::WString color2str(Color color);

/** If the color is white return black, else return white */
inline Color other_color(Color color) {
    return color == WHITE ? BLACK : WHITE;
}

/** Return char of piece letter */
inline char piece_char(Letter letter) {
    return PIECE_LETTERS[(int)letter];
}

/** Piece.
This class consists of Color and Letter members.

\ingroup chess
*/
class Piece {
public:
    /** Construct a piece using a color and a letter */
    Piece(Color color, Letter letter);

    /** Comparison operator */
    bool operator==(const Piece& piece) const {
        return color() == piece.color() && letter() == piece.letter();
    }

    /** Comparison operator */
    bool operator!=(const Piece& piece) const {
        return color() != piece.color() || letter() != piece.letter();
    }

    /** Get piece color */
    Color color() const {
        return color_;
    }

    /** Get piece letter */
    Letter letter() const {
        return piece_;
    }

    /** Set piece color */
    void color(Color v) {
        color_ = v;
    }

    /** Set piece letter */
    void letter(Letter v) {
        piece_ = v;
    }

private:
    Color color_;
    Letter piece_;
};

}

#endif

