/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_STAGED_COMPETITION_GRAPH_HPP_
#define THECHESS_WIDGETS_STAGED_COMPETITION_GRAPH_HPP_

#include <Wt/Wc/FilterResource.hpp>

#include "model/global.hpp"

namespace thechess {

/** Graphviz-generated graph for a staged competition */
class StagedCompetitionGraph : public Wt::Wc::FilterResource {
public:
    /** Constructor.
    \note The pointer to StagedCompetition must be valid all the time.
    */
    StagedCompetitionGraph(const StagedCompetition* comp, Wt::WObject* p = 0);

    /** Output competition tree in DOT format (graphviz) */
    void write_input(std::ostream& out) const;

private:
    const StagedCompetition* comp_;
};

}

#endif

