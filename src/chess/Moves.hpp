/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MOVES_H_
#define THECHESS_MOVES_H_

namespace thechess {
class Moves;
class MovesIterator;
}

#include <vector>

#include "HalfMove.hpp"
#include "Square.hpp"
#include "Board.hpp"
#include "tests.hpp"

/** Type representing a byte */
typedef unsigned char byte;

namespace thechess {

/** Vector of bytes */
typedef std::vector<byte> svuc;

/** Sequence of moves.
This class was designed to store a sequence of moves very compactly.

Sizes:
 - move: 24 bits (3 bytes)
 - half-move: 12 bits
 - square: 6 bits
 - file of rank: 3 bits
*/
class Moves : public svuc {
public:
    /** Constructor.
    \param moves_count number of half-moves to preserve space for
    */
    Moves(int moves_count = 0);

    /** Constructor.
    Fills container with moves_count moves from array moves.
    */
    Moves(HalfMove moves[], int moves_count);

    /** Get internal representation: vector of bytes */
    const svuc& as_svuc() const {
        return *this;
    }

    /** Get internal representation: vector of bytes */
    svuc& as_svuc() {
        return *this;
    }

    /** Return the number of half-moves */
    int size() const {
        return (this->svuc::size() * 8) / 12;
    }

    /** Push the half-move to the end of container */
    void push_move(HalfMove half_move);

    /** Pop last half-move */
    void pop_move();

    /** Pop several moves from the ending */
    void pop_moves(int number);

    /** Return the half-move.
    \param n index of half-move.
    \param board board position preceeding the half-move.
        This argument is nedded because of pawn promotions,
        which are stored together with HalfMove::to() (see HalfMove::packed_to())
    */
    HalfMove move_at(int n, const Board& board) const {
        return HalfMove(square_(n, SQUARE_FROM), square_(n, SQUARE_TO), board);
    }

    /** Return the half-move.
    \param n 0-based number of half-move.
    \note Does not work correctly for pawn promotions.
    */
    HalfMove move_at(int n) const {
        return HalfMove(square_(n, SQUARE_FROM), square_(n, SQUARE_TO));
    }

    /** Return board position before the half-move */
    Board board_at(int n) const;

    /** Test correctness.
    Returned value:
     - -1 if no errors were found
     - index of invalid half-move if any
    */
    int check() const;

    /** Convert index of half-move to number of move */
    static int n_to_human(int move_n) {
        return (move_n + 2) / 2;
    }

    /** Convert number of half-moves to number of moves */
    static int size_to_human(int size) {
        return n_to_human(size - 1);
    }

    /** Convert number of move and active color to index of half-move */
    static int n_from_human(int human_i, Color color) {
        return (human_i - 1) * 2 + ((color == WHITE) ? 0 : 1);
    }

    /** Return number of moves.
    \sa size()
    */
    int human_size() const {
        return size_to_human(size());
    }

    /** Return 0 for white half-move and 1 otherwise */
    static int order_int(int move_n) {
        return move_n % 2;
    }

    /** Return active color of half-move */
    static Color order(int move_n) {
        return order_int(move_n) == 0 ? WHITE : BLACK;
    }

    /** Write (PGN) movetext to stream.
    \param result the value of Result tag (see PGN description)
    \param reduced use reduced export PGN format
    */
    void pgn(std::ostream& out, const std::string& result,
             bool reduced = false) const;

private:
    enum Square_type {
        SQUARE_FROM = 0,
        SQUARE_TO = 1
    };

    byte q(int i) const {
        return (byte)((*this)[i]);
    }

    Square square_(int i) const;
    Square square_(int n, Square_type square_type) const {
        return square_(n * 2 + (int)square_type);
    }
    void square_(int i, Square square);
    void square_(int n, Square_type square_type, Square square) {
        square_(n * 2 + (int)square_type, square);
    }

    void set_move(int n, HalfMove half_move);
    void fill(int moves_count);
    friend void run_tests();
};

/** Iterator for Moves container.
\sa THECHESS_MOVES_FOREACH
*/
class MovesIterator {
public:
    /** Constructor.
    \param moves Container
    \param board Board instance using while iterating.
        This board should represent board position before \c from half-move.
    \param from Starting half-move.
    */
    MovesIterator(const Moves* moves, Board& board, int from = 0);

    /** Get current half-move */
    HalfMove half_move() const {
        return moves_->move_at(n, board_);
    }

    /** Indirection, get current half-move */
    HalfMove operator*() const {
        return half_move();
    }

    /** Increment, increases half-move index */
    MovesIterator& operator ++() {
        board_.make_move(half_move());
        n++;
        return *this;
    }

    /** Pointer to Moves */
    const Moves* moves_;

    /** Board position before current half-move */
    Board& board_;

    /** Index of current half-move */
    int n;
};

/** Iterate moves from \c from to \c to (exclusive).
Usage:
\code
Moves moves = ...
Board board = moves.board_at(10);
THECHESS_MOVES_FROM_TO (moves_it, moves, board, 10, 20) {
    HalfMove half_move = *move_it;
    Board board_after = board;
    board_after.make_move(half_move);
    std::cout << half_move.san(board, board_after) << std::endl;
}
\endcode
*/
#define THECHESS_MOVES_FROM_TO(move_it, moves, board, from, to) \
for (thechess::MovesIterator move_it = \
thechess::MovesIterator(moves, board, from); \
move_it.n < to; ++move_it)

/** Iterate moves from the beginning to \c to (exclusive).
Usage:
\code
Moves moves = ...
Board board;
THECHESS_MOVES_TO (moves_it, moves, board, 20) {
    HalfMove half_move = *move_it;
    Board board_after = board;
    board_after.make_move(half_move);
    std::cout << half_move.san(board, board_after) << std::endl;
}
\endcode
*/
#define THECHESS_MOVES_TO(move_it, moves, board, to) \
THECHESS_MOVES_FROM_TO (move_it, moves, board, 0, to)

/** Iterate moves from \c from to the end.
Usage:
\code
Moves moves = ...
Board board = moves.board_at(10);
THECHESS_MOVES_FROM (moves_it, moves, board, 10) {
    HalfMove half_move = *move_it;
    Board board_after = board;
    board_after.make_move(half_move);
    std::cout << half_move.san(board, board_after) << std::endl;
}
\endcode
*/
#define THECHESS_MOVES_FROM(move_it, moves, board, from) \
THECHESS_MOVES_FROM_TO (move_it, moves, board, from, (moves)->size())

/** Iterate all moves.
Usage:
\code
Moves moves = ...
Board board;
THECHESS_MOVES_FOREACH (moves_it, moves, board) {
    HalfMove half_move = *move_it;
    Board board_after = board;
    board_after.make_move(half_move);
    std::cout << half_move.san(board, board_after) << std::endl;
}
\endcode
*/
#define THECHESS_MOVES_FOREACH(move_it, moves, board) \
THECHESS_MOVES_FROM (move_it, moves, board, 0)

}

#endif

