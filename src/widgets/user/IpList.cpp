/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WTableView>
#include <Wt/WPushButton>
#include <Wt/Dbo/QueryModel>
#include <Wt/Wc/TimeDurationWidget.hpp>
#include <Wt/Wc/Pager.hpp>

#include "widgets/user/IpList.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"
#include "log.hpp"
#include "config.hpp"

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
        TYPE,
        VALUE,
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

    void set_last_used(const Td& last_used) {
        if (last_used != last_used_) {
            last_used_ = last_used;
            set_query();
        }
    }

private:
    UserPtr user_;
    std::string ip_;
    Td last_used_;

    void initialize() {
        last_used_ = 4 * WEEK;
        set_query();
        addColumn("type", tr("tc.user.user")); // dummy
        addColumn("type", tr("tc.common.type")); // dummy
        addColumn("value", tr("tc.common.value"));
        addColumn("used", tr("tc.common.last"));
        addColumn("type", tr("tc.user.New_ban")); // dummy
        sort(LAST_USE, Wt::DescendingOrder);
    }

    void set_query() {
        dbo::Transaction t(tApp->session());
        ILP::Q q = tApp->session().find<BD>();
        if (user_) {
            q.where("user_id = ?").bind(user_);
        } else {
            q.where("value = ?").bind(ip_);
        }
        q.where("used > ?").bind(now() - last_used_);
        setQuery(q, /* keep_columns */ true);
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const BDPtr& o = resultRow(index.row());
        if (role == Wt::DisplayRole) {
            if (index.column() == BAN) {
                if (o->type() == Wt::Wc::Gather::IP) {
                    if (IpBan::is_banned(o->value())) {
                        return tr("tc.user.Already_banned");
                    } else {
                        return tr("tc.user.New_ban");
                    }
                } else {
                    return "";
                }
            } else if (index.column() == USER) {
                return o->user()->username();
            } else if (index.column() == TYPE) {
                return Wt::Wc::Gather::type_to_str(o->type());
            }
        } else if (role == Wt::LinkRole) {
            if (index.column() == VALUE && o->type() == Wt::Wc::Gather::IP) {
                return tApp->path().banned_ip()->get_link(o->value());
            } else if (index.column() == BAN &&
                       o->type() == Wt::Wc::Gather::IP &&
                       !IpBan::is_banned(o->value())) {
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
        setColumnWidth(IpListModel::VALUE, 200);
        setSelectable(true);
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
    model_ = new IpListModel(user, this);
    admin_log("List IP of " + user_a(user.id()), true);
    initialize();
}

IpList::IpList(const std::string& ip, Wt::WContainerWidget* parent):
    WContainerWidget(parent) {
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    model_ = new IpListModel(ip, this);
    admin_log("List users of IP " + ip_a(ip), true);
    initialize();
}

void IpList::initialize() {
    addWidget(new Header(tr("tc.user.IpList")));
    last_used_ = new Wt::Wc::TimeDurationWidget(
        config::min::IP_LAST_USED,
        config::defaults::IP_LAST_USED,
        config::max::IP_LAST_USED,
        this);
    Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Apply"), this);
    b->clicked().connect(this, &IpList::apply);
    IpListView* view = new IpListView(model_, this);
}

void IpList::apply() {
    model_->set_last_used(last_used_->corrected_value());
}

}

