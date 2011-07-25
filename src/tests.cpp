/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */


#include <cassert>
#include <cmath>
#include <iostream>
#include <boost/assert.hpp>

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
#include "rand.hpp"

namespace thechess {
namespace chess {

void run_tests() {
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


    // rand (binomial test, p=q=1/2)
    const int n = 10000;
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += random::rr(2);
    }
    int sigma = sqrt(n*0.5*0.5);
    BOOST_ASSERT(abs(sum - n/2) < 10*sigma);
}



}
}
