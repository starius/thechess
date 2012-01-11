/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_STAGED_COMPETITION_GRAPH_HPP_
#define THECHESS_WIDGETS_STAGED_COMPETITION_GRAPH_HPP_

#include <Wt/WFileResource>

#include "model/all.hpp"

namespace thechess {

// FIXME The file can not be downloaded
// http://redmine.emweb.be/issues/1127
/** Graphviz-generated graph for a staged competition */
class StagedCompetitionGraph : public Wt::WFileResource {
public:
    /** Constructor.
    Create temporary image file using \c system().
    Command \c dot must be available.
    */
    StagedCompetitionGraph(const StagedCompetition* comp, Wt::WObject* p = 0);

    /** Delete temrary file */
    ~StagedCompetitionGraph();

private:
    std::string png_file_;
};

}

#endif

