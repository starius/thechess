/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MOVES_H_
#define THECHESS_MOVES_H_

#include <vector>
#include <iterator>

#include "chess/global.hpp"
#include "chess/HalfMove.hpp"
#include "chess/Square.hpp"
#include "chess/Board.hpp"

/** \defgroup chess Chess container library
Classes for dealing with chess data such as chess board and move history.
*/

namespace thechess {

/** Sequence of moves.
This class was designed to store a sequence of moves very compactly.

Sizes:
 - move: 24 bits (3 bytes)
 - half-move: 12 bits
 - square: 6 bits
 - file or rank: 3 bits

\ingroup chess
*/
class Moves {
public:
    /** Type representing a byte */
    typedef unsigned char byte;

    /** Vector of bytes */
    typedef std::vector<byte> svuc;

    /** Default constructor.
    Empty container
    */
    Moves();

    /** Constructor.
    Fills container with size half-moves from array moves.
    */
    Moves(HalfMove moves[], int size);

    /** Construct moves from a string returned by to_string() */
    Moves(const std::string& data);

    /** Return some string representation, usable in URL's */
    std::string to_string() const;

    /** Destructor */
    virtual ~Moves()
    { }

    /** \name Getters */
    /* @{ */

    /** Get internal representation: vector of bytes */
    const svuc& as_svuc() const {
        return svuc_;
    }

    /** Get internal representation: vector of bytes */
    svuc& as_svuc() {
        return svuc_;
    }

    /** Return the half-move.
    \param n index of half-move.
    \param board board position preceeding the half-move.
        This argument is nedded because of pawn promotions,
        which are stored together with HalfMove::to()
        (see HalfMove::packed_to())
    */
    HalfMove half_move(int n, const Board& board) const;

    /** Return the half-move.
    \param n 0-based number of half-move.
    \note Does not work correctly for pawn promotions.
    */
    virtual HalfMove half_move(int n) const;

    /** Return board position before the half-move.
    \note Takes O(n) time. Use CachedMoves::board instead
    */
    Board board(int n) const;

    /* @} */

    /** \name Modifiers */
    /* @{ */

    /** Push the half-move to the end of container */
    virtual void push(HalfMove half_move);

    /** Pop last half-move */
    virtual void pop();

    /** Pop several half-moves from the ending */
    void pop(int number);

    /** Strip replace half-move and all its descendants.
    This method pops several moves (see pop()) and
    pushes the half-move (see push()).
    */
    void reset_half_move(int n, HalfMove half_move);

    /* @} */

    /** \name Numbers */
    /* @{ */

    /** Return the number of half-moves */
    int size() const;

    /** Convert index of half-move to number of move */
    static int move_number(int move_n);

    /** Convert number of half-moves to number of moves */
    static int moves_number(int size);

    /** Convert number of move and active color to index of half-move */
    static int half_move_index(int move_number, Piece::Color color);

    /** Return number of moves.
    \sa size()
    */
    int moves_number() const;

    /** Return 0 for white half-move and 1 otherwise */
    static int order_int(int move_n);

    /** Return active color of half-move */
    static Piece::Color order(int move_n);

    /* @} */

    /** Test correctness.
    Returned value:
     - -1 if no errors were found
     - index of invalid half-move if any
    */
    int check() const;

    /** Write (PGN) movetext to stream.
    \param out     Output stream.
    \param result  The value of Result tag (see PGN description).
    \param reduced Use reduced export PGN format.
    */
    void pgn(std::ostream& out, const std::string& result,
             bool reduced = false) const;

    /** Apply func to range of half-moves.
    \param func function to apply
    \param from index of half-move to begin iteration
    \param to index of half-move to stop before.
        If to==-1, iterates from \c from to the end of the container.
    */
    void foreach(void* func(HalfMove half_move, const Board& board),
                 int from = 0, int to = -1) const;

protected:
    /** Base const itarator */
    class base_iterator :
        public std::iterator<std::input_iterator_tag, HalfMove> {
    public:
        /** Get moves */
        const Moves& moves() const {
            return *moves_;
        }

        /** Get half-move index */
        int n() const {
            return n_;
        }

        /** Increment, increases half-move index.
        Lazy method.
        */
        base_iterator& operator ++();

        /** Comparison operator */
        bool operator ==(const base_iterator& other) const;

        /** Comparison operator */
        bool operator !=(const base_iterator& other) const;

        /** Comparison operator */
        bool operator <(const base_iterator& other) const;

        /** Comparison operator */
        bool operator <=(const base_iterator& other) const;

    protected:
        /** Pointer to Moves */
        const Moves* moves_;

        /** Index of current half-move */
        int n_;

        /** Constructor */
        base_iterator(const Moves& moves, int from = 0);
    };

public:
    /** Const iterator with lazy applying of moves.
    Moves are applied from dereference operator and board().
    Constructor and increment operator do not cause applying of moves.
    */
    class const_iterator : public base_iterator {
    public:
        /** Indirection, get current half-move */
        HalfMove operator*() const;

        /** Get board before current half-move */
        const Board& board() const;

        friend class Moves;

    private:
        /** Board position */
        mutable Board board_;

        /** Index of half-move following board position */
        mutable int board_n_;

        const_iterator(const Moves& moves, int from = 0);

        /** Update board to position before n() */
        void sync_board() const;

        /** Return half-move following board position */
        HalfMove half_move() const;
    };

    /** Return iterator referring to the first half-move */
    const_iterator begin() const;

    /** Return iterator referring to the past-the-end half-move */
    const_iterator end() const;

    /** Return iterator referring to the last half-move */
    const_iterator back() const;

    /** Return iterator referring to the half-move */
    const_iterator iter(int from = 0) const;

private:
    svuc svuc_;

    enum SquareType {
        SQUARE_FROM = 0,
        SQUARE_TO = 1
    };

    byte q(int i) const;

    Square square(int i) const;
    Square square(int n, SquareType square_type) const;
    void square(int i, Square square);
    void square(int n, SquareType square_type, Square square);

    void set_half_move(int n, HalfMove half_move);
    friend void run_tests();
};

}

#endif

