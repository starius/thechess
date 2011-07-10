/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

//
#ifndef THECHESS_BOARD_H_
#define THECHESS_BOARD_H_

namespace thechess {
    namespace chess {
        class Board;
    }
}

#include <string>

#include "tests.hpp"
#include "move.hpp"
#include "xy.hpp"
#include "field.hpp"

namespace thechess {
namespace chess {

const int order_byte = 32;
const int castling_byte = 33;
const int fields_size = 34;
typedef unsigned char byte;
typedef byte Fields[34];

enum FinishState
{
    nothing = 0,
    checkmate = 1, // мат
    stalemate = 2  // пат
};

class Board
{
public:
    void init_chessmans(Yname y, Color color);
    void init_pawns(Yname y, Color color);
    Board();
    const Fields& fields() const { return fields_; }

    bool isset(Xy xy) const;
    Color color(Xy xy) const;
    Chessman chessman(Xy xy) const;
    Field field(Xy xy) const;

    void make_move(const Move move);

    Color order() const;
    bool castling(Xy rock_xy) const;
    bool long_pawn() const;
    Xname long_pawn_x() const;

    bool test_move(const Move move) const;
    bool test_attack(Xy xy, Color c) const;
    bool test_attack(Move move) const;
    FinishState test_end() const;

    const std::string to_string() const
        { return std::string((char*)fields_, sizeof(fields_)); }
    const std::string to_table() const;

    Xy some_target(Xy from) const;
    Xy some_source(Xy to) const;
    Move some_move() const;
    bool can_move(Xy from) const { return some_target(from) != xy_null; }
    bool test_shah(Color color) const;
    bool test_shah() const { return test_shah(order()); }
    Xy find_king(Color c) const;

    bool test_takes(const Move move) const;
    bool test_castling(const Move move) const;

private:
    Fields fields_;
    byte q(Xy xy) const; // quarta of this field
    void q(Xy xy, byte q_);

    void unset(Xy xy);
    void color(Xy xy, Color c);
    void chessman(Xy xy, Chessman chessman);
    void field(Xy xy, Field field);

    void order(Color c);
    void change_order();
    void castling_reset();
    void castling_off(Xy rock_xy);
    void long_pawn(bool value, int x=0);

    void simple_move(const Move move);
    bool simple_test_move(Move move) const;
    bool test_attack(Xy xy) const;
friend void run_tests();
};

}
}

/* fields_: 34 bytes:
 *
 * 1) 64 * 4 bites = 32 bytes (0-31)
 * 4 bites = (Color << 3 | Chessman)
 * if is not set: 4 bites = 0
 * 2) 1 byte (32'd): == bool Order (Color)
 * 3) 1 byte (33'th):
 * castling (4) = a1, h1, a8, h8
 * Last pawn long move: 1 bit (0 if was not) + 3 bits (X of pawn)
 * Note: (a1, h1 - white; a8, h8 - black, a1, a8 - long; h1, h8 - short)
*/


#endif // THECHESS_BOARD_H_
