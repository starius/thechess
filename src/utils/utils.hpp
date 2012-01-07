/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_UTILS_HPP_
#define THECHESS_UTILS_HPP_

#include <string>

namespace thechess {

/** Expand path like a posix-shell */
std::string expand_path(const std::string& path);

/** Return first existing file */
std::string first_file(const char* const files[], const int files_size);

/** Convert \c char to \c std::string */
inline std::string char2str(char c) {
    return std::string(1, c);
}

}

#endif

