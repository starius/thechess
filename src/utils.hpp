/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_UTILS_HPP_
#define THECHESS_UTILS_HPP_

#include <string>
#include <ostream>

namespace thechess {

std::string expand_path(const std::string& path);
std::string first_file(const char* const files[], const int files_size);

/** Convert \c char to \c std::string */
inline std::string char2str(char c) {
    return std::string(1, c);
}

class PlainTextWritter {
public:
    PlainTextWritter(std::ostream& out, const short cpl=79);
    void write_word(const std::string& word);
private:
    std::ostream& out_;
    const unsigned short cpl_;
    unsigned short current_cpl_;
};

}

#endif

