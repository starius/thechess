/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_STAGED_COMPETITION_HPP_
#define THECHESS_MODEL_STAGED_COMPETITION_HPP_

#include <map>

#include "model/all.hpp"

namespace thechess {

class UserPair {
public:
    UserPair();
    UserPair(const UserPtr& first, const UserPtr& second);

    bool operator<(const UserPair& other) const;
    bool operator==(const UserPair& other) const;
    bool operator!=(const UserPair& other) const;
    operator bool() const {
        return first_ && second_;
    }
    const UserPtr& first() const {
        return first_;
    }
    const UserPtr& second() const {
        return second_;
    }

private:
    UserPtr first_;
    UserPtr second_;
};

class StagedCompetition {
public:
    enum State {
        LOSER, PAIRED, WINNER, UNPAIRED
    };

    typedef std::map<UserPtr, State> States;
    typedef std::map<UserPtr, int> Stages;
    typedef std::multimap<int, UserPair> Paires;
    typedef std::map<UserPair, GamesVector> Games;
    typedef std::map<UserPair, UserPtr> Winners;

    StagedCompetition(const Competition* competition);
    void process(Competition* competition, Objects& objects);

    const States& states() const {
        return states_;
    }
    const Stages& stages() const {
        return stages_;
    }
    const Paires& paires() const {
        return paires_;
    }
    const Games& games() const {
        return games_;
    }
    const Winners& winners() const {
        return winners_;
    }
    const UserPtr& winner() const {
        return winner_;
    }

private:
    const Competition* competition_;

    States states_;
    Stages stages_;
    Paires paires_;
    Games games_;
    Winners winners_;

    int winner_stage_;
    UserPtr winner_;

    void read_games_();
    void read_paires_();
    void read_pair_(int stage, const UserPair& pair);

    void start_competition_();
    void join_users_();
    void create_games_(Competition* competition, Objects& objects);
};

}

#endif

