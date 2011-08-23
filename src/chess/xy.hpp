/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

//
#ifndef THECHESS_XY_H_
#define THECHESS_XY_H_

#include <string>

namespace thechess {
namespace chess {

class Xy;

const char* const letters_abc = "abcdefgh";
const char* const letters_123 = "12345678";

enum Xname {
    x_end = 8,
    x_a = 0,    x_1 = 0,
    x_b = 1,    x_2 = 1,
    x_c = 2,    x_3 = 2,
    x_d = 3,    x_4 = 3,
    x_e = 4,    x_5 = 4,
    x_f = 5,    x_6 = 5,
    x_g = 6,    x_7 = 6,
    x_h = 7,    x_8 = 7
};

enum Yname {
    y_end = 8,
    y_1 = 0,
    y_2 = 1,
    y_3 = 2,
    y_4 = 3,
    y_5 = 4,
    y_6 = 5,
    y_7 = 6,
    y_8 = 7
};


inline char x_char(Xname x) {
    return letters_abc[(int)x];
}

inline char y_char(Yname y) {
    return letters_123[(int)y];
}

class Xy {
public:
    Xy(Xname x, Yname y);
    Xy(unsigned x, unsigned y);
    Xy(unsigned i); // for rage creation
    Xy(); // no xy
    bool operator==(const Xy& xy1) const {
        return i() == xy1.i();
    }
    bool operator!=(const Xy& xy1) const {
        return i() != xy1.i();
    }

    Xname x() const {
        return (Xname)x_();
    }
    Yname y() const {
        return (Yname)y_();
    }
    void x(Xname v) {
        x_((unsigned)v);
    }
    void y(Yname v) {
        y_((unsigned)v);
    }

    unsigned i() const {
        return i_;
    }
    void i(unsigned v) {
        i_ = v;
    }

    unsigned x_() const {
        return i_ / 8;
    }
    unsigned y_() const {
        return i_ % 8;
    }
    void x_(unsigned v) {
        i_ = v * 8 + y_();
    }
    void y_(unsigned v) {
        i_ = x_() * 8 + v;
    }

    static Xy begin() {
        return Xy(0);
    };
    static Xy end() {
        return Xy(64);
    };
    Xy& operator++ () {
        i_ += 1;
        return *this;
    }

    operator int() const {
        return i_;
    }
    operator unsigned() const {
        return i_;
    }

    std::string str() const {
        return std::string(1, letters_abc[x_()]) +
               std::string(1, letters_123[y_()]);
    }

protected:
    unsigned i_ : 7; // 0-63
};

const Xy xy_null(64);


#define THECHESS_XY_FOREACH(xy) \
for (thechess::chess::Xy xy = thechess::chess::Xy::begin(); \
xy != thechess::chess::Xy::end(); ++xy)

#define THECHESS_X_FOREACH(x) \
for (thechess::chess::Xname x = thechess::chess::x_a; \
x != thechess::chess::x_end; \
x = (thechess::chess::Xname)((int)x + 1))

#define THECHESS_Y_FOREACH(y) \
for (thechess::chess::Yname y = thechess::chess::y_1; \
y != thechess::chess::y_end; \
y = (thechess::chess::Yname)((int)y + 1))

}
}

#endif // THECHESS_XY_H_

