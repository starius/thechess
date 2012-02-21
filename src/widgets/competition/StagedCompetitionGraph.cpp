/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/competition/StagedCompetitionGraph.hpp"
#include "model/all.hpp"

namespace thechess {

StagedCompetitionGraph::StagedCompetitionGraph(const StagedCompetition* comp,
        Wt::WObject* p):
    Wt::Wc::FilterResource("dot -Tpng {1} -o {2}", p),
    comp_(comp) {
    setMimeType("image/png");
}

void StagedCompetitionGraph::write_input(std::ostream& out) const {
    comp_->dot(out);
}

}

