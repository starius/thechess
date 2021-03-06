/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <stdlib.h>
#include <fstream>
#include <wordexp.h>

#include "utils/utils.hpp"

namespace thechess {

std::string expand_path(const std::string& path) {
    wordexp_t exp_result;
    wordexp(path.c_str(), &exp_result, 0);
    std::string result(exp_result.we_wordv[0]);
    wordfree(&exp_result);
    return result;
}

std::string first_file(const char* const files[], const int files_size) {
    for (int i = 0; i < files_size; i++) {
        std::string filename = expand_path(files[i]);
        if (std::ifstream(filename.c_str())) {
            return filename;
        }
    }
    return 0;
}

}

