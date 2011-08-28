/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include "chess/HalfMove.hpp"
#include "utils.hpp"

namespace thechess {

Move::Move(Xy from, Xy to) :
    from_(from), to_(to), turn_into_(CHESSMAN_NULL) {
}

// ?? no tests of valideness of chessman
Move::Move(Xy from, Xy packed_to, const Board& board) :
    from_(from), to_(packed_to), turn_into_(CHESSMAN_NULL) {
    if (board.chessman(this->from()) == PAWN) {
        if (board.order() == WHITE && this->from().y() == Y_7) {
            to_ = Xy(packed_to.x(), Y_8);
            turn_into_ = (Chessman)packed_to.y();
        }
        if (board.order() == BLACK && this->from().y() == Y_2) {
            to_ = Xy(packed_to.x(), Y_1);
            turn_into_ = (Chessman)packed_to.y();
        }
    }
}

Move::Move() :
    from_(XY_NULL), to_(XY_NULL), turn_into_(CHESSMAN_NULL) {
}

Xy Move::packed_to() const {
    return turn_into_ == CHESSMAN_NULL ? to() : Xy(to().x(), (Yname)turn_into_);
}

bool Move::could_turn_into(const Board& board) const {
    return board.chessman(from()) == PAWN && (to().y() == Y_1 || to().y() == Y_8);
}

std::string Move::san_from_(const Board& board) const {
    Chessman chessman = board.chessman(from());
    if (chessman == PAWN && board.test_takes(*this)) {
        return char2str(x_char(from().x()));
    }
    bool alt = false, alt_x = false, alt_y = false;
    THECHESS_XY_FOREACH (xy) {
        Move alt_move(xy, to(), board);
        if (board.chessman(xy) == chessman && xy != from() &&
                board.test_move(alt_move) && !board.test_castling(alt_move)) {
            alt = true;
            if (xy.x() == from().x()) {
                alt_x = true;
            }
            if (xy.y() == from().y()) {
                alt_y = true;
            }
        }
    }
    if (alt_x && alt_y) {
        return from().str();
    } else if (alt_x) {
        return char2str(y_char(from().y()));
    } else if (alt) {
        return char2str(x_char(from().x()));
    }
    return "";
}

std::string Move::san(const Board& board, const Board& board_after,
                      bool skip_chessmen) const {
    std::string result;
    result.reserve(10);
    if (board.test_castling(*this)) {
        result += to().x() == X_G ? "O-O" : "O-O-O";
    } else {
        Chessman chessman = board.chessman(from());
        if (!skip_chessmen && chessman != PAWN) {
            result += chessman_char(chessman);
        }
        result += san_from_(board);
        if (board.test_takes(*this)) {
            result += 'x';
        }
        result += to().str();
        if (!skip_chessmen) {
            if (turn_into() != CHESSMAN_NULL) {
                (result += '=') += chessman_char(turn_into());
            }
            if (board_after.test_shah()) {
                result += board_after.test_end() == CHECKMATE ? '#' : '+';
            }
        }
    }
    return result;
}

}

