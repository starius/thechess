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
        ID_COL, /**< ID (href) */
        TIME_COL, /** Time (creation or last comment) */
        INIT_COL, /** User */
        DESTINATION_COL, /** Destination user */
        CONTENTS_COLUMN /**< Contents of the comment */
    };

    /** Constructor.
    \param type Type of selected comments. NO_TYPE means all comments.
    \param root Root of selected comments (ignored for PRIVATE_MESSAGE).
    \param init Author of message to filter in. If type == PRIVATE_MESSAGE,
        this specifies user to show private chat with.
    \param parent Parent widget.
    */
    CommentModel(Comment::Type type, const CommentPtr& root = CommentPtr(),
                 const UserPtr& init = UserPtr(), Wt::WObject* parent = 0);

    /** Return data at a specific model index */
    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const;

    /** Flags of columns */
    Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex& i) const;

    /** Return html comment representation */
    Wt::WString contents(const CommentPtr& comment) const;

    /** Return query for the MVC model */
    Query get_query() const;

    /** Return root */
    const CommentPtr& root() const {
        return root_;
    }

    /** Return init */
    const UserPtr& init() const {
        return init_;
    }

    /** Return type */
    Comment::Type type() const {
        return type_;
    }

    /** Get if only Comment's with state=OK are filtered */
    bool only_ok() const {
        return only_ok_;
    }

    /** Set if only Comment's with state=OK are filtered */
    void set_only_ok(bool only_ok = true);

    /** Get if only Comment's with state=DRAFT are filtered */
    bool only_draft() const {
        return only_draft_;
    }

    /** Set if only Comment's with state=DRAFT are filtered */
    void set_only_draft(bool only_draft = true);

    /** Get if only my comments are shown */
    bool only_my() const {
        return only_my_;
    }

    /** Set if only my comments are shown */
    void set_only_my(bool only_my = true);

    /** Get filter by ip of author */
    const std::string ip() const {
        return ip_;
    }

    /** Set filter by ip of author */
    void set_ip(const std::string ip);

    /** Get filter by text of ID */
    const Wt::WString& text_like() const {
        return text_like_;
    }

    /** Set filter by text of ID */
    void set_text_like(const Wt::WString& text_like);

private:
    Comment::Type type_;
    CommentPtr root_;
    UserPtr init_;
    bool only_ok_;
    bool only_draft_;
    bool only_my_;
    std::string ip_;
    Wt::WString text_like_;
};

}

#endif

