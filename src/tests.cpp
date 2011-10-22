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

#include "tests.hpp"
#include "chess/Board.hpp"
#include "chess/HalfMove.hpp"
#include "chess/Moves.hpp"
#include "chess/Square.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "utils/rand.hpp"

namespace thechess {

void run_tests() {
    Board test_board;
    assert(test_board.isset(Square(Square::FILE_E, Square::RANK_2)) == true);
    assert(test_board.color(Square(Square::FILE_E, Square::RANK_1)) == Piece::WHITE);
    assert(test_board.letter(Square(Square::FILE_E, Square::RANK_1)) == Piece::KING);
    assert(test_board.color(Square(Square::FILE_E, Square::RANK_2)) == Piece::WHITE);
    assert(test_board.letter(Square(Square::FILE_E, Square::RANK_2)) == Piece::PAWN);
    // Piece::PAWN
    assert(test_board.simple_test_move(HalfMove(Square(Square::FILE_E, Square::RANK_2), Square(Square::FILE_E, Square::RANK_3))) == true);
    assert(test_board.simple_test_move(HalfMove(Square(Square::FILE_E, Square::RANK_2), Square(Square::FILE_E, Square::RANK_4))) == true);
    assert(test_board.simple_test_move(HalfMove(Square(Square::FILE_E, Square::RANK_2), Square(Square::FILE_E, Square::RANK_5))) == false);
    assert(test_board.simple_test_move(HalfMove(Square(Square::FILE_E, Square::RANK_2), Square(Square::FILE_E, Square::RANK_8))) == false);
    assert(test_board.test_move(HalfMove(Square(Square::FILE_E, Square::RANK_2), Square(Square::FILE_E, Square::RANK_3))) == true);
    assert(test_board.test_move(HalfMove(Square(Square::FILE_E, Square::RANK_2), Square(Square::FILE_E, Square::RANK_4))) == true);
    assert(test_board.test_move(HalfMove(Square(Square::FILE_E, Square::RANK_2), Square(Square::FILE_E, Square::RANK_5))) == false);
    // Piece::KNIGHT
    assert(test_board.simple_test_move(HalfMove(Square(Square::FILE_B, Square::RANK_1), Square(Square::FILE_C, Square::RANK_3))) == true);
    assert(test_board.simple_test_move(HalfMove(Square(Square::FILE_B, Square::RANK_1), Square(Square::FILE_D, Square::RANK_3))) == false);
    // Piece::KING
    assert(test_board.simple_test_move(HalfMove(Square(Square::FILE_E, Square::RANK_1), Square(Square::FILE_E, Square::RANK_2))) == false);
    Moves moves0 = Moves();
    moves0.push(HalfMove(Square(Square::FILE_E, Square::RANK_2), Square(Square::FILE_E, Square::RANK_4)));
    moves0.push(HalfMove(Square(Square::FILE_E, Square::RANK_7), Square(Square::FILE_E, Square::RANK_5)));
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

