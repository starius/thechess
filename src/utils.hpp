#ifndef THECHESS_UTILS_HPP_
#define THECHESS_UTILS_HPP_

#include <string>

namespace thechess
{

std::string expand_path(const std::string& path);
std::string first_file(const char* const files[], const int files_size);

}

#endif

