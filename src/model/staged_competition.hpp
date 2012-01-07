/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_STAGED_COMPETITION_HPP_
#define THECHESS_MODEL_STAGED_COMPETITION_HPP_

#include <map>

#include "model/all.hpp"
#include "Planning.hpp"

namespace thechess {

/** Pair of user pointers */
class UserPair {
public:
    /** Create null user pair */
    UserPair();

    /** Constructor.
    \attention Asserts first != second
    \note stores min value to first(), max() -- to second()
    */
    UserPair(const UserPtr& first, const UserPtr& second);

    /** Comparison operator */
    bool operator<(const UserPair& other) const;

    /** Comparison operator */
    bool operator==(const UserPair& other) const;

    /** Comparison operator */
    bool operator!=(const UserPair& other) const;

    /** Boolean operator */
    operator bool() const {
        return first_ && second_;
    }

    /** Get min of two user pointers */
    const UserPtr& first() const {
        return first_;
    }

    /** Get max of two user pointers */
    const UserPtr& second() const {
        return second_;
    }

private:
    UserPtr first_;
    UserPtr second_;
};

/** Staged competition accessor.
Instances of this are not stored to database,
but used to access staged competition-specific data of a competition
and to process scheduling tasks.

StagedCompetition doesn't reflect Competition's changes.

Stage index is 0-based.
*/
class StagedCompetition {
public:
    /** State of member */
    enum State {
        LOSER, PAIRED, WINNER, UNPAIRED
    };

    /** Map User to State */
    typedef std::map<UserPtr, State> States;

    /** Map User to stage */
    typedef std::map<UserPtr, int> Stages;

    /** MultiMap stage to UserPair */
    typedef std::multimap<int, UserPair> Paires;

    /** Map UserPair to GamesVector */
    typedef std::map<UserPair, GamesVector> Games;

    /** Map UserPair to User (winner) */
    typedef std::map<UserPair, UserPtr> Winners;

    /** Map user to user */
    typedef std::map<UserPtr, UserPtr> U2U;

    /** Competitor of the user in the stage */
    typedef std::map<int, U2U> Competitiors;

    /** Constructor */
    StagedCompetition(const Competition* competition);

    /** Process scheduling tasks */
    void process(Competition* competition, Planning* planning);

    /** Get states */
    const States& states() const {
        return states_;
    }

    /** Get stages */
    const Stages& stages() const {
        return stages_;
    }

    /** Get paires */
    const Paires& paires() const {
        return paires_;
    }

    /** Get games */
    const Games& games() const {
        return games_;
    }

    /** Get winners of user pairs */
    const Winners& winners() const {
        return winners_;
    }

    /** Get winner (if any) of the competition.
    Max number of winners of a staged competition is 1.
    */
    const UserPtr& winner() const {
        return winner_;
    }

    /** Output competition tree in DOT format (graphviz) */
    void dot(std::ostream& out) const;

    /** Return competitions */
    Competitiors competitors() const;

private:
    const Competition* competition_;

    States states_;
    Stages stages_;
    Paires paires_;
    Games games_;
    Winners winners_;

    int winner_stage_;
    UserPtr winner_;

    void read_games();
    void read_paires();
    void read_pair_(int stage, const UserPair& pair);

    void start_competition();
    void join_users();
    void create_games_(Competition* competition, Planning* planning);
};

}

#endif

