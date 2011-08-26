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
    white = 0, /**< White color */
    black = 1, /**< Black color */
    color_null = 2 /**< Undetermined color, can be the return value */
};

/** The number of Color members */
const int Color_count = 3;

/** Piece letter */
enum Chessman {
    chessman_null = 0, /**< Undefined piece letter, can be the return value */
    bishop = 1, /**< Bishop */
    king   = 2, /**< King */
    knight = 3, /**< Knight */
    pawn   = 4, /**< Pawn */
    queen  = 5, /**< Queen */
    rock   = 6  /**< Rock */
};

/** The number of Chessman members */
const int Chessman_count = 7;

const char* const letters_chessmen = "-BKNPQR";

/** Return l9n string for the color */
Wt::WString color2str(Color color);

/** If the color is white return black, else return white */
inline Color other_color(Color color) {
    return color == white ? black : white;
}

/** Return char of piece letter */
inline char chessman_char(Chessman chessman) {
    return letters_chessmen[(int)chessman];
}

/** Piece.
This class consists of Color and Chessman members.
*/
class Field {
public:
    /** Construct a piece using a color and a letter */
    Field(Color color, Chessman chessman);

    /** Comparison operator */
    bool operator==(const Field& field) const {
        return color() == field.color() && chessman() == field.chessman();
    }

    /** Comparison operator */
    bool operator!=(const Field& field) const {
        return color() != field.color() || chessman() != field.chessman();
    }

    /** Get piece color */
    Color color() const {
        return color_;
    }

    /** Get piece letter */
    Chessman chessman() const {
        return chessman_;
    }

    /** Set piece color */
    void color(Color v) {
        color_ = v;
    }

    /** Set piece letter */
    void chessman(Chessman v) {
        chessman_ = v;
    }

private:
    Color color_;
    Chessman chessman_;
};

}

#endif // FIELD_H_

