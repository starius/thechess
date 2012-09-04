/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WTableView>
#include <Wt/Dbo/QueryModel>
#include <Wt/Wc/Pager.hpp>

#include "widgets/user/IpList.hpp"
#include "Application.hpp"

namespace thechess {

namespace ILP {
typedef BDPtr Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

const int WIDTH = 750;
const int HEIGHT = 500;
}

class IpListModel : public ILP::BaseQM {
public:
    enum {
        IP,
        LAST_USE,
        BANNED
    };

    IpListModel(const UserPtr& user, Wt::WObject* parent = 0):
        ILP::BaseQM(parent),
        user_(user) {
        set_query();
        addColumn("value", tr("tc.user.ip"));
        addColumn("used", tr("tc.common.last"));
        addColumn("type", tr("tc.user.Already_banned")); // dummy
        sort(LAST_USE, Wt::DescendingOrder);
    }

private:
    UserPtr user_;

    void set_query() {
        dbo::Transaction t(tApp->session());
        ILP::Q q = tApp->session().find<BD>();
        q.where("user_id = ?").bind(user_);
        q.where("type = ?").bind(Wt::Wc::Gather::IP);
        setQuery(q, /* keep_columns */ true);
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const BDPtr& o = resultRow(index.row());
        if (role == Wt::DisplayRole) {
            if (index.column() == BANNED) {
                return IpBan::is_banned(o->value());
            }
        }
        // TODO reference to IP page
        return ILP::BaseQM::data(index, role);
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }
};

class IpListView : public Wt::WTableView {
public:
    IpListView(IpListModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(ILP::WIDTH, ILP::HEIGHT);
    }

protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

IpList::IpList(const UserPtr& user, Wt::WContainerWidget* parent):
    WContainerWidget(parent) {
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    IpListModel* m = new IpListModel(user, this);
    IpListView* view = new IpListView(m, this);
}

}

