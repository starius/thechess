//

#ifndef THECHESS_MOVES_H_
#define THECHESS_MOVES_H_

namespace thechess {
    namespace chess {
        struct MovesCheck;
        class Moves;
        class MovesIterator;
    }
}

#include <vector>

#include "move.hpp"
#include "xy.hpp"
#include "board.hpp"
#include "tests.hpp"

typedef unsigned char byte;

namespace thechess {
 namespace chess {

struct MovesCheck
{
    bool correct;
    int move_with_error;
    Board board;
};

enum Xy_type
{
    xy_from = 0,
    xy_to = 1
};

typedef std::vector<byte> svuc;

class Moves : public svuc
{
public:
    Moves(int moves_count=0);
    Moves(Move moves[], int moves_count);
    const svuc& as_svuc() const { return *this; }
    svuc& as_svuc() { return *this; }
    int size() const { return (this->svuc::size() * 8) / 12; }

    void push_move(Move move);
    void pop_move();
    void pop_moves(int number);

    Move move_at(int n, const Board& board) const
    {
        return Move(xy_(n, xy_from), xy_(n, xy_to), board);
    }
    Move move_at(int n) const
    {
        return Move(xy_(n, xy_from), xy_(n, xy_to));
    }
    Board board_at(int n) const; // returns board position BEFORE move number n

    int check() const; // -1 = no errors, other value -- n of wrong move

    static int n_to_human(int move_n) { return (move_n + 2) / 2; }
    static int size_to_human(int size) { return n_to_human(size - 1); }
    static int n_from_human(int human_i, Color color)
    {
        return (human_i - 1) * 2 + ((color == white) ? 0 : 1);
    }
    int human_size() const { return size_to_human(size()); }

    static int order_int(int move_n) { return move_n % 2; }
    static Color order(int move_n)
    {
        return order_int(move_n) == 0 ? white : black;
    }

private:
    byte q(int i) const { return (byte)((*this)[i]); }

    Xy xy_(int i) const;
    Xy xy_(int n, Xy_type xy_type) const { return xy_(n * 2 + (int)xy_type); }
    void xy_(int i, Xy xy);
    void xy_(int n, Xy_type xy_type, Xy xy) { xy_(n * 2 + (int)xy_type, xy); }

    void set_move(int n, Move move);
    void fill(int moves_count);
friend void run_tests();
};

class MovesIterator
{
public:
    MovesIterator(const Moves* moves, Board& board, int from=0);
    //~ MovesIterator(const Moves* moves, int from=0);
    Move move() const { return moves_->move_at(n, board_); }
    Move operator*() const { return move(); }
    MovesIterator& operator ++() { board_.make_move(move()); n++; return *this; }
    const Moves* moves_;
    Board& board_;
    int n;
};


#define THECHESS_MOVES_FROM_TO(move_it, moves, board, from, to) \
for (thechess::chess::MovesIterator move_it = \
thechess::chess::MovesIterator(moves, board, from); \
move_it.n < to; ++move_it)

#define THECHESS_MOVES_TO(move_it, moves, board, to) \
THECHESS_MOVES_FROM_TO(move_it, moves, board, 0, to)

#define THECHESS_MOVES_FROM(move_it, moves, board, from) \
THECHESS_MOVES_FROM_TO(move_it, moves, board, from, (moves)->size())

#define THECHESS_MOVES_FOREACH(move_it, moves, board) \
THECHESS_MOVES_FROM(move_it, moves, board, 0)

}
}


/* moves_:
 * each move takes 12 bites, each Xy -- 6 bits, each coordinate -- 3 bits
 * pair of moves -- 3 bytes
 */

#endif // THECHESS_MOVES_H_

