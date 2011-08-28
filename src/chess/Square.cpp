/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include "chess/Square.hpp"

namespace thechess {

Square::Square(File file, Rank rank) :
    i_((unsigned)file * 8 + (unsigned)rank) {
}

Square::Square(unsigned file, unsigned rank) :
    i_((unsigned)file * 8 + (unsigned)rank) {
}

Square::Square(unsigned i) :
    i_(i) {
}

Square::Square() :
    i_(SQUARE_NULL) {
}

}

