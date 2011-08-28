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

Piece::Piece(Color color, Letter letter) :
    color_(color), piece_(letter) {
}

Wt::WString color2str(Color color) {
    return (color == WHITE) ? Wt::WString::tr("tc.game.white") :
           ((color == BLACK) ? Wt::WString::tr("tc.game.black") :
            Wt::WString::tr("thechess.nocolor"));
}

}

