/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

//
#ifndef THECHESS_MOVE_H_
#define THECHESS_MOVE_H_

namespace thechess {
namespace chess {
class Move;
}
}

#include "xy.hpp"
#include "field.hpp"
#include "board.hpp"

namespace thechess {
namespace chess {

class Move {
public:
    Move(Xy from, Xy to, Chessman turn_into)
        : from_(from), to_(to), turn_into_(turn_into) {}

    Move(Xy from, Xy to);
    Move(Xy from, Xy packed_to, const Board& board);
    Move();

    Xy from() const {
        return from_;
    }
    Xy to() const {
        return to_;
    }
    Chessman turn_into() const {
        return turn_into_;
    }
    void from(Xy v) {
        from_ = v;
    }
    void to(Xy v) {
        to_ = v;
    }
    void turn_into(Chessman v) {
        turn_into_ = v;
    }

    int dx() const {
        return to().x_() - from().x_();
    }
    int dy() const {
        return to().y_() - from().y_();
    }

    Xy packed_to() const;

    bool could_turn_into(const Board& board) const;

    bool operator==(const Move& other) const {
        return from() == other.from() && to() == other.to();
    }

    bool operator!=(const Move& other) const {
        return !(*this == other);
    }

    std::string pgn_from(const Board& board) const;
    std::string pgn(const Board& board, const Board& board_after,
                    bool skip_chessmen=false) const;

private:
    unsigned from_ : 7;
    unsigned to_ : 7;
    Chessman turn_into_ : 3;
};

const Move move_null(xy_null, xy_null);

}
}

#endif // THECHESS_MOVE_H_

