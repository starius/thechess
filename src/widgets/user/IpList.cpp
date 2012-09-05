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
        USER,
        IP,
        LAST_USE,
        BAN
    };

    IpListModel(const UserPtr& user, Wt::WObject* parent = 0):
        ILP::BaseQM(parent), user_(user) {
        initialize();
    }

    IpListModel(const std::string& ip, Wt::WObject* parent = 0):
        ILP::BaseQM(parent), ip_(ip) {
        initialize();
    }

private:
    UserPtr user_;
    std::string ip_;

    void initialize() {
        set_query();
        addColumn("type", tr("tc.user.user")); // dummy
        addColumn("value", tr("tc.user.ip"));
        addColumn("used", tr("tc.common.last"));
        addColumn("type", tr("tc.user.New_ban")); // dummy
        sort(LAST_USE, Wt::DescendingOrder);
    }

    void set_query() {
        dbo::Transaction t(tApp->session());
        ILP::Q q = tApp->session().find<BD>();
        q.where("type = ?").bind(Wt::Wc::Gather::IP);
        if (user_) {
            q.where("user_id = ?").bind(user_);
        } else {
            q.where("value = ?").bind(ip_);
        }
        setQuery(q, /* keep_columns */ true);
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const BDPtr& o = resultRow(index.row());
        if (role == Wt::DisplayRole) {
            if (index.column() == BAN) {
                if (IpBan::is_banned(o->value())) {
                    return tr("tc.user.Already_banned");
                } else {
                    return tr("tc.user.New_ban");
                }
            } else if (index.column() == USER) {
                return o->user()->username();
            }
        } else if (role == Wt::LinkRole) {
            if (index.column() == IP) {
                return tApp->path().banned_ip()->get_link(o->value());
            } else if (index.column() == BAN && !IpBan::is_banned(o->value())) {
                tApp->path().user_view()->set_integer_value(o->user().id());
                return tApp->path().new_ip_ban()->get_link(o->value());
            } else if (index.column() == USER) {
                return tApp->path().user_view()->get_link(o->user().id());
            }
        }
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

IpList::IpList(const std::string& ip, Wt::WContainerWidget* parent):
    WContainerWidget(parent) {
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    IpListModel* m = new IpListModel(ip, this);
    IpListView* view = new IpListView(m, this);
}

}

