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
#include "widgets/Header.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "config.hpp"
#include "log.hpp"

namespace thechess {

CompetitionCreateWidget::CompetitionCreateWidget(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    dbo::Transaction t(tApp->session());
    if (tApp->user() && Competition::can_create_competition(tApp->user())) {
        new Header(tr("tc.competition.Create_welcome"), this);
        print();
        ok_ = new Wt::WPushButton(tr("tc.common.Create"), this);
        ok_->clicked().connect(this, &CompetitionCreateWidget::button_handler);
        error_message_ = new Wt::WText(this);
        error_message_->setStyleClass("thechess-error");
    } else {
        new Wt::WText(tr("tc.competition.Cant_create"), this);
    }
}

CompetitionCreateWidget::CompetitionCreateWidget(const CPPtr& cp,
        Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    dbo::Transaction t(tApp->session());
    if (tApp->user() && Competition::can_create_competition(tApp->user())) {
        new Header(tr("tc.competition.Create_welcome"), this);
        print();
        gp_selector_->set_gp(cp->gp());
        cp_selector_->set_cp(cp);
        ok_ = new Wt::WPushButton(tr("tc.common.Create"), this);
        ok_->clicked().connect(this, &CompetitionCreateWidget::button_handler);
        error_message_ = new Wt::WText(this);
        error_message_->setStyleClass("thechess-error");
    } else {
        new Wt::WText(tr("tc.competition.Cant_create"), this);
    }
}

CompetitionCreateWidget::CompetitionCreateWidget(const CompetitionPtr& c,
        Wt::WContainerWidget* p):
    Wt::WContainerWidget(p), c_(c) {
    dbo::Transaction t(tApp->session());
    if (tApp->user() && c->can_change_parameters(tApp->user())) {
        new Wt::WText(tr("tc.competition.Change_welcome").arg(int(c.id())),
                      this);
        print();
        gp_selector_->set_gp(c->self_gp());
        cp_selector_->set_cp(c->cp());
        ok_ = new Wt::WPushButton(tr("tc.common.Save"), this);
        ok_->clicked().connect(this, &CompetitionCreateWidget::button_handler);
        error_message_ = new Wt::WText(this);
        error_message_->setStyleClass("thechess-error");
    } else {
        new Wt::WText(tr("tc.competition.Cant_change"), this);
    }
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
    CPPtr cp = cp_selector_->cp();
    if (!cp) {
        error_message_->setText(tr("tc.common.no_params_error"));
        return;
    }
    error_message_->setText("");
    if (c_) {
        comp = c_;
        comp.reread();
        write_record(comp.modify(), /* init */ false);
        if (comp->state() == Competition::CANCELLED) {
            comp.modify()->revive();
            admin_log("Change and revive " + comp_a(comp.id()));
        } else {
            admin_log("Change " + comp_a(comp.id()));
        }
    } else {
        comp = tApp->session().add(new Competition(true));
        write_record(comp.modify(), /* init */ true);
        comp.flush();
        admin_log("Create " + comp_a(comp.id()));
    }
    comp.modify()->set_gp(gp_selector_->gp());
    comp.modify()->set_cp(cp);
    t.commit();
    tApp->path().competition_view()->set_integer_value(comp.id());
    tApp->path().competition_view()->open(/* change path */ true);
    t_task(COMPETITION, comp.id());
}

}

