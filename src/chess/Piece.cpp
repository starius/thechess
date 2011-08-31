/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include "chess/Piece.hpp"

namespace thechess {

const char* const PIECE_LETTERS = "-BKNPQR";

Piece::Piece(Piece::Color color, Piece::Letter letter) :
    color_(color), piece_(letter) {
}

bool Piece::operator==(const Piece& piece) const {
    return color() == piece.color() && letter() == piece.letter();
}

bool Piece::operator!=(const Piece& piece) const {
    return color() != piece.color() || letter() != piece.letter();
}

Wt::WString Piece::color_str() const {
    return (color() == WHITE) ? Wt::WString::tr("tc.game.white") :
           ((color() == BLACK) ? Wt::WString::tr("tc.game.black") :
            Wt::WString::tr("thechess.nocolor"));
}

Piece::Color Piece::other_color(Piece::Color color) {
    return color == WHITE ? BLACK : WHITE;
}

char Piece::piece_char(Piece::Letter letter) {
    return PIECE_LETTERS[(int)letter];
}

}

