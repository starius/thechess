/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WPushButton>
#include <Wt/Dbo/Transaction>

#include "widgets/CompetitionCreateWidget.hpp"
#include "widgets/PleaseLoginWidget.hpp"
#include "widgets/CPWidget.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "config.hpp"

namespace thechess {

CompetitionCreateWidget::CompetitionCreateWidget(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    dbo::Transaction t(tApp->session());
    if (!tApp->user()) {
        new PleaseLoginWidget(this);
    } else if (Competition::can_create_competition(tApp->user())) {
        new Wt::WText(tr("tc.competition.Create_welcome"), this);
        GPPtr gp = new GP(true);
        CPPtr cp = new CP(gp);
        CompetitionPtr c = new Competition(cp);
        cpw_ = new CPWidget2(&(*(c)), /*allow_change_type*/ true, this);
        new Wt::WBreak(this);
        ok_ = new Wt::WPushButton(tr("tc.common.Create"), this);
        ok_->clicked().connect(this, &CompetitionCreateWidget::button_handler_);
    } else {
        new Wt::WText(tr("tc.competition.Cant_create"), this);
    }
    t.commit();
}

CompetitionCreateWidget::CompetitionCreateWidget(const CompetitionPtr& c,
        Wt::WContainerWidget* p):
    Wt::WContainerWidget(p), c_(c) {
    dbo::Transaction t(tApp->session());
    if (!tApp->user()) {
        new PleaseLoginWidget(this);
    } else if (c->can_change_parameters(tApp->user())) {
        new Wt::WText(tr("tc.competition.Change_welcome").arg(int(c.id())), this);
        cpw_ = new CPWidget2(&(*c), /*allow_change_type*/ true, this);
        new Wt::WBreak(this);
        ok_ = new Wt::WPushButton(tr("tc.common.Save"), this);
        ok_->clicked().connect(this, &CompetitionCreateWidget::button_handler_);
    } else {
        new Wt::WText(tr("tc.competition.Cant_change"), this);
    }
    t.commit();
}

Wt::EventSignal<Wt::WMouseEvent>& CompetitionCreateWidget::saved() {
    return ok_->clicked();
}

void CompetitionCreateWidget::button_handler_() {
    dbo::Transaction t(tApp->session());
    GPPtr gp;
    CPPtr cp;
    CompetitionPtr comp;
    if (c_) {
        comp = c_;
        cp = comp->cp();
        gp = cp->gp();
    } else {
        gp = tApp->session().add(new GP(true));
        cp = tApp->session().add(new CP(gp));
        comp = tApp->session().add(new Competition(cp));
    }
    cpw_->apply_parameters(comp.modify());
    comp.modify()->create_competition(tApp->user());
    t.commit();
    tApp->path().competition_view()->set_integer_value(comp.id());
    tApp->path().competition_view()->open(/* change path */ true);
    tApp->server().tracker().add_or_update_task(Object(COMPETITION, comp.id()));
}

}

