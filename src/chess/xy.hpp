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

/** File of square */
enum Xname {
    x_end = 8, /**< Dummy member for itarators */
    x_a = 0, /**< File a (left most or queenside) */
    x_1 = 0, /**< File a (left most or queenside) */
    x_b = 1, /**< File b */    x_2 = 1, /**< File b */
    x_c = 2, /**< File c */    x_3 = 2, /**< File c */
    x_d = 3, /**< File d */    x_4 = 3, /**< File d */
    x_e = 4, /**< File e */    x_5 = 4, /**< File e */
    x_f = 5, /**< File f */    x_6 = 5, /**< File f */
    x_g = 6, /**< File g */    x_7 = 6, /**< File g */
    x_h = 7, /**< File h (right most or kingside) */
    x_8 = 7, /**< File h (right most or kingside) */
};

/** Rank of square */
enum Yname {
    y_end = 8, /**< Dummy member for itarators */
    y_1 = 0, /**< Rank 1 (White's first rank) */
    y_2 = 1, /**< Rank 2 */
    y_3 = 2, /**< Rank 3 */
    y_4 = 3, /**< Rank 4 */
    y_5 = 4, /**< Rank 5 */
    y_6 = 5, /**< Rank 6 */
    y_7 = 6, /**< Rank 7 */
    y_8 = 7  /**< Rank 8 (Black's first rank) */
};

/** Return letter (\c char) of the file */
inline char x_char(Xname x) {
    return letters_abc[(int)x];
}

/** Return number (\c char) of the rank */
inline char y_char(Yname y) {
    return letters_123[(int)y];
}

/** Square representation.
Internal representation is i().
7 bits are used.
*/
class Xy {
public:
    /** Construct a square from a file and a rank */
    Xy(Xname x, Yname y);

    /** Construct a square from a file and a rank.
    Similar to \code Xy((Xname)x, (Yname)y) \endcode
    */
    Xy(unsigned x, unsigned y);

    /** Construct a square from an integer.
    \param i the value returned by i().
    This constructor is used for rage creation.
    */
    Xy(unsigned i);

    /** Default constructor.
    Similar to \code Xy(xy_null) \endcode
    */
    Xy();

    /** Comparison operator. */
    bool operator==(const Xy& xy1) const {
        return i() == xy1.i();
    }

    /** Comparison operator. */
    bool operator!=(const Xy& xy1) const {
        return i() != xy1.i();
    }

    /** Get the file */
    Xname x() const {
        return (Xname)x_();
    }

    /** Get the rank */
    Yname y() const {
        return (Yname)y_();
    }

    /** Set the file */
    void x(Xname v) {
        x_((unsigned)v);
    }

    /** Set the rank */
    void y(Yname v) {
        y_((unsigned)v);
    }

    /** Get internal representation.
    \code i = 8 * x + y \endcode
    */
    unsigned i() const {
        return i_;
    }

    /** Set internal representation.
    \sa i()
    */
    void i(unsigned v) {
        i_ = v;
    }

    /** Return file as an integer.
    \todo Remove this method
    */
    unsigned x_() const {
        return i_ / 8;
    }

    /** Return rank as an integer.
    \todo Remove this method
    */
    unsigned y_() const {
        return i_ % 8;
    }

    /** Set the file using \c unsigned-represented file.
    \todo Rename this method to x().
    */
    void x_(unsigned v) {
        i_ = v * 8 + y_();
    }

    /** Set the rank using \c unsigned-represented rank.
    \todo Rename this method to y().
    */
    void y_(unsigned v) {
        i_ = x_() * 8 + v;
    }

    /** Get the first square for iteration by i() */
    static Xy begin() {
        return Xy(0);
    };

    /** Get the end square for iteration by i() */
    static Xy end() {
        return Xy(64);
    };

    /** Increment operator for iteration by i() */
    Xy& operator++ () {
        i_ += 1;
        return *this;
    }

    /** Convert to \c int.
    Return i().
    */
    operator int() const {
        return i_;
    }

    /** Convert to \c unsigned.
    Return i().
    */
    operator unsigned() const {
        return i_;
    }

    /** String representation.
    File letter is concatenated with rank letter.
    \sa Move::san
    */
    std::string str() const {
        return std::string(1, letters_abc[x_()]) +
               std::string(1, letters_123[y_()]);
    }

protected:
    unsigned i_ : 7; // 0-63
};

/** Undefined square.
*/
const Xy xy_null(64);

/** Iterate squares.
Usage:
\code
THECHESS_XY_FOREACH (xy) {
    std::cout << xy.str() << std::endl;
}
\endcode
*/
#define THECHESS_XY_FOREACH(xy) \
for (thechess::chess::Xy xy = thechess::chess::Xy::begin(); \
xy != thechess::chess::Xy::end(); ++xy)

/** Iterate files.
Usage:
\code
THECHESS_X_FOREACH (x) {
    std::cout << x_char(x) << std::endl;
}
\endcode
*/
#define THECHESS_X_FOREACH(x) \
for (thechess::chess::Xname x = thechess::chess::x_a; \
x != thechess::chess::x_end; \
x = (thechess::chess::Xname)((int)x + 1))

/** Iterate ranks.
Usage:
\code
THECHESS_Y_FOREACH (y) {
    std::cout << y_char(y) << std::endl;
}
\endcode
*/
#define THECHESS_Y_FOREACH(y) \
for (thechess::chess::Yname y = thechess::chess::y_1; \
y != thechess::chess::y_end; \
y = (thechess::chess::Yname)((int)y + 1))

}
}

#endif // THECHESS_XY_H_

