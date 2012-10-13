/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WPushButton>
#include <Wt/WTableView>
#include <Wt/Dbo/QueryModel>
#include <Wt/Wc/Pager.hpp>

#include "widgets/user/IpBanList.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"

namespace thechess {

namespace BLP {
typedef IpBanPtr Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

const int WIDTH = 750;
const int HEIGHT = 500;
}

class IpBanListModel : public BLP::BaseQM {
public:
    enum {
        ENABLED,
        IP,
        START,
        STOP,
        REASON,
        CREATOR
    };

    IpBanListModel(const std::string& ip, Wt::WObject* parent = 0):
        BLP::BaseQM(parent),
        ip_(ip) {
        set_query();
        addColumn("id", tr("tc.common.enabled")); // dummy
        addColumn("ip", tr("tc.user.ip"));
        addColumn("start", tr("tc.common.start"));
        addColumn("stop", tr("tc.common.stop"));
        addColumn("reason", tr("tc.user.ban_reason"));
        addColumn("id", tr("tc.common.creator"));
        sort(STOP, Wt::DescendingOrder);
    }

private:
    std::string ip_;

    void set_query() {
        dbo::Transaction t(tApp->session());
        BLP::Q q = tApp->session().find<IpBan>();
        if (!ip_.empty()) {
            q.where("ip = ?").bind(ip_);
        }
        setQuery(q, /* keep_columns */ true);
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const IpBanPtr& o = resultRow(index.row());
        if (role == Wt::DisplayRole) {
            if (index.column() == ENABLED) {
                return tr(IpBan::state2str(o->state()));
            } else if (index.column() == CREATOR) {
                return o->creator()->username();
            }
        } else if (role == Wt::LinkRole) {
            if (index.column() == ENABLED) {
                return tApp->path().edit_banned_ip()->get_link(o.id());
            } else if (index.column() == IP) {
                return tApp->path().banned_ip()->get_link(o->ip());
            } else if (index.column() == CREATOR) {
                return tApp->path().user_view()->get_link(o->creator().id());
            }
        }
        return BLP::BaseQM::data(index, role);
    }

    Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex& i) const {
        Wt::WFlags<Wt::ItemFlag> f = BLP::BaseQM::flags(i);
        if (i.column() == REASON) {
            f |= Wt::ItemIsXHTMLText;
        }
        return f;
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }
};

class IpBanListView : public Wt::WTableView {
public:
    IpBanListView(IpBanListModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(BLP::WIDTH, BLP::HEIGHT);
        setColumnWidth(IpBanListModel::ENABLED, 100);
        setColumnWidth(IpBanListModel::IP, 100);
        setColumnWidth(IpBanListModel::START, 100);
        setColumnWidth(IpBanListModel::STOP, 100);
    }

protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

IpBanList::IpBanList(const std::string& ip, Wt::WContainerWidget* parent):
    WContainerWidget(parent) {
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    addWidget(new Header(tr("tc.user.IpBanList")));
    IpBanListModel* m = new IpBanListModel(ip, this);
    IpBanListView* view = new IpBanListView(m, this);
}

IpBanList::IpBanList(Wt::WContainerWidget* parent):
    WContainerWidget(parent) {
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    addWidget(new Header(tr("tc.user.IpBanList")));
    IpBanListModel* m = new IpBanListModel("", this);
    IpBanListView* view = new IpBanListView(m, this);
}

}

