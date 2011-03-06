//
#ifndef FIELD_H_
#define FIELD_H_

#include <boost/format.hpp>

#include <Wt/WString>

namespace thechess {
 namespace chess {

enum Color
{
    white = 0,
    black = 1,
    color_null = 2
};

const int Color_count = 3;

enum Chessman
{
    chessman_null = 0,
    bishop = 1, // слон
    king   = 2,
    knight = 3, // конь
    pawn   = 4, // пешка
    queen  = 5,
    rock   = 6  // ладья
};

const int Chessman_count = 7;

//~ inline bool operator !(const Color& c) { return (Color)((bool)c); }

Wt::WString color2str(Color color);

inline Color other_color(Color color)
{
    return color == white ? black : white;
}

class Field // Color & Chessman
{
public:
    Field(Color color, Chessman chessman);
    bool operator==(const Field& field) const
        { return color() == field.color() && chessman() == field.chessman(); }
    bool operator!=(const Field& field) const
        { return color() != field.color() || chessman() != field.chessman(); }

    Color color() const { return color_; }
    Chessman chessman() const { return chessman_; }
    void color(Color v) { color_ = v; }
    void chessman(Chessman v) { chessman_ = v; }


private:
    Color color_;
    Chessman chessman_;
};

}
}

#endif // FIELD_H_
