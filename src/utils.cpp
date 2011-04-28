#include <stdlib.h>
#include <wordexp.h>

#include "utils.hpp"

namespace thechess
{

std::string expand_path(const std::string& path)
{
    wordexp_t exp_result;
    wordexp(path.c_str(), &exp_result, 0);
    std::string result(exp_result.we_wordv[0]);
    wordfree(&exp_result);
    return result;
}

}

