/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_BD_HPP_
#define THECHESS_MODEL_BD_HPP_

#include "model/global.hpp"

namespace thechess {

/** Browser data.

\ingroup model
*/
class BD : public dbo::Dbo<BD> {
public:
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

private:
    BDId id_;
    Wt::WDateTime used_;
};

}

#endif

