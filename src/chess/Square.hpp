/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

//
#ifndef THECHESS_SQUARE_H_
#define THECHESS_SQUARE_H_

#include <string>

namespace thechess {

class Square;

const char* const LETTERS_ABC = "abcdefgh";
const char* const LETTERS_123 = "12345678";

/** File of square */
enum File {
    FILE_END = 8, /**< Dummy member for itarators */
    FILE_A = 0, /**< File a (left most or queenside) */
    FILE_1 = 0, /**< File a (left most or queenside) */
    FILE_B = 1, /**< File b */    FILE_2 = 1, /**< File b */
    FILE_C = 2, /**< File c */    FILE_3 = 2, /**< File c */
    FILE_D = 3, /**< File d */    FILE_4 = 3, /**< File d */
    FILE_E = 4, /**< File e */    FILE_5 = 4, /**< File e */
    FILE_F = 5, /**< File f */    FILE_6 = 5, /**< File f */
    FILE_G = 6, /**< File g */    FILE_7 = 6, /**< File g */
    FILE_H = 7, /**< File h (right most or kingside) */
    FILE_8 = 7, /**< File h (right most or kingside) */
};

/** Rank of square */
enum Rank {
    RANK_END = 8, /**< Dummy member for itarators */
    RANK_1 = 0, /**< Rank 1 (White's first rank) */
    RANK_2 = 1, /**< Rank 2 */
    RANK_3 = 2, /**< Rank 3 */
    RANK_4 = 3, /**< Rank 4 */
    RANK_5 = 4, /**< Rank 5 */
    RANK_6 = 5, /**< Rank 6 */
    RANK_7 = 6, /**< Rank 7 */
    RANK_8 = 7  /**< Rank 8 (Black's first rank) */
};

/** Return letter (\c char) of the file */
inline char file_char(File file) {
    return LETTERS_ABC[(int)file];
}

/** Return number (\c char) of the rank */
inline char rank_char(Rank rank) {
    return LETTERS_123[(int)rank];
}

/** Square representation.
Internal representation is i().
7 bits are used.
*/
class Square {
public:
    /** Construct a square from a file and a rank */
    Square(File file, Rank rank);

    /** Construct a square from a file and a rank.
    Similar to \code Square((File)file, (Rank)rank) \endcode
    */
    Square(unsigned file, unsigned rank);

    /** Construct a square from an integer.
    \param i the value returned by i().
    This constructor is used for rage creation.
    */
    Square(unsigned i);

    /** Default constructor.
    Similar to \code Square(SQUARE_NULL) \endcode
    */
    Square();

    /** Comparison operator. */
    bool operator==(const Square& square1) const {
        return i() == square1.i();
    }

    /** Comparison operator. */
    bool operator!=(const Square& square1) const {
        return i() != square1.i();
    }

    /** Get the file */
    File file() const {
        return (File)file_();
    }

    /** Get the rank */
    Rank rank() const {
        return (Rank)rank_();
    }

    /** Set the file */
    void file(File v) {
        file_((unsigned)v);
    }

    /** Set the rank */
    void rank(Rank v) {
        rank_((unsigned)v);
    }

    /** Get internal representation.
    \code i = 8 * file + rank \endcode
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
    unsigned file_() const {
        return i_ / 8;
    }

    /** Return rank as an integer.
    \todo Remove this method
    */
    unsigned rank_() const {
        return i_ % 8;
    }

    /** Set the file using \c unsigned-represented file.
    \todo Rename this method to file().
    */
    void file_(unsigned v) {
        i_ = v * 8 + rank_();
    }

    /** Set the rank using \c unsigned-represented rank.
    \todo Rename this method to rank().
    */
    void rank_(unsigned v) {
        i_ = file_() * 8 + v;
    }

    /** Get the first square for iteration by i() */
    static Square begin() {
        return Square(0);
    };

    /** Get the end square for iteration by i() */
    static Square end() {
        return Square(64);
    };

    /** Increment operator for iteration by i() */
    Square& operator++ () {
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
    \sa HalfMove::san
    */
    std::string str() const {
        return std::string(1, LETTERS_ABC[file_()]) +
               std::string(1, LETTERS_123[rank_()]);
    }

private:
    unsigned i_ : 7; // 0-63
};

/** Undefined square.
*/
const Square SQUARE_NULL(64);

/** Iterate squares.
Usage:
\code
THECHESS_SQUARE_FOREACH (square) {
    std::cout << square.str() << std::endl;
}
\endcode
*/
#define THECHESS_SQUARE_FOREACH(square) \
for (thechess::Square square = thechess::Square::begin(); \
square != thechess::Square::end(); ++square)

/** Iterate files.
Usage:
\code
THECHESS_FILE_FOREACH (file) {
    std::cout << file_char(file) << std::endl;
}
\endcode
*/
#define THECHESS_FILE_FOREACH(file) \
for (thechess::File file = thechess::FILE_A; \
file != thechess::FILE_END; \
file = (thechess::File)((int)file + 1))

/** Iterate ranks.
Usage:
\code
THECHESS_RANK_FOREACH (rank) {
    std::cout << rank_char(rank) << std::endl;
}
\endcode
*/
#define THECHESS_RANK_FOREACH(rank) \
for (thechess::Rank rank = thechess::RANK_1; \
rank != thechess::RANK_END; \
rank = (thechess::Rank)((int)rank + 1))

}

#endif

