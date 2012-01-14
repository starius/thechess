/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_COMPETITION_PARAMETERS_HPP_
#define THECHESS_MODEL_COMPETITION_PARAMETERS_HPP_

#include "model/global.hpp"
#include "model/record.hpp"

namespace thechess {

/** Parameters of a competition.
This model stores abstract competition parameters, which could be used
by multiple competitions.
(Reference to) \ref GP "game parameters" is one of fields.

\ingroup model
*/
class CP : public dbo::Dbo<CP>, public Record {
public:
    /** Competition type */
    typedef CompetitionType Type;

#ifndef DOXYGEN_ONLY
    CP();
#endif

    /** Create instance to be added to database.
    \param gp Default game parameters for competitions.
    */
    CP(const GPPtr& gp);

#ifndef DOXYGEN_ONLY
    template<class Action>
    void persist(Action& a) {
        Record::persist(a);
        dbo::belongsTo(a, parent_, "parent");
        dbo::hasMany(a, children_, dbo::ManyToOne, "parent");
        dbo::belongsTo(a, gp_, "gp");
        dbo::hasMany(a, competitions_, dbo::ManyToOne, "cp");
        dbo::field(a, type_, "type");
        dbo::field(a, min_rating_, "min_rating");
        dbo::field(a, max_rating_, "max_rating");
        dbo::field(a, min_classification_, "min_classification");
        dbo::field(a, max_classification_, "max_classification");
        dbo::field(a, force_start_delay_, "force_start_delay");
        dbo::field(a, min_users_, "min_users");
        dbo::field(a, max_users_, "max_users");
        dbo::field(a, min_recruiting_time_, "min_recruiting_time");
        dbo::field(a, max_recruiting_time_, "max_recruiting_time");
        dbo::field(a, max_simultaneous_games_, "max_simultaneous_games");
        dbo::field(a, games_factor_, "games_factor");
        dbo::field(a, relax_time_, "relax_time");
        dbo::field(a, min_substages_, "min_substages");
        dbo::field(a, increment_substages_, "increment_substages");
    }
#endif

    /** Get type */
    Type type() const {
        return type_;
    }

    /** Set type */
    void set_type(Type v) {
        type_ = v;
    }

    /** Get game parameters (default for competitions) */
    const GPPtr& gp() const {
        return gp_;
    }

    /** Get parent */
    const CPPtr& parent() const {
        return parent_;
    }

    /** Set parent */
    void set_parent(const CPPtr& parent) {
        parent_ = parent;
    }

    /** \name Restrictions for users */
    /* @{ */

    /** Get min acceptable rating of members */
    int min_rating() const {
        return min_rating_;
    }

    /** Set min acceptable rating of members */
    void set_min_rating(int v) {
        min_rating_ = v;
    }

    /** Get max acceptable rating of members */
    int max_rating() const {
        return max_rating_;
    }

    /** Set max acceptable rating of members */
    void set_max_rating(int v) {
        max_rating_ = v;
    }

    /** Get min acceptable classification of members */
    Classification min_classification() const {
        return min_classification_;
    }

    /** Set min acceptable classification of members */
    void set_min_classification(Classification v) {
        min_classification_ = v;
    }

    /** Get max acceptable classification of members */
    Classification max_classification() const {
        return max_classification_;
    }

    /** Set max acceptable classification of members */
    void set_max_classification(Classification v) {
        max_classification_ = v;
    }

    /* @} */

    /** \name Parameters of CLASSICAL and STAGED competitions */
    /* @{ */

    /** Get time after which confirmed games would be started automatically */
    Td force_start_delay() const {
        return force_start_delay_;
    }

    /** Set time after which confirmed games would be started automatically */
    void set_force_start_delay(Td v) {
        force_start_delay_ = v;
    }

    /** Get min number of members competition to start */
    int min_users() const {
        return min_users_;
    }

    /** Set min number of members competition to start */
    void set_min_users(int v) {
        min_users_ = v;
    }

    /** Get max allowed number of members */
    int max_users() const {
        return max_users_;
    }

    /** Set max allowed number of members */
    void set_max_users(int v) {
        max_users_ = v;
    }

    /** Get min recruiting time competition to start */
    Td min_recruiting_time() const {
        return min_recruiting_time_;
    }

    /** Set min recruiting time competition to start */
    void set_min_recruiting_time(Td v) {
        min_recruiting_time_ = v;
    }

    /** Get time after which competition would be canceled in case not started */
    Td max_recruiting_time() const {
        return max_recruiting_time_;
    }

    /** Set time after which competition would be canceled in case not started */
    void set_max_recruiting_time(Td v) {
        max_recruiting_time_ = v;
    }

    /** Get max number of simultaneous games by user */
    int max_simultaneous_games() const {
        return max_simultaneous_games_;
    }

    /** Set max number of simultaneous games by user */
    void set_max_simultaneous_games(int v) {
        max_simultaneous_games_ = v;
    }

    /* @} */

    /** \name Parameters of CLASSICAL competition */
    /* @{ */

    /** Get factor of games number.
    Each user plays (n - 1) * games_factor() games with white pieces
    and the same number with black pieces (n is the number of users).
    */
    float games_factor() const {
        return games_factor_;
    }

    /** Set factor of games number.
    \copydetails games_factor()
    */
    void set_games_factor(float v) {
        games_factor_ = v;
    }

    /** Get time after which created games would be confirmed automatically */
    Td relax_time() const {
        return relax_time_;
    }

    /** Set time after which created games would be confirmed automatically */
    void set_relax_time(Td v) {
        relax_time_ = v;
    }

    /* @} */

    /** \name Parameters of STAGED competition */
    /* @{ */

    /** Get min number of substages played in case of draw before no-draw game.
    substages = min_substages + stage * increment_substages.
    (stage is 0-indexed.)
    */
    int min_substages() const {
        return min_substages_;
    }

    /** Set min number of substages played in case of draw before no-draw game.
    \copydetails min_substages()
    */
    void set_min_substages(int v) {
        min_substages_ = v;
    }

    /** Get increment of the number of substages per each substage.
    \copydetails min_substages()
    */
    int increment_substages() const {
        return increment_substages_;
    }

    /** Set increment of the number of substages per each substage.
    \copydetails min_substages()
    */
    void set_increment_substages(int v) {
        increment_substages_ = v;
    }

    /* @} */

private:
    CPPtr parent_;
    CPs children_;

    GPPtr gp_;
    Competitions competitions_;
    Type type_;

    int min_rating_;
    int max_rating_;
    Classification min_classification_;
    Classification max_classification_;

    Td force_start_delay_;

    // classical and staged
    int min_users_;
    int max_users_;
    Td min_recruiting_time_;
    Td max_recruiting_time_;

    // classical
    int max_simultaneous_games_;
    float games_factor_;

    // staged
    Td relax_time_;
    int min_substages_;
    int increment_substages_;
};

}

#endif

