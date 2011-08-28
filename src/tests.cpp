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
#include "chess/Board.hpp"
#include "chess/HalfMove.hpp"
#include "chess/Moves.hpp"
#include "chess/Square.hpp"
#include "model/User.hpp"
#include "ThechessApplication.hpp"
#include "rand.hpp"

namespace thechess {

void run_tests() {
    Board test_board;
    assert(test_board.isset(Xy(X_E, Y_2)) == true);
    assert(test_board.color(Xy(X_E, Y_1)) == WHITE);
    assert(test_board.chessman(Xy(X_E, Y_1)) == KING);
    assert(test_board.color(Xy(X_E, Y_2)) == WHITE);
    assert(test_board.chessman(Xy(X_E, Y_2)) == PAWN);
    // PAWN
    assert(test_board.simple_test_move(Move(Xy(X_E, Y_2), Xy(X_E, Y_3))) == true);
    assert(test_board.simple_test_move(Move(Xy(X_E, Y_2), Xy(X_E, Y_4))) == true);
    assert(test_board.simple_test_move(Move(Xy(X_E, Y_2), Xy(X_E, Y_5))) == false);
    assert(test_board.simple_test_move(Move(Xy(X_E, Y_2), Xy(X_E, Y_8))) == false);
    assert(test_board.test_move(Move(Xy(X_E, Y_2), Xy(X_E, Y_3))) == true);
    assert(test_board.test_move(Move(Xy(X_E, Y_2), Xy(X_E, Y_4))) == true);
    assert(test_board.test_move(Move(Xy(X_E, Y_2), Xy(X_E, Y_5))) == false);
    // KNIGHT
    assert(test_board.simple_test_move(Move(Xy(X_B, Y_1), Xy(X_C, Y_3))) == true);
    assert(test_board.simple_test_move(Move(Xy(X_B, Y_1), Xy(X_D, Y_3))) == false);
    // KING
    assert(test_board.simple_test_move(Move(Xy(X_E, Y_1), Xy(X_E, Y_2))) == false);
    Moves moves0 = Moves();
    moves0.push_move(Move(Xy(X_E, Y_2), Xy(X_E, Y_4)));
    moves0.push_move(Move(Xy(X_E, Y_7), Xy(X_E, Y_5)));
    assert(moves0.size() == 2);
    assert(moves0.check() == -1);
    // rand (binomial test, p=q=1/2)
    const int n = 10000;
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += rr(2);
    }
    int sigma = sqrt(n * 0.5 * 0.5);
    BOOST_ASSERT(abs(sum - n / 2) < 10 * sigma);
}

}
