/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
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
#include "HalfMove.hpp"
#include "Square.hpp"
#include "Piece.hpp"

namespace thechess {

/** Chess board representation.
Stores information about position, active color, castling availability,
en passant target square. There are methods of the class to get information
about board and to apply moves to board.

Objects of this class are very light, consuming only 34 bytes per object:
    - 64 * 4 bites = 32 bytes (0-31)
      - 4 bites = (Color << 3 | Letter)
      - if is not set: 4 bites = 0
    - 1 byte (32'd): bool Order (Color)
    - 1 byte (33'th):
      - castling (4) = a1, h1, a8, h8.
        Note: a1, h1 - white; a8, h8 - black, a1, a8 - long; h1, h8 - short
      - Pawn promotion: 1 bit (0 if was not) + 3 bits (FILE of pawn)

\ingroup chess
*/
class Board {
public:
    /** Result of testing if board is checkmate or stalemate */
    enum FinishState {
        NOTHING = 0,
        CHECKMATE = 1,
        STALEMATE = 2
    };

    /** Construct a board with start position */
    Board();

    /** Apply the half-move to the board.
    The validness of half-move is not tested.
    */
    void make_move(const HalfMove half_move);

    /** Return FEN notation of the board.
    \param halfmove This is the number of halfmoves since
        the last pawn advance or capture.
    \param fullmove The number of the full move.
        It starts at 1, and is incremented after Black's half-move.
    */
    void fen(std::ostream& out, int halfmove, int fullmove) const;

    /** \name Low level methods */
    /* @{ */

    /** Return if the square of board is not empty */
    bool isset(Square square) const;

    /** Return the color of the square.
    If the square is empty, return \ref Piece::COLOR_NULL */
    Piece::Color color(Square square) const;

    /** Return the letter of the square.
    If the square is empty, return \ref Piece::LETTER_NULL */
    Piece::Letter letter(Square square) const;

    /** Return the \ref Piece structure of the square.
    If the square is empty, return \ref Piece constructed from
    \ref Piece::COLOR_NULL and \ref Piece::LETTER_NULL */
    Piece piece(Square square) const;

    /** Return active color.
    For ended games, return the color of loser
    */
    Piece::Color order() const;

    /** Return availability of castling involving the rook.
    The castling may be impossible even if this method returns true,
    since it takes into account only permanent impossibility of castling
    caused by move involving the king or the rook.
    \sa test_move
    */
    bool castling(Square rook_square) const;

    /** Return if the previous half-move was long pawn half-move */
    bool long_pawn() const;

    /** Return the file of previous long pawn half-move.
    If previous half-move was not long pawn half-move, return \ref Square::FILE_A
    */
    Square::File long_pawn_file() const;

    /* @} */

    /** \name Tests of half-move */
    /* @{ */

    /** Return if the half-move can be applied to the board.
    All chess rules are applied.
    */
    bool test_move(const HalfMove half_move) const;

    /** Return if the square were attacked if it would be of given color */
    bool test_attack(Square square, Piece::Color c) const;

    /** Return if the destination will be attacked after the half-move */
    bool test_attack(HalfMove half_move) const;

    /** Return if the destination is attacked by the half-move */
    bool test_takes(const HalfMove half_move) const;

    /** Return if the half-move is a castling */
    bool test_castling(const HalfMove half_move) const;

    /* @} */

    /** \name Searching for moves */
    /* @{ */

    /** Return some target square */
    Square some_target(Square from) const;

    /** Return some source square */
    Square some_source(Square to) const;

    /** Return some possible half-move */
    HalfMove some_move() const;

    /** Return if the square can be a target of any half-move */
    bool can_move(Square from) const;

    /* @} */

    /** \name King-related methods */
    /* @{ */

    /** Return if the board is in checking state for pieces of the color */
    bool test_shah(Piece::Color color) const;

    /** Return if the board is in checking state.
    This convenience method returns \c test_shah(order())
    */
    bool test_shah() const;

    /** Return the square of the king of the color */
    Square find_king(Piece::Color c) const;

    /** Return finish state of the board */
    FinishState test_end() const;

    /* @} */

private:
    typedef unsigned char byte;
    static const int PIECES_SIZE = 34;
    typedef byte Pieces[PIECES_SIZE];

    Pieces pieces_;

    void init_pieces(Square::Rank rank, Piece::Color color);
    void init_pawns(Square::Rank rank, Piece::Color color);

    byte q(Square square) const; // quarta of this piece
    void q(Square square, byte q_);

    void unset(Square square);
    void color(Square square, Piece::Color c);
    void letter(Square square, Piece::Letter letter);
    void piece(Square square, Piece piece);

    void order(Piece::Color c);
    void change_order();
    void castling_reset();
    void castling_off(Square rook_square);
    void long_pawn(bool value, int file = 0);

    void simple_move(const HalfMove half_move);
    bool simple_test_move(HalfMove half_move) const;
    bool test_attack(Square square) const;

    void fen_pieces(std::ostream& out) const;
    void fen_castling(std::ostream& out) const;

    friend void run_tests();
};

}

#endif

