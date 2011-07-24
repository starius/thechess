/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <utility>
#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cctype>

#include "chess/board.hpp"
#include "chess/xy.hpp"
#include "chess/field.hpp"


namespace thechess {
namespace chess {

void Board::init_chessmans(Yname y, Color color)
{
    field(Xy(x_a, y), Field(color, rock));
    field(Xy(x_b, y), Field(color, knight));
    field(Xy(x_c, y), Field(color, bishop));
    field(Xy(x_d, y), Field(color, queen));
    field(Xy(x_e, y), Field(color, king));
    field(Xy(x_f, y), Field(color, bishop));
    field(Xy(x_g, y), Field(color, knight));
    field(Xy(x_h, y), Field(color, rock));
}

void Board::init_pawns(Yname y, Color color)
{
    for (int x = x_a; x <= x_h; x++)
    {
        field(Xy((Xname)x, y), Field(color, pawn));
    }
}

Board::Board()
{
    memset(fields_, 0x00, sizeof(fields_));
    init_chessmans(y_1, white);
    init_pawns(y_2, white);
    init_chessmans(y_8, black);
    init_pawns(y_7, black);

    order(white);
    castling_reset();
    long_pawn(false);
}

byte Board::q(Xy xy) const
{
    int i = xy.i();
    byte b = fields_[i/2];
    return (i%2) ? (b & 0x0F) : (b >> 4);
}

void Board::q(Xy xy, byte q_)
{
    int i = xy.i();

    fields_[i/2] = i%2 ?
    ((fields_[i/2] & 0xF0) | (q_ & 0x0F)) :
    ((fields_[i/2] & 0x0F) | (q_ << 4));
}

bool Board::isset(Xy xy) const
{
    return chessman(xy) ? true : false;
}

void Board::unset(Xy xy)
{
    q(xy, 0);
}

Color Board::color(Xy xy) const
{
    return isset(xy) ? (q(xy) & 0x08 ? (Color)1 : (Color)0) : color_null; // 0000 1000
}

void Board::color(Xy xy, Color color)
{
    byte Q = q(xy);
    Q &= 0xF7; // 1111 0111
    Q |= color << 3;
    q(xy, Q);
}

Chessman Board::chessman(Xy xy) const
{
    return (Chessman)(q(xy) & 0x07); // 0000 0111
}

void Board::chessman(Xy xy, Chessman chessman)
{
    byte Q = q(xy);
    Q &= 0xF8; // 1111 1000
    Q |= chessman;
    q(xy, Q);
}

Field Board::field(Xy xy) const
{
    return Field(color(xy), chessman(xy));
}

void Board::field(Xy xy, Field field)
{
    byte Q = q(xy);
    Q &= 0xF0; // 1111 0000
    Q |= field.color() << 3;
    Q |= field.chessman();
    q(xy, Q);
}

Color Board::order() const
{
    return (Color)(fields_[order_byte]);
}

void Board::order(Color color)
{
    fields_[order_byte] = color;
}

void Board::change_order()
{
    order(other_color(order()));
}

bool Board::castling(Xy rock_xy) const
{
    if (rock_xy == Xy(x_a, y_1))
    {
        return fields_[castling_byte] & ~0x7F; // 0111 1111
    }
    else if (rock_xy == Xy(x_h, y_1))
    {
        return fields_[castling_byte] & ~0xBF; // 1011 1111
    }
    else if (rock_xy == Xy(x_a, y_8))
    {
        return fields_[castling_byte] & ~0xDF; // 1101 1111
    }
    else if (rock_xy == Xy(x_h, y_8))
    {
        return fields_[castling_byte] & ~0xEF; // 1110 1111
    }
    return false;
}

void Board::castling_reset()
{
    fields_[castling_byte] |= 0xF0;
}

void Board::castling_off(Xy rock_xy)
{
    if (rock_xy == Xy(x_a, y_1))
    {
        fields_[castling_byte] &= 0x7F; // 0111 1111
    }
    else if (rock_xy == Xy(x_h, y_1))
    {
        fields_[castling_byte] &= 0xBF; // 1011 1111
    }
    else if (rock_xy == Xy(x_a, y_8))
    {
        fields_[castling_byte] &= 0xDF; // 1101 1111
    }
    else if (rock_xy == Xy(x_h, y_8))
    {
        fields_[castling_byte] &= 0xEF; // 1110 1111
    }
}

bool Board::long_pawn() const
{
    return (fields_[castling_byte] & 0x08) ? true : false; // 0000 1000
}

Xname Board::long_pawn_x() const
{
    return Xname(fields_[castling_byte] & 0x07); // 0000 0111
}


void Board::long_pawn(bool value, int x)
{
    if (value)
    {
        fields_[castling_byte] |= 0x08; // 0000 1000
        fields_[castling_byte] |= x & 0x07; // 0000 0111
    }
    else
    {
        fields_[castling_byte] &= 0xF0; // 1111 0000
    }
}

int sign(int a)
{
    return a>0 ? 1 : (a<0 ? -1 : 0);
}

void Board::simple_move(const Move move)
{
    field(move.to(), field(move.from()));
    unset(move.from());
}

void Board::make_move(const Move move)
{
    long_pawn(false);

    Field active = field(move.from());

    if (active.chessman() == pawn)
    {
        if (!isset(move.to()) && move.from().x() != move.to().x())
        {
            // take on the aisle
            unset(Xy(move.to().x(), move.from().y()));
        }
        if (abs(move.dy()) == 2)
        {
            long_pawn(true, move.to().x_());
        }
    }

    if (active.chessman() == king && move.from().x() == x_5)
    {
        // potential castling -- move rock
        if (move.to().x() == x_3)
        {
            simple_move(Move(
            Xy(x_1, move.from().y()),
            Xy(x_4, move.from().y())));
        }
        if (move.to().x() == x_7)
        {
            simple_move(Move(
            Xy(x_8, move.from().y()),
            Xy(x_6, move.from().y())));
        }
    }

    simple_move(move);

    if (move.turn_into() != chessman_null)
    {
        // turn pawn into ...
        chessman(move.to(), move.turn_into());
    }

    // forbid castling in case king or rock moved
    if (active.chessman() == king)
    {
        Xy rock = move.from();
        rock.x(x_1);
        castling_off(rock);
        rock.x(x_8);
        castling_off(rock);
    }
    if (active.chessman() == rock)
    {
        castling_off(move.from());
    }
    if (chessman(move.to()) == rock)
    {
        castling_off(move.to());
    }

    // change order of move
    change_order();
}

bool Board::simple_test_move(Move move) const
{
    int dx = move.dx();
    int dy = move.dy();

    if (!isset(move.from()))
    {
        return false;
    }
    Field active = field(move.from());
    if (active.color() == color(move.to()))
    {
        return false;
    }

    if (move.from().x() < x_a || move.from().x() > x_h ||
    move.to().x() < x_a || move.to().x() > x_h ||
    move.from().y() < y_1 || move.from().y() > y_8 ||
    move.to().y() < y_1 || move.to().y() > y_8)
    {
        return false;
    }

    if (move.from() == move.to())
    {
        return false;
    }

    if (active.chessman() == pawn)
    {
        if (!isset(move.to()) && move.from().x() == move.to().x())
        {
            // not eat

            if ((active.color() == white && dy == 1) ||
            (active.color() == black && dy == -1))
            {
                return true;
            }

            if ((active.color() == white && dy == 2 && move.from().y() == y_2) ||
            (active.color() == black && dy == -2 && move.from().y() == y_7))
            {
                int my = (int)move.from().y_() + dy/2;
                if (!isset(Xy(move.from().x_(), my)))
                {
                    return true;
                }
            }
            return false;
        }

        if (isset(move.to()) && abs(dx) == 1)
        {
            if ((active.color() == white && dy == 1) ||
            (active.color() == black && dy == -1))
            {
                return true;
            }
        }
        return false;
    }

    if (active.chessman() == king)
    {
        if (abs(dx) <= 1 && abs(dy) <= 1)
        {
                return true;
        }
        return false;
    }

    if (active.chessman() == knight)
    {
        if (abs(dx) == 2 && abs(dy) == 1)
        {
            return true;
        }

        if (abs(dx) == 1 && abs(dy) == 2)
        {
            return true;
        }
        return false;
    }


    if (active.chessman() == bishop && abs(dx) != abs(dy))
    {
        return false;
    }
    if (active.chessman() == rock && (dx != 0 && dy != 0))
    {
        return false;
    }
    if (active.chessman() == queen && (dx != 0 && dy != 0) && abs(dx) != abs(dy))
    {
        return false;
    }

    if (active.chessman() == bishop ||
    active.chessman() == rock ||
    active.chessman() == queen)
    {
        int vx = sign(dx);
        int vy = sign(dy);
        for(Xy p = move.from(); p != move.to(); p.x_(p.x_() + vx), p.y_(p.y_() + vy))
        {
            if (p != move.from() && isset(p))
            {
                return false;
            }
        }
        return true;
    }

    return false;
}

bool Board::test_attack(Xy xy) const
{
    Color c = color(xy);

    THECHESS_XY_FOREACH (from)
    {
        if (isset(from) && color(from) != c)
        {
            if (simple_test_move(Move(from, xy)))
            {
                return true;
            }
        }
    }
    return false;
}

bool Board::test_attack(Xy xy, Color c) const
{
    if (color(xy) == c)
    {
        return test_attack(xy);
    }
    else
    {
        Field previous = field(xy);
        const_cast<Board*>(this)->field(xy, Field(c, pawn));
        bool result = test_attack(xy);
        const_cast<Board*>(this)->field(xy, previous);
        return result;
    }
}

bool Board::test_attack(Move move) const
{
    Field previous = field(move.from());
    const_cast<Board*>(this)->unset(move.from());
    bool result = test_attack(move.to(), previous.color());
    const_cast<Board*>(this)->field(move.from(), previous);
    if (!result && previous.chessman() == pawn && abs(move.dy()) == 2)
    {
        // take on the aisle
        const_cast<Board*>(this)->simple_move(move);
        Xy middle(move.from().x(), (move.from().y()+move.to().y())/2);
        const_cast<Board*>(this)->change_order();
        result |= test_move(Move(Xy(move.to().x()-1,move.to().y()), middle));
        result |= test_move(Move(Xy(move.to().x()+1,move.to().y()), middle));
        const_cast<Board*>(this)->change_order();
        const_cast<Board*>(this)->simple_move(Move(move.to(), move.from()));
    }
    return result;
}

FinishState Board::test_end() const
{
    FinishState state = nothing;
    if (some_move() == move_null)
    {
        state = test_shah() ? checkmate : stalemate;
    }
    return state;
}

bool Board::test_shah(Color c) const
{
    Xy king_xy = find_king(c);
    if (king_xy != xy_null)
    {
        return test_attack(king_xy);
    }
    else
    {
        return true; // no king
    }
}

bool Board::test_move(const Move move) const
{
    Board board_copy = *this;
    bool castling_test = false;
    Field active = field(move.from());

    if (active.color() != order())
    {
        return false;
    }

    if (active.chessman() == king && move.from().x() == x_5 &&
    (move.to().x() == x_3 || move.to().x() == x_7))
    {
        // only castling
        if (move.from().y() != y_1 && move.from().y() != y_8)
        {
            return false;
        }
        if (move.from().y() != move.to().y())
        {
            return false;
        }

        Xname rock_from_x = move.to().x() == x_3 ? x_1 : x_8;
        Xy rock_from = Xy(rock_from_x, move.to().y());
        if (!castling(rock_from))
        {
            return false;
        }

        if (move.to().x() == x_3)
        {
            // check field crossed by rock
            if (isset(Xy(x_2, move.to().y())))
            {
                return false;
            }
        }

        if (isset(move.to()))
        {
            return false;
        }

        Xname rock_to_x = move.to().x() == x_3 ? x_4 : x_6;
        Xy rock_to = Xy(rock_to_x, move.to().y());
        if (isset(rock_to))
        {
            return false;
        }
        if (test_attack(rock_to, active.color()))
        {
            return false;
        }

        if (test_attack(move.from(), active.color()))
        {
            return false;
        }

        castling_test = true;
    }

    if (active.chessman() == pawn &&
    abs(move.dx()) == 1 && abs(move.dy()) == 1 && !isset(move.to()))
    {
        // take on the aisle
        Xy eaten_pawn_xy = Xy(move.to().x(), move.from().y());
        if (chessman(eaten_pawn_xy) != pawn)
        {
            return false;
        }
        if (color(eaten_pawn_xy) == active.color())
        {
            return false;
        }
        if (!long_pawn() || long_pawn_x() != eaten_pawn_xy.x())
        {
            return false;
        }

        // imagine this pawn to be at move.to()
        board_copy.simple_move(Move(eaten_pawn_xy, move.to()));
    }

    //~ Yname move_to_y = real_to_y(move);
    //~ move.to().y(move_to_y);

    if (!castling_test && !board_copy.simple_test_move(move))
    {
        return false;
    }

    board_copy.make_move(move);

    // shah test
    if (board_copy.test_shah(active.color()))
    {
        return false;
    }
    return true;
}

Xy Board::some_target(Xy from) const
{
    if (color(from) == order()) // optimization
    {
        THECHESS_XY_FOREACH (to)
        {
            if (test_move(Move(from, to)))
            {
                return to;
            }
        }
    }
    return xy_null;
}

Xy Board::some_source(Xy to) const
{
    THECHESS_XY_FOREACH (from)
    {
        if (test_move(Move(from, to)))
        {
            return from;
        }
    }
    return xy_null;
}

Move Board::some_move() const
{
    THECHESS_XY_FOREACH (from)
    {
        Xy to = some_target(from);
        if (to != xy_null)
        {
            return Move(from, to);
        }
    }
    return move_null;
}

Xy Board::find_king(Color c) const
{
    THECHESS_XY_FOREACH (xy)
    {
        if (field(xy) == Field(c, king))
        {
            return xy;
        }
    }
    return xy_null;
}

bool Board::test_takes(const Move move) const
{
    return isset(move.to()) ||
    (chessman(move.from()) == pawn && move.from().x() != move.to().x());
}

bool Board::test_castling(const Move move) const
{
    return chessman(move.from()) == king && abs(move.dx()) == 2;
}

void Board::fen_pieces(std::ostream& out) const
{
    for (Yname y=y_8; y >= y_1; y = (Yname)((int)y - 1))
    {
        if (y < y_8)
            out << '/';
        int empty_fields = 0;
        THECHESS_X_FOREACH (x)
        {
            Xy xy(x, y);
            if (isset(xy))
            {
                if (empty_fields)
                {
                    out << empty_fields;
                    empty_fields = 0;
                }
                char piece = chessman_char(chessman(xy));
                if (color(xy) == black)
                    piece = tolower(piece);
                out << piece;
            }
            else
                empty_fields += 1;
        }
        if (empty_fields)
            out << empty_fields;
    }
}

void Board::fen_castling(std::ostream& out) const
{
    bool castling_availability = false;
    if (castling(Xy(x_h, y_1)))
    {
        out << 'K';
        castling_availability = true;
    }
    if (castling(Xy(x_a, y_1)))
    {
        out << 'Q';
        castling_availability = true;
    }
    if (castling(Xy(x_h, y_8)))
    {
        out << 'k';
        castling_availability = true;
    }
    if (castling(Xy(x_a, y_8)))
    {
        out << 'q';
        castling_availability = true;
    }
    if (!castling_availability)
        out << '-';
}

void Board::fen(std::ostream& out, int halfmove, int fullmove) const
{
    fen_pieces(out);
    out << ' ';
    out << (order() == white ? 'w' : 'b');
    out << ' ';
    fen_castling(out);
    out << ' ';
    if (long_pawn())
        out << x_char(long_pawn_x()) << (order() == white ? '6' : '3');
    else
        out << '-';
    out << ' ';
    out << halfmove;
    out << ' ';
    out << fullmove;
}

}
}

