/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WText>

#include "widgets/gp/GPSelector.hpp"
#include "widgets/gp/GPListWidget.hpp"
#include "widgets/gp/GPWidget.hpp"
#include "Application.hpp"

namespace thechess {

enum {
    GP_SELECTOR_LIST_TAB,
    GP_SELECTOR_NEW_TAB
};

GPSelector::GPSelector(Wt::WContainerWidget* p):
    Wt::WPanel(p) {
    setTitle(tr("tc.game.Select_gp"));
    setCollapsible(true);
    Wt::WContainerWidget* c = new Wt::WContainerWidget();
    setCentralWidget(c);
    new Wt::WText(tr("tc.common.Select_or_create"), c);
    list_ = new GPListWidget();
    tab_ = new Wt::WTabWidget(c);
    tab_->addTab(list_, tr("tc.common.Select_existing"));
    if (tApp->user() && tApp->user()->has_permission(GP_CREATOR)) {
        new_cont_ = new Wt::WContainerWidget();
        new_ = new GPWidget(0, new_cont_);
        tab_->addTab(new_cont_, tr("tc.common.Create_new"));
    }
    tab_->currentChanged().connect(this, &GPSelector::tab_handler);
}

GPPtr GPSelector::gp() const {
    GPPtr result;
    if (tab_->currentIndex() == GP_SELECTOR_LIST_TAB) {
        result = list_->gp();
    } else if (tab_->currentIndex() == GP_SELECTOR_NEW_TAB) {
        GP* new_gp = new GP(true);
        new_->apply_parameters(new_gp);
        new_gp->set_init(tApp->user());
        if (list_->gp()) {
            new_gp->set_parent(list_->gp());
        }
        result = tApp->session().add(new_gp);
    }
    return result;
}

void GPSelector::set_gp(const GPPtr& gp) {
    list_->set_gp(gp);
}

void GPSelector::set_moves(const Moves& moves) {
    if (GP_SELECTOR_NEW_TAB >= tab_->count()) {
        return;
    }
    tab_->setCurrentIndex(GP_SELECTOR_NEW_TAB);
    new_cont_->clear();
    GP* gp = new GP(true);
    gp->set_moves(moves);
    new_ = new GPWidget(gp, new_cont_);
    delete gp;
}

void GPSelector::tab_handler(int tab_index) {
    if (tab_index == GP_SELECTOR_NEW_TAB && list_->gp()) {
        new_cont_->clear();
        new_ = new GPWidget(list_->gp().get(), new_cont_);
    }
}

}

