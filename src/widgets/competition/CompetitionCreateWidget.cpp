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
#include <Wt/Wc/TableForm.hpp>

#include "widgets/competition/CompetitionCreateWidget.hpp"
#include "widgets/cp/CPSelector.hpp"
#include "widgets/gp/GPSelector.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "config.hpp"

namespace thechess {

CompetitionCreateWidget::CompetitionCreateWidget(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    dbo::Transaction t(tApp->session());
    if (tApp->user() && Competition::can_create_competition(tApp->user())) {
        new Wt::WText(tr("tc.competition.Create_welcome"), this);
        print();
        ok_ = new Wt::WPushButton(tr("tc.common.Create"), this);
        ok_->clicked().connect(this, &CompetitionCreateWidget::button_handler);
    } else {
        new Wt::WText(tr("tc.competition.Cant_create"), this);
    }
    t.commit();
}

CompetitionCreateWidget::CompetitionCreateWidget(const CompetitionPtr& c,
        Wt::WContainerWidget* p):
    Wt::WContainerWidget(p), c_(c) {
    dbo::Transaction t(tApp->session());
    if (tApp->user() && c->can_change_parameters(tApp->user())) {
        new Wt::WText(tr("tc.competition.Change_welcome").arg(int(c.id())), this);
        print();
        ok_ = new Wt::WPushButton(tr("tc.common.Save"), this);
        ok_->clicked().connect(this, &CompetitionCreateWidget::button_handler);
    } else {
        new Wt::WText(tr("tc.competition.Cant_change"), this);
    }
    t.commit();
}

void CompetitionCreateWidget::print() {
    cp_selector_ = new CPSelector(this);
    gp_selector_ = new GPSelector(this);
    gp_selector_->setCollapsed(true);
    form_ = new Wt::Wc::TableForm(this);
    add_record_inputs(c_.get(), form_);
}

Wt::EventSignal<Wt::WMouseEvent>& CompetitionCreateWidget::saved() {
    return ok_->clicked();
}

void CompetitionCreateWidget::button_handler() {
    dbo::Transaction t(tApp->session());
    CompetitionPtr comp;
    if (!cp_selector_->cp()) {
        return;
    }
    if (c_) {
        comp = c_;
        write_record(comp.modify(), /* init */ false);
    } else {
        comp = tApp->session().add(new Competition(true));
        write_record(comp.modify(), /* init */ true);
    }
    comp.modify()->set_gp(gp_selector_->gp());
    comp.modify()->set_cp(cp_selector_->cp());
    t.commit();
    tApp->path().competition_view()->set_integer_value(comp.id());
    tApp->path().competition_view()->open(/* change path */ true);
    tApp->server().planning().add(new Object(COMPETITION, comp.id()), now());
}

}

