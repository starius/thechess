/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_BOARD_H_
#define THECHESS_BOARD_H_

namespace thechess {
class Board;
}

#include <string>
#include <ostream>

#include "tests.hpp"
#include "move.hpp"
#include "xy.hpp"
#include "field.hpp"

namespace thechess {

const int order_byte = 32;
const int castling_byte = 33;
const int fields_size = 34;
typedef unsigned char byte;
typedef byte Fields[34];

/** Result of testing if board is checkmate or stalemate */
enum FinishState {
    nothing = 0,
    checkmate = 1,
    stalemate = 2
};

/** Chess board representation.
Stores information about position, active color, castling availability,
en passant target square. There are methods of the class to get information
about board and to apply a move to board.

Objects of this class are very light, consuming only 34 bytes per object:
    - 64 * 4 bites = 32 bytes (0-31)
      - 4 bites = (Color << 3 | Chessman)
      - if is not set: 4 bites = 0
    - 1 byte (32'd): bool Order (Color)
    - 1 byte (33'th):
      - castling (4) = a1, h1, a8, h8.
        Note: a1, h1 - white; a8, h8 - black, a1, a8 - long; h1, h8 - short
      - Last pawn long move: 1 bit (0 if was not) + 3 bits (X of pawn)

*/
class Board {
public:

    /** Construct a board with start position */
    Board();

    /** Apply the move to the board.
    The validness of move is not tested.
    */
    void make_move(const Move move);

    /** Return FEN notation of the board.
    \param halfmove This is the number of halfmoves since
        the last pawn advance or capture.
    \param fullmove The number of the full move.
        It starts at 1, and is incremented after Black's move.
    */
    void fen(std::ostream& out, int halfmove, int fullmove) const;

    /** \name Low level methods */
    /* @{ */

    /** Return if the square of board is not empty */
    bool isset(Xy xy) const;

    /** Return the color of the square.
    If the square is empty, return \ref color_null */
    Color color(Xy xy) const;

    /** Return the chessman of the square.
    If the square is empty, return \ref chessman_null */
    Chessman chessman(Xy xy) const;

    /** Return the \ref Field structure of the square.
    If the square is empty, return \ref Field constructed from
    \ref color_null and \ref chessman_null */
    Field field(Xy xy) const;

    /** Return active color.
    For ended games, return the color of loser
    */
    Color order() const;

    /** Return availability of castling involving the rock.
    The castling may be impossible even if this method returns true,
    since it takes into account only permanent impossibility of castling
    caused by move involving the king or the rock.
    \sa test_move
    */
    bool castling(Xy rock_xy) const;

    /** Return if the previous move was long pawn move */
    bool long_pawn() const;

    /** Return the file of previous long pawn move.
    If previous move was not long pawn move, return \ref x_a
    */
    Xname long_pawn_x() const;

    /* @} */

    /** \name Tests of move */
    /* @{ */

    /** Return if the move can be applied to the board.
    All chess rules are applied.
    */
    bool test_move(const Move move) const;

    /** Return if the square were attacked if it would be of given color */
    bool test_attack(Xy xy, Color c) const;

    /** Return if the destination will be attacked after the move */
    bool test_attack(Move move) const;

    /** Return if the destination is attacked by the move */
    bool test_takes(const Move move) const;

    /** Return if the move is a castling */
    bool test_castling(const Move move) const;

    /* @} */

    /** \name Searching for moves */
    /* @{ */

    /** Return some target-of-move square for the piece */
    Xy some_target(Xy from) const;

    /** Return some source-of-move square for the piece */
    Xy some_source(Xy to) const;

    /** Return some possible move */
    Move some_move() const;

    /** Return if the piece can be a target of any move */
    bool can_move(Xy from) const {
        return some_target(from) != xy_null;
    }

    /* @} */

    /** \name King-related methods */
    /* @{ */

    /** Return if the board is in checking state for pieces of the color */
    bool test_shah(Color color) const;

    /** Return if the board is in checking state.
    This convenience method returns \c test_shah(order())
    */
    bool test_shah() const {
        return test_shah(order());
    }

    /** Return the square of the king of the color */
    Xy find_king(Color c) const;

    /** Return finish state of the board */
    FinishState test_end() const;

    /* @} */

private:
    Fields fields_;

    void init_chessmans(Yname y, Color color);
    void init_pawns(Yname y, Color color);

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
    void long_pawn(bool value, int x = 0);

    void simple_move(const Move move);
    bool simple_test_move(Move move) const;
    bool test_attack(Xy xy) const;

    void fen_pieces(std::ostream& out) const;
    void fen_castling(std::ostream& out) const;

    friend void run_tests();
};

}

#endif // THECHESS_BOARD_H_
