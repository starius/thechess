/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_BD_HPP_
#define THECHESS_MODEL_BD_HPP_

#include <set>

#include "model/model.hpp"

namespace thechess {

/** Browser data.

\ingroup model
*/
class BD : public dbo::Dbo<BD> {
public:
    /** A pair of BDPtr's */
    typedef ::thechess::BDPair BDPair;

    /** Map user pair to int */
    typedef std::map<UserPair, int> Scores;

    /** Default constructor.
    Should be used only by Wt::Dbo itself.
    */
    BD();

    /** Create instance to be added to database */
    BD(const BDId& id);

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        dbo::id(a, id_, "");
        dbo::field(a, used_, "used");
    }

    /** Get user */
    const UserPtr& user() const;

    /** Get type of data */
    Wt::Wc::Gather::DataType type() const;

    /** Get value */
    const std::string& value() const;

    /** Get last time the data was used */
    const Wt::WDateTime used() const {
        return used_;
    }

    /** Update the time of ast use */
    void use();

    /** Return query for all pairs of BD's with same type and value.
    The query uses two aliases of BD's table: U and V.
    So you can filter returned query for example by user_id or by type.

    This is usefull for both global (all virtuals of everybody) and local
    (all virtuals of somebody) searches.

    \note To avoid redundancy, only rows where U.user_id<V.user_id are selected.
    */
    static dbo::Query<BDPair> pairs(dbo::Session& session);

    /** Increase scores of user pairs, corresponding to the query */
    static void scores(const dbo::Query<BDPair>& pairs, Scores& scores);

    /** Filter scores map.
    If min_score = -1, the default one is used.
    */
    static void filter(Scores& scores, int min_score = -1);

    /** Add users, involved into scores map, to the set */
    static void add_users(const Scores& scores, std::set<UserPtr>& users);

private:
    BDId id_;
    Wt::WDateTime used_;
};

}

DBO_EXTERN_TEMPLATES(thechess::BD);

#endif

