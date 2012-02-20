/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/comment/CommentModel.hpp"
#include "Application.hpp"

namespace thechess {

CommentModel::CommentModel(const CommentPtr& root, Wt::WObject* parent):
    CommentModel::BaseQM(parent),
    root_(root) {
    dbo::Transaction t(tApp->session());
    setQuery(get_query());
    addColumn("id", ""); // surrogate field
    t.commit();
}

boost::any CommentModel::data(const Wt::WModelIndex& index, int role) const {
    dbo::Transaction t(tApp->session());
    if (role == Wt::DisplayRole && index.column() == CONTENTS_COLUMN) {
        const CommentPtr& o = resultRow(index.row());
        return contents(o);
    }
    return BaseQM::data(index, role);
}

Wt::WFlags<Wt::ItemFlag> CommentModel::flags(const Wt::WModelIndex& i) const {
    Wt::WFlags<Wt::ItemFlag> f = BaseQM::flags(i);
    if (i.column() == CONTENTS_COLUMN) {
        f |= Wt::ItemIsXHTMLText;
    }
    return f;
}

Wt::WString CommentModel::contents(const CommentPtr& comment) {
    return comment->text(); // TODO
}

CommentModel::Query CommentModel::get_query() const {
    return root_->family().find().where("state <> ?").bind(Comment::SURROGATE);
}

}

