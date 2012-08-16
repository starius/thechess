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
#include <Wt/WText>
#include <Wt/WSlider>
#include <Wt/Wc/FilterResource.hpp>

#include "widgets/user/VirtualsWidget.hpp"
#include "Application.hpp"

namespace thechess {

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
                    !tApp->user()->has_permission(User::VIRTUALS_VIEWER)) {
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
                int score = pair_and_score.second;
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

VirtualsWidget::VirtualsWidget(const dbo::Query<BD::BDPair>& pairs,
                               Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent) {
    initialize(pairs);
}

VirtualsWidget::VirtualsWidget(const UserPtr& user, Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    dbo::Transaction t(tApp->session());
    dbo::Query<BD::BDPair> pairs = BD::pairs(tApp->session());
    pairs.where("U.user_id = ? or V.user_id = ?").bind(user).bind(user);
    initialize(pairs);
}

VirtualsWidget::VirtualsWidget(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    dbo::Transaction t(tApp->session());
    dbo::Query<BD::BDPair> pairs = BD::pairs(tApp->session());
    initialize(pairs);
}

static void update_text(Wt::WText* label, Wt::WSlider* slider) {
    label->setText(slider->valueText());
}

void VirtualsWidget::initialize(const dbo::Query<BD::BDPair>& pairs) {
    VirtualsResource* res = new VirtualsResource(pairs, this);
    Wt::WSlider* min_score = new Wt::WSlider(this);
    Wt::WText* t = new Wt::WText(this);
    min_score->setMinimum(20);
    min_score->setMaximum(250);
    min_score->setValue(Wt::Wc::Gather::MIN_SIGNIFICANT);
    min_score->valueChanged().connect(res, &VirtualsResource::set_min_score);
    min_score->valueChanged().connect(boost::bind(update_text, t, min_score));
    new Wt::WBreak(this);
    Wt::WImage* image = new Wt::WImage(this);
    image->setResource(res);
    update_text(t, min_score);
}

}

