//

#include <cassert>
#include <iostream>

#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

#include "tests.hpp"
#include "chess/board.hpp"
#include "chess/move.hpp"
#include "chess/moves.hpp"
#include "chess/xy.hpp"
#include "model/User.hpp"
#include "ThechessApplication.hpp"

namespace thechess {
namespace chess {

void run_tests()
{
    Board test_board;

    assert(test_board.isset(Xy(x_e, y_2)) == true);

    assert(test_board.color(Xy(x_e, y_1)) == white);
    assert(test_board.chessman(Xy(x_e, y_1)) == king);
    assert(test_board.color(Xy(x_e, y_2)) == white);
    assert(test_board.chessman(Xy(x_e, y_2)) == pawn);

    // pawn
    assert(test_board.simple_test_move(Move(Xy(x_e, y_2), Xy(x_e, y_3))) == true);
    assert(test_board.simple_test_move(Move(Xy(x_e, y_2), Xy(x_e, y_4))) == true);
    assert(test_board.simple_test_move(Move(Xy(x_e, y_2), Xy(x_e, y_5))) == false);
    assert(test_board.simple_test_move(Move(Xy(x_e, y_2), Xy(x_e, y_8))) == false);
    assert(test_board.test_move(Move(Xy(x_e, y_2), Xy(x_e, y_3))) == true);
    assert(test_board.test_move(Move(Xy(x_e, y_2), Xy(x_e, y_4))) == true);
    assert(test_board.test_move(Move(Xy(x_e, y_2), Xy(x_e, y_5))) == false);

    // knight
    assert(test_board.simple_test_move(Move(Xy(x_b, y_1), Xy(x_c, y_3))) == true);
    assert(test_board.simple_test_move(Move(Xy(x_b, y_1), Xy(x_d, y_3))) == false);

    // king
    assert(test_board.simple_test_move(Move(Xy(x_e, y_1), Xy(x_e, y_2))) == false);

    Moves moves0 = Moves();
    moves0.push_move(Move(Xy(x_e, y_2), Xy(x_e, y_4)));
    moves0.push_move(Move(Xy(x_e, y_7), Xy(x_e, y_5)));
    assert(moves0.size() == 2);
    assert(moves0.check() == -1);

}



}
}
