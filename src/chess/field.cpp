

#include "chess/field.hpp"

namespace thechess {
 namespace chess {


Field::Field(Color color, Chessman chessman) :
    color_(color), chessman_(chessman)
{
}

Wt::WString color2str(Color color)
{
    return (color == white) ? Wt::WString::tr("thechess.white") :
        ((color == black) ? Wt::WString::tr("thechess.black") :
        Wt::WString::tr("thechess.nocolor"));
}

}
}

