/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */


#include "chess/move.hpp"

namespace thechess {
namespace chess {

Move::Move(Xy from, Xy to) :
    from_(from), to_(to), turn_into_(chessman_null) {
}

// ?? no tests of valideness of chessman
Move::Move(Xy from, Xy packed_to, const Board& board) :
    from_(from), to_(packed_to), turn_into_(chessman_null) {
    if (board.chessman(this->from()) == pawn) {
        if (board.order() == white && this->from().y() == y_7) {
            to_ = Xy(packed_to.x(), y_8);
            turn_into_ = (Chessman)packed_to.y();
        }
        if (board.order() == black && this->from().y() == y_2) {
            to_ = Xy(packed_to.x(), y_1);
            turn_into_ = (Chessman)packed_to.y();
        }
    }
}

Move::Move() :
    from_(xy_null), to_(xy_null), turn_into_(chessman_null) {
}

Xy Move::packed_to() const {
    return turn_into_==chessman_null ? to() : Xy(to().x(), (Yname)turn_into_);
}

bool Move::could_turn_into(const Board& board) const {
    return board.chessman(from()) == pawn && (to().y() == y_1 || to().y() == y_8);
}

std::string Move::pgn_from(const Board& board) const {
    chess::Chessman chessman = board.chessman(from());
    if (chessman == pawn && board.test_takes(*this)) {
        return chess::x_string(from().x());
    }
    bool alt=false, alt_x=false, alt_y=false;
    THECHESS_XY_FOREACH (xy) {
        chess::Move alt_move(xy, to(), board);
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
        return chess::y_string(from().y());
    } else if (alt) {
        return chess::x_string(from().x());
    }
    return "";
}

std::string Move::pgn(const Board& board, const Board& board_after,
                      bool skip_chessmen) const {
    std::string result;
    result.reserve(10);
    if (board.test_castling(*this)) {
        result += to().x() == chess::x_g ? "O-O" : "O-O-O";
    } else {
        chess::Chessman chessman = board.chessman(from());
        if (!skip_chessmen && chessman != chess::pawn) {
            result += chess::chessman_char(chessman);
        }
        result += pgn_from(board);
        if (board.test_takes(*this)) {
            result += 'x';
        }
        result += to().str();
        if (!skip_chessmen) {
            if (turn_into() != chess::chessman_null) {
                (result += '=') += chess::chessman_char(turn_into());
            }
            if (board_after.test_shah()) {
                result += board_after.test_end() == checkmate ? '#' : '+';
            }
        }
    }
    return result;
}



}
}

