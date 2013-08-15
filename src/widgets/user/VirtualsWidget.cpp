/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <map>
#include <set>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WImage>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WBreak>
#include <Wt/WSlider>
#include <Wt/WTableView>
#include <Wt/WViewWidget>
#include <Wt/Dbo/QueryModel>
#include <Wt/Wc/FilterResource.hpp>
#include <Wt/Wc/Gather.hpp>

#include "widgets/user/VirtualsWidget.hpp"
#include "widgets/user/user_anchor.hpp"
#include "Application.hpp"
#include "log.hpp"

namespace thechess {

// TODO. Does not work with large graphs. Crashes
class VirtualsResource : public Wt::Wc::FilterResource {
public:
    VirtualsResource(const dbo::Query<BD::BDPair>& pairs, Wt::WObject* parent):
        Wt::Wc::FilterResource("dot -Tpng {1} -o {2}", parent),
        pairs_(pairs), min_score_(Wt::Wc::Gather::MIN_SIGNIFICANT) {
        setMimeType("image/png");
    }

    void write_input(std::ostream& out) const {
        Wt::WApplication::UpdateLock lock(wApp);
        if (lock) {
            dbo::Transaction t(tApp->session());
            if (!tApp->user() ||
                    !tApp->user()->has_permission(VIRTUALS_VIEWER)) {
                return;
            }
            BD::Scores scores;
            std::set<UserPtr> users;
            BD::scores(pairs_, scores);
            BD::filter(scores, min_score_);
            BD::add_users(scores, users);
            out << "graph virtuals {" << std::endl;
            out << "rankdir=LR" << std::endl;
            BOOST_FOREACH (UserPtr user, users) {
                out << user.id();
                std::string username = user->username20().toUTF8();
                boost::replace_all(username, "\"", "\\\"");
                out << "[label=\"" << username << "\"]" << std::endl;
            }
            BOOST_FOREACH (BD::Scores::value_type& pair_and_score, scores) {
                const UserPair& pair = pair_and_score.first;
                const UserPtr a = pair.first();
                const UserPtr b = pair.second();
                out << a.id() << "--" << b.id() << std::endl;
            }
            out << "}" << std::endl;
        }
    }

    void set_min_score(int min_score) {
        min_score_ = min_score;
        update();
    }

private:
    dbo::Query<BD::BDPair> pairs_;
    int min_score_;
};

class VirtualsList : public Wt::WViewWidget {
public:
    VirtualsList(const dbo::Query<BD::BDPair>& pairs,
                 Wt::WContainerWidget* parent = 0):
        Wt::WViewWidget(parent),
        pairs_(pairs), min_score_(Wt::Wc::Gather::MIN_SIGNIFICANT)
    { }

    virtual Wt::WWidget* renderView() {
        Wt::WContainerWidget* c = new Wt::WContainerWidget;
        dbo::Transaction t(tApp->session());
        if (!tApp->user() ||
                !tApp->user()->has_permission(VIRTUALS_VIEWER)) {
            return new Wt::WContainerWidget;
        }
        BD::Scores scores;
        BD::scores(pairs_, scores);
        BD::filter(scores, min_score_);
        BOOST_FOREACH (BD::Scores::value_type& pair_and_score, scores) {
            const UserPair& pair = pair_and_score.first;
            const UserPtr a = pair.first();
            const UserPtr b = pair.second();
            c->addWidget(new Wt::WBreak());
            c->addWidget(user_anchor(a));
            tApp->path().user_view()->set_integer_value(a.id());
            tApp->path().virtuals_of_user_pair()->set_integer_value(b.id());
            Wt::WAnchor* an = new Wt::WAnchor;
            an->setText(" &mdash; ");
            an->setLink(tApp->path().virtuals_of_user_pair()->link());
            c->addWidget(an);
            c->addWidget(user_anchor(b));
        }
        return c;
    }

    void set_min_score(int min_score) {
        min_score_ = min_score;
        update();
    }

private:
    dbo::Query<BD::BDPair> pairs_;
    int min_score_;
};

namespace VW {
typedef BDPair Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

const int WIDTH = 750;
const int HEIGHT = 500;
}

class BDPairModel : public VW::BaseQM {
public:
    enum Column {
        BD_TYPE,
        U_USED,
        U_USER,
        BD_VALUE,
        V_USER,
        V_USED
    };

    BDPairModel(const dbo::Query<BD::BDPair>& pairs, Wt::WObject* parent = 0):
        VW::BaseQM(parent) {
        setQuery(pairs);
        addColumn("U.type", tr("tc.common.type")); // dummy
        addColumn("U.used", tr("tc.common.last"));
        addColumn("U.user_id", tr("tc.user.user")); // dummy
        addColumn("U.value", tr("tc.common.value"));
        addColumn("V.user_id", tr("tc.user.user")); // dummy
        addColumn("V.used", tr("tc.common.last"));
        sort(BD_TYPE, Wt::AscendingOrder);
    }

private:
    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const BDPair& o = resultRow(index.row());
        if (role == Wt::DisplayRole) {
            if (index.column() == BD_TYPE) {
                return Wt::Wc::Gather::type_to_str(o.get<0>()->type());
            } else if (index.column() == U_USER) {
                UserPtr u_user = o.get<0>()->user();
                return u_user->username();
            } else if (index.column() == V_USER) {
                UserPtr v_user = o.get<1>()->user();
                return v_user->username();
            }
        } else if (role == Wt::LinkRole) {
            if (index.column() == U_USER) {
                UserPtr u_user = o.get<0>()->user();
                return tApp->path().user_view()->get_link(u_user.id());
            } else if (index.column() == V_USER) {
                UserPtr v_user = o.get<0>()->user();
                return tApp->path().user_view()->get_link(v_user.id());
            }
        }
        return VW::BaseQM::data(index, role);
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }
};

class BDPairView : public Wt::WTableView {
public:
    BDPairView(BDPairModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(VW::WIDTH, VW::HEIGHT);
        setSelectable(true);
    }

protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

VirtualsWidget::VirtualsWidget(const dbo::Query<BD::BDPair>& pairs,
                               Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent) {
    initialize(pairs);
    admin_log("List virtuals (paires provided)", true);
}

VirtualsWidget::VirtualsWidget(const UserPtr& user, Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    dbo::Transaction t(tApp->session());
    dbo::Query<BD::BDPair> pairs = BD::pairs(tApp->session());
    pairs.where("U.user_id = ? or V.user_id = ?").bind(user).bind(user);
    admin_log("List virtuals of user " + user_a(user.id()), true);
    initialize(pairs);
}

VirtualsWidget::VirtualsWidget(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    dbo::Transaction t(tApp->session());
    dbo::Query<BD::BDPair> pairs = BD::pairs(tApp->session());
    admin_log("List all virtuals", true);
    initialize(pairs);
}

static void update_text(Wt::WText* label, Wt::WSlider* slider) {
    label->setText(slider->valueText());
}

void VirtualsWidget::initialize(const dbo::Query<BD::BDPair>& pairs) {
    if (!tApp->user() || !tApp->user()->has_permission(VIRTUALS_VIEWER)) {
        return;
    }
    BDPairModel* m = new BDPairModel(pairs, this);
    addWidget(new BDPairView(m));
    VirtualsList* list = new VirtualsList(pairs);
    Wt::WSlider* min_score = new Wt::WSlider(this);
    Wt::WText* t = new Wt::WText(this);
    min_score->setMinimum(10);
    min_score->setMaximum(1000);
    min_score->setWidth(600);
    min_score->setValue(Wt::Wc::Gather::MIN_SIGNIFICANT);
    min_score->valueChanged().connect(list, &VirtualsList::set_min_score);
    min_score->valueChanged().connect(boost::bind(update_text, t, min_score));
    new Wt::WBreak(this);
    addWidget(list);
    update_text(t, min_score);
}

}

