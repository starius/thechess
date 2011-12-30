/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_SQUARE_H_
#define THECHESS_SQUARE_H_

#include <string>

namespace thechess {

/** Square representation.
Internal representation is i().
7 bits are used.

\ingroup chess
*/
class Square {
public:
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

    /** Create a <i>Null</i> square */
    Square();

    /** Comparison operator. */
    bool operator==(const Square& square1) const;

    /** Comparison operator. */
    bool operator!=(const Square& square1) const;

    /** Check for null */
    operator bool() const;

    /** Get the file */
    File file() const;

    /** Get the rank */
    Rank rank() const;

    /** Set the file */
    void file(File v);

    /** Set the rank */
    void rank(Rank v);

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
    unsigned file_() const;

    /** Return rank as an integer.
    \todo Remove this method
    */
    unsigned rank_() const;

    /** Set the file using \c unsigned-represented file.
    \todo Rename this method to file().
    */
    void file_(unsigned v);

    /** Set the rank using \c unsigned-represented rank.
    \todo Rename this method to rank().
    */
    void rank_(unsigned v);

    /** Get the first square for iteration by i() */
    static Square begin();

    /** Get the end square for iteration by i() */
    static Square end();

    /** Increment operator for iteration by i() */
    Square& operator++ ();

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

    /** Return letter (\c char) of the file */
    static char file_char(File file);

    /** Return letter (\c char) of the file */
    char file_char() const;

    /** Return number (\c char) of the rank */
    static char rank_char(Rank rank);

    /** Return number (\c char) of the rank */
    char rank_char() const;

    /** String representation.
    File letter is concatenated with rank letter.
    \sa HalfMove::san
    */
    std::string str() const;

private:
    unsigned i_ : 7; // 0-63
};

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
    std::cout << Square::file_char(file) << std::endl;
}
\endcode
*/
#define THECHESS_FILE_FOREACH(file) \
for (thechess::Square::File file = thechess::Square::FILE_A; \
file != thechess::Square::FILE_END; \
file = (thechess::Square::File)((int)file + 1))

/** Iterate ranks.
Usage:
\code
THECHESS_RANK_FOREACH (rank) {
    std::cout << Square::rank_char(rank) << std::endl;
}
\endcode
*/
#define THECHESS_RANK_FOREACH(rank) \
for (thechess::Square::Rank rank = thechess::Square::RANK_1; \
rank != thechess::Square::RANK_END; \
rank = (thechess::Square::Rank)((int)rank + 1))

}

#endif

