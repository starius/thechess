/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */


#include "chess/xy.hpp"

namespace thechess {
namespace chess {


Xy::Xy(Xname x, Yname y) :
 i_((unsigned)x * 8 + (unsigned)y)
{
}

Xy::Xy(unsigned x, unsigned y) :
 i_((unsigned)x * 8 + (unsigned)y)
{
}

Xy::Xy(unsigned i) :
 i_(i)
{
}

Xy::Xy() :
 i_(chess::xy_null)
{
}

}
}

