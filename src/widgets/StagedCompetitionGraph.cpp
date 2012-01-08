/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#define BOOST_FILESYSTEM_VERSION 3

#include <fstream>
#include <cstdio>
#include <boost/filesystem.hpp>

#include "widgets/StagedCompetitionGraph.hpp"

namespace thechess {

StagedCompetitionGraph::StagedCompetitionGraph(const StagedCompetition* comp,
        Wt::WObject* p):
    Wt::WFileResource(p) {
    using namespace boost::filesystem;
    const char* const dot_m = "thechess-%%%%-%%%%-%%%%-%%%%.dot";
    const char* const png_m = "thechess-%%%%-%%%%-%%%%-%%%%.png";
    std::string dot_file = unique_path(temp_directory_path() / dot_m).string();
    png_file_ = unique_path(temp_directory_path() / png_m).string();
    std::ofstream dot(dot_file.c_str());
    if (dot.is_open()) {
        comp->dot(dot);
        dot.close();
        std::string cmd("dot -Tpng ");
        cmd += dot_file + " -o " + png_file_;
        system(cmd.c_str());
        remove(dot_file.c_str());
        if (exists(png_file_)) {
            setFileName(png_file_);
        }
    }
    setMimeType("image/png");
}

StagedCompetitionGraph::~StagedCompetitionGraph() {
    remove(png_file_.c_str());
}

}

