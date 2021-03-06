/*
 * thechess, chess competition web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WText>

#include "widgets/cp/CPSelector.hpp"
#include "widgets/cp/CPListWidget.hpp"
#include "widgets/cp/CPWidget.hpp"
#include "Application.hpp"

namespace thechess {

enum {
    CP_SELECTOR_LIST_TAB,
    CP_SELECTOR_NEW_TAB
};

CPSelector::CPSelector(Wt::WContainerWidget* p):
    Wt::WPanel(p) {
    setTitle(tr("tc.competition.Select_cp"));
    setCollapsible(true);
    Wt::WContainerWidget* c = new Wt::WContainerWidget();
    setCentralWidget(c);
    new Wt::WText(tr("tc.common.Select_or_create"), c);
    list_ = new CPListWidget();
    tab_ = new Wt::WTabWidget(c);
    tab_->addTab(list_, tr("tc.common.Select_existing"));
    if (tApp->user() && tApp->user()->has_permission(CP_CREATOR)) {
        new_cont_ = new Wt::WContainerWidget();
        new_ = new CPWidget(0, true, new_cont_);
        tab_->addTab(new_cont_, tr("tc.common.Create_new"));
    }
    tab_->currentChanged().connect(this, &CPSelector::tab_handler);
}

CPPtr CPSelector::cp() const {
    CPPtr result;
    if (tab_->currentIndex() == CP_SELECTOR_LIST_TAB) {
        result = list_->cp();
    } else if (tab_->currentIndex() == CP_SELECTOR_NEW_TAB) {
        CPPtr new_cp = new CP(true);
        new_->apply_parameters(new_cp.modify());
        new_cp.modify()->set_init(tApp->user());
        if (list_->cp()) {
            new_cp.modify()->set_parent(list_->cp());
        }
        if (new_cp->gp()) {
            result = tApp->session().add(new_cp);
        }
    }
    return result;
}

void CPSelector::set_cp(const CPPtr& cp) {
    list_->set_cp(cp);
}

void CPSelector::tab_handler(int tab_index) {
    if (tab_index == CP_SELECTOR_NEW_TAB && list_->cp()) {
        new_cont_->clear();
        new_ = new CPWidget(list_->cp().get(), true, new_cont_);
    }
}

}

