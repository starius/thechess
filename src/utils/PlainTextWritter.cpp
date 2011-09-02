/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include "utils/PlainTextWritter.hpp"

namespace thechess {

PlainTextWritter::PlainTextWritter(std::ostream& out, const short cpl):
    out_(out), cpl_(cpl), current_cpl_(0)
{ }

PlainTextWritter::~PlainTextWritter() {
    out_ << std::endl;
}

void PlainTextWritter::write_word(const std::string& word) {
    if (current_cpl_ + word.length() > cpl_) {
        out_ << std::endl;
        current_cpl_ = 0;
    } else if (current_cpl_) {
        out_ << ' ';
        current_cpl_ += 1;
    }
    out_ << word;
    current_cpl_ += word.length();
}

}

