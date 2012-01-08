/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <algorithm>
#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include <Wt/Wc/rand.hpp>

#include "model/all.hpp"

namespace thechess {

UserPair::UserPair() {
}

UserPair::UserPair(const UserPtr& first, const UserPtr& second) {
    BOOST_ASSERT(first != second);
    if (first < second) {
        first_ = first;
        second_ = second;
    } else {
        first_ = second;
        second_ = first;
    }
}

bool UserPair::operator<(const UserPair& other) const {
    return first_ < other.first() || (first_ == other.first() && second_ < other.second());
}

bool UserPair::operator==(const UserPair& other) const {
    return first_ == other.first() && second_ == other.second();
}

bool UserPair::operator!=(const UserPair& other) const {
    return !(*this == other);
}

StagedCompetition::StagedCompetition(const Competition* competition):
    competition_(competition) {
    BOOST_ASSERT(competition->type() == STAGED);
    winner_stage_ = ceil(log2(double(competition->members().size()))) + 0.5;
    read_games();
    read_paires();
}

void StagedCompetition::process(Competition* competition, Planning* planning) {
    BOOST_ASSERT(competition->id() == competition_->id());
    if (games_.empty() && competition->state() == Competition::ACTIVE) {
        start_competition();
    }
    join_users();
    create_games_(competition, planning);
}

void StagedCompetition::dot(std::ostream& out) const {
    Competitiors comp = competitors();
    out << "digraph compatition_" << competition_->id() << " {" << std::endl;
    typedef std::map<UserPtr, UserPtr> U2U;
    typedef std::map<int, U2U> Stage2Users;
    Stage2Users s2u;
    BOOST_FOREACH (const Paires::value_type& stage_and_pair, paires_) {
        int stage = stage_and_pair.first;
        const UserPair& pair = stage_and_pair.second;
        UserPtr winner;
        if (winners_.find(pair) != winners_.end()) {
            winner = winners_.find(pair)->second;
        }
        s2u[stage][pair.first()] = winner;
        s2u[stage][pair.second()] = winner;
    }
    BOOST_FOREACH (const Stages::value_type& user_and_stage, stages_) {
        const UserPtr& user = user_and_stage.first;
        int stage = user_and_stage.second;
        if (stage == 1) {
            s2u[stage - 1][user] = user;
        }
    }
    BOOST_FOREACH (const Stage2Users::value_type& stage_and_u2u, s2u) {
        int stage = stage_and_u2u.first;
        int stage_number = stage + 1;
        const U2U& u2u = stage_and_u2u.second;
        out << "subgraph stage_" << stage_number << " {" << std::endl;
        BOOST_FOREACH (const U2U::value_type& user_to_user, u2u) {
            const UserPtr& from = user_to_user.first;
            const UserPtr& to = user_to_user.second;
            out << "user_" << from.id() << "_stage_" << stage_number;
            out << "[label='" << from->username() << "']" << std::endl;
            out << "user_" << from.id() << "_stage_" << stage_number << " -> ";
            if (to) {
                out << "user_" << to.id() << "_stage_" << (stage_number + 1);
            } else {
                UserPair pair(from, comp[stage][from]);
                int id = pair.first().id();
                out << "empty_" << id << "_stage_" << (stage_number + 1);
            }
            out << std::endl;
        }
        if (s2u.find(stage - 1) != s2u.end()) {
            BOOST_FOREACH (const U2U::value_type& user_to_user, s2u[stage - 1]) {
                const UserPtr& from = user_to_user.first;
                const UserPtr& to = user_to_user.second;
                if (!to) {
                    UserPair pair(from, comp[stage][from]);
                    int id = pair.first().id();
                    out << "empty_" << id << "_stage_" << stage_number;
                }
            }
        }
        out << "}" << std::endl;
    }
    out << "}" << std::endl;
}

StagedCompetition::Competitiors StagedCompetition::competitors() const {
    Competitiors r;
    BOOST_FOREACH (const Paires::value_type& stage_and_pair, paires_) {
        int stage = stage_and_pair.first;
        const UserPair& pair = stage_and_pair.second;
        r[stage][pair.first()] = pair.second();
        r[stage][pair.second()] = pair.first();
    }
    return r;
}

void StagedCompetition::read_games() {
    BOOST_FOREACH (const GamePtr& game, competition_->games_vector()) {
        UserPair pair(game->white(), game->black());
        int stage = game->competition_stage();
        if (games_.find(pair) == games_.end()) {
            paires_.insert(std::make_pair(stage, pair));
        }
        games_[pair].push_back(game);
    }
}

void StagedCompetition::read_paires() {
    BOOST_FOREACH (const UserPtr& user, competition_->members_vector()) {
        states_[user] = UNPAIRED;
        stages_[user] = 1; // if he was paired in 0 stage, it will be overwritten
    }
    BOOST_FOREACH (Paires::value_type& stage_and_pair, paires_) {
        int stage = stage_and_pair.first;
        UserPair pair = stage_and_pair.second;
        read_pair_(stage, pair);
    }
}

void StagedCompetition::read_pair_(int stage, const UserPair& pair) {
    states_[pair.first()] = PAIRED;
    states_[pair.second()] = PAIRED;
    stages_[pair.first()] = stage;
    stages_[pair.second()] = stage;
    if (Competition::all_ended(games_[pair])) {
        UsersVector winners = Competition::winners_of_games(games_[pair]);
        if (winners.size() == 1) {
            states_[pair.first()] = LOSER;
            states_[pair.second()] = LOSER;
            const UserPtr& winner = winners[0];
            winners_[pair] = winner;
            states_[winner] = UNPAIRED;
            stages_[winner] = stage + 1;
            if (stage + 1 == winner_stage_) {
                BOOST_ASSERT(!winner_);
                states_[winner] = WINNER;
                winner_ = winner;
            }
        }
    }
}

void StagedCompetition::start_competition() {
    UsersVector members = competition_->members_vector();
    std::random_shuffle(members.begin(), members.end(),
                        Wt::Wc::rand_for_shuffle);
    int members_size = members.size();
    int max_pow2 = pow(2, floor(log2(members_size))) + 0.5;
    int pairs = members_size - max_pow2 || members_size / 2;
    for (int i = 0; i < pairs; i++) {
        const UserPtr& first = members[2 * i];
        const UserPtr& second = members[2 * i + 1];
        states_[first] = PAIRED;
        states_[second] = PAIRED;
        stages_[first] = 0;
        stages_[second] = 0;
        paires_.insert(std::make_pair(0, UserPair(first, second)));
    }
}

void StagedCompetition::join_users() {
    typedef std::map<int, UsersVector> Unpaired;
    Unpaired unpaired;
    BOOST_FOREACH (States::value_type& user_and_state, states_) {
        if (user_and_state.second == UNPAIRED) {
            unpaired[stages_[user_and_state.first]].push_back(user_and_state.first);
        }
    }
    BOOST_FOREACH (Unpaired::value_type& stage_and_users, unpaired) {
        int stage = stage_and_users.first;
        UsersVector& users = stage_and_users.second;
        std::random_shuffle(users.begin(), users.end(),
                            Wt::Wc::rand_for_shuffle);
        while (users.size() >= 2) {
            UserPtr first = users.back();
            users.pop_back();
            UserPtr second = users.back();
            users.pop_back();
            states_[first] = PAIRED;
            states_[second] = PAIRED;
            paires_.insert(std::make_pair(stage, UserPair(first, second)));
        }
    }
}

void StagedCompetition::create_games_(Competition* competition,
                                      Planning* planning) {
    BOOST_FOREACH (Paires::value_type& stage_and_pair, paires_) {
        int stage = stage_and_pair.first;
        UserPair pair = stage_and_pair.second;
        if (!winners_[pair] && Competition::all_ended(games_[pair])) {
            bool no_draw = static_cast<int>(games_[pair].size()) ==
                           competition->cp()->min_substages() + stage *
                           competition->cp()->increment_substages();
            int n = no_draw ? 1 : 2;
            for (int i = 0; i < n; i++) {
                const UserPtr& white = i ? pair.first() : pair.second();
                const UserPtr& black = i ? pair.second() : pair.first();
                GamePtr game = competition->create_game_(white, black, stage, no_draw);
                games_[pair].push_back(game);
                planning->add(new Object(GAME, game.id()), now(), false);
            }
            planning->add(new Object(USER, pair.first().id()), now(), false);
            planning->add(new Object(USER, pair.second().id()), now(), false);
        }
    }
}

}

