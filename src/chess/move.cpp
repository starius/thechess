//

#include "chess/move.hpp"

namespace thechess {
namespace chess {

Move::Move(Xy from, Xy to) :
    from_(from), to_(to), turn_into_(chessman_null)
{
}

// ?? no tests of valideness of chessman
Move::Move(Xy from, Xy packed_to, const Board& board) :
    from_(from), to_(packed_to), turn_into_(chessman_null)
{
    if (board.chessman(this->from()) == pawn)
    {
        if (board.order() == white && this->from().y() == y_7)
        {
            to_ = Xy(packed_to.x(), y_8);
            turn_into_ = (Chessman)packed_to.y();
        }
        if (board.order() == black && this->from().y() == y_2)
        {
            to_ = Xy(packed_to.x(), y_1);
            turn_into_ = (Chessman)packed_to.y();
        }
    }
}

Move::Move() :
    from_(xy_null), to_(xy_null), turn_into_(chessman_null)
{
}

Xy Move::packed_to() const
{
    return turn_into_==chessman_null ? to() : Xy(to().x(), (Yname)turn_into_);
}

bool Move::could_turn_into(const Board& board) const
{
    return board.chessman(from()) == pawn && (to().y() == y_1 || to().y() == y_8);
}

std::string Move::str(const Board& board, const Board& board_after) const
{
    return board.test_castling(*this) ?
    (to().x() == x_g ? "0-0" : "0-0-0") :
    (from().str() +
    (board.test_takes(*this) ? ":" : "-") +
    to().str() +
    (board_after.test_shah() ? "+" : "")
    );
}

}
}

