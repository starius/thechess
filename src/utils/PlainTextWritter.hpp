/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_UTILS_PLAINTEXT_WRITTER_HPP_
#define THECHESS_UTILS_PLAINTEXT_WRITTER_HPP_

#include <string>
#include <ostream>

namespace thechess {

class PlainTextWritter {
public:
    PlainTextWritter(std::ostream& out, const short cpl = 79);
    virtual ~PlainTextWritter();
    void write_word(const std::string& word);
private:
    std::ostream& out_;
    const unsigned short cpl_;
    unsigned short current_cpl_;
};

}

#endif

