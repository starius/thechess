/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "chess/Square.hpp"
#include "utils/utils.hpp"

namespace thechess {

const char* const LETTERS_ABC = "abcdefgh";
const char* const LETTERS_123 = "12345678";
const int SQUARE_NULL = 64;

Square::Square(Square::File file, Square::Rank rank) :
    i_((unsigned)file * 8 + (unsigned)rank) {
}

Square::Square(unsigned file, unsigned rank) :
    i_((unsigned)file * 8 + (unsigned)rank) {
}

Square::Square(unsigned i) :
    i_(i) {
}

Square::Square() :
    i_(SQUARE_NULL) {
}

char Square::file_char(Square::File file) {
    return LETTERS_ABC[(int)file];
}

char Square::file_char() const {
    return file_char(file());
}

char Square::rank_char(Square::Rank rank) {
    return LETTERS_123[(int)rank];
}

char Square::rank_char() const {
    return rank_char(rank());
}

std::string Square::str() const {
    return char2str(file_char()) + char2str(rank_char());
}

bool Square::operator==(const Square& square1) const {
    return i() == square1.i();
}

bool Square::operator!=(const Square& square1) const {
    return i() != square1.i();
}

Square::operator bool() const {
    return *this != Square();
}

Square::File Square::file() const {
    return (File)file_int();
}

Square::Rank Square::rank() const {
    return (Rank)rank_int();
}

void Square::file(Square::File v) {
    file((unsigned)v);
}

void Square::rank(Square::Rank v) {
    rank((unsigned)v);
}

unsigned Square::file_int() const {
    return i_ / 8;
}

unsigned Square::rank_int() const {
    return i_ % 8;
}

void Square::file(unsigned v) {
    i_ = v * 8 + rank_int();
}

void Square::rank(unsigned v) {
    i_ = file_int() * 8 + v;
}

Square Square::begin() {
    return Square(0);
};

Square Square::end() {
    return Square(64);
};

Square& Square::operator++ () {
    i_ += 1;
    return *this;
}

}

