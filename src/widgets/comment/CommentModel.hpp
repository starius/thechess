/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_COMMENT_MODEL_HPP_
#define THECHESS_MODEL_COMMENT_MODEL_HPP_

#include <Wt/WGlobal>
#include <Wt/WString>
#include <Wt/Dbo/QueryModel>

#include "model/all.hpp"

namespace thechess {

/** MVC model class for comment */
class CommentModel : public dbo::QueryModel<CommentPtr> {
public:
    /** Result type */
    typedef CommentPtr Result;

    /** Query type */
    typedef dbo::Query<Result> Query;

    /** Base class of this class */
    typedef dbo::QueryModel<Result> BaseQM;

    /** Enum for column indices */
    enum Columns {
        CONTENTS_COLUMN /**< Contents of the comment */
    };

    /** Constructor */
    CommentModel(const CommentPtr& root, Wt::WObject* parent = 0);

    /** Return data at a specific model index */
    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const;

    /** Flags of columns */
    Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex& i) const;

    /** Return html comment representation */
    static Wt::WString contents(const CommentPtr& comment);

    /** Return query for the MVC model */
    Query get_query() const;

private:
    CommentPtr root_;
};

}

#endif

