/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */



#include "chess/field.hpp"

namespace thechess {
namespace chess {


Field::Field(Color color, Chessman chessman) :
    color_(color), chessman_(chessman) {
}

Wt::WString color2str(Color color) {
    return (color == white) ? Wt::WString::tr("tc.game.white") :
           ((color == black) ? Wt::WString::tr("tc.game.black") :
            Wt::WString::tr("thechess.nocolor"));
}

}
}

