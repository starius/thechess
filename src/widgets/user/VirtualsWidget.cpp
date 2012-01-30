/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <map>
#include <set>
#include <boost/foreach.hpp>

#include <Wt/Wc/Gather.hpp>

#include "widgets/user/VirtualsWidget.hpp"
#include "model/staged_competition.hpp" // for UserPair
#include "Application.hpp"

namespace thechess {

VirtualsWidget::VirtualsWidget(const dbo::Query<BD::BDPair>& pairs,
                               Wt::WObject* parent):
    Wt::Wc::FilterResource("dot -Tpng {1} -o {2}", parent), pairs_(pairs) {
    setMimeType("image/png");
}

void VirtualsWidget::write_input(std::ostream& out) const {
    Wt::WApplication::UpdateLock lock(wApp);
    if (lock) {
        dbo::Transaction t(tApp->session());
        typedef std::map<UserPair, int> Scores;
        Scores scores;
        std::set<UserPtr> users;
        dbo::collection<BD::BDPair> pairs = pairs_;
        BOOST_FOREACH (BD::BDPair pair, pairs) {
            UserPtr a = pair.get<0>()->user();
            UserPtr b = pair.get<1>()->user();
            Wt::Wc::Gather::DataType type = pair.get<0>()->type();
            int score = Wt::Wc::Gather::significance(type);
            scores[UserPair(a, b)] += score;
            users.insert(a);
            users.insert(b);
        }
        out << "graph virtuals {" << std::endl;
        BOOST_FOREACH (UserPtr user, users) {
            out << user.id();
            out << "[label=\"" << user->username() << "\"]" << std::endl;
        }
        BOOST_FOREACH (Scores::value_type& pair_and_score, scores) {
            int score = pair_and_score.second;
            const UserPair& pair = pair_and_score.first;
            const UserPtr a = pair.first();
            const UserPtr b = pair.second();
            if (score > Wt::Wc::Gather::MIN_SIGNIFICANT) {
                out << a.id() << "--" << b.id() << std::endl;
            }
        }
        out << "}" << std::endl;
        t.commit();
    }
}

}

