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
    assert(test_board.isset(Square(FILE_E, RANK_2)) == true);
    assert(test_board.color(Square(FILE_E, RANK_1)) == WHITE);
    assert(test_board.letter(Square(FILE_E, RANK_1)) == KING);
    assert(test_board.color(Square(FILE_E, RANK_2)) == WHITE);
    assert(test_board.letter(Square(FILE_E, RANK_2)) == PAWN);
    // PAWN
    assert(test_board.simple_test_move(HalfMove(Square(FILE_E, RANK_2), Square(FILE_E, RANK_3))) == true);
    assert(test_board.simple_test_move(HalfMove(Square(FILE_E, RANK_2), Square(FILE_E, RANK_4))) == true);
    assert(test_board.simple_test_move(HalfMove(Square(FILE_E, RANK_2), Square(FILE_E, RANK_5))) == false);
    assert(test_board.simple_test_move(HalfMove(Square(FILE_E, RANK_2), Square(FILE_E, RANK_8))) == false);
    assert(test_board.test_move(HalfMove(Square(FILE_E, RANK_2), Square(FILE_E, RANK_3))) == true);
    assert(test_board.test_move(HalfMove(Square(FILE_E, RANK_2), Square(FILE_E, RANK_4))) == true);
    assert(test_board.test_move(HalfMove(Square(FILE_E, RANK_2), Square(FILE_E, RANK_5))) == false);
    // KNIGHT
    assert(test_board.simple_test_move(HalfMove(Square(FILE_B, RANK_1), Square(FILE_C, RANK_3))) == true);
    assert(test_board.simple_test_move(HalfMove(Square(FILE_B, RANK_1), Square(FILE_D, RANK_3))) == false);
    // KING
    assert(test_board.simple_test_move(HalfMove(Square(FILE_E, RANK_1), Square(FILE_E, RANK_2))) == false);
    Moves moves0 = Moves();
    moves0.push_move(HalfMove(Square(FILE_E, RANK_2), Square(FILE_E, RANK_4)));
    moves0.push_move(HalfMove(Square(FILE_E, RANK_7), Square(FILE_E, RANK_5)));
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

