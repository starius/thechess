
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

