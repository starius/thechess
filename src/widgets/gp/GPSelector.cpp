/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WTabWidget>
#include <Wt/WText>

#include "widgets/gp/GPSelector.hpp"
#include "widgets/gp/GPListWidget.hpp"
#include "widgets/gp/GPWidget.hpp"
#include "Application.hpp"

namespace thechess {

const int GP_SELECTOR_LIST_TAB = 0;
const int GP_SELECTOR_NEW_TAB = 1;

GPSelector::GPSelector(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    new Wt::WText(tr("tc.common.Select_or_create"), this);
    list_ = new GPListWidget();
    new_cont_ = new Wt::WContainerWidget();
    new_ = new GPWidget(0, new_cont_);
    tab_ = new Wt::WTabWidget(this);
    tab_->addTab(list_, tr("tc.common.Select_existing"));
    tab_->addTab(new_cont_, tr("tc.common.Create_new"));
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

void GPSelector::tab_handler(int tab_index) {
    if (tab_index == GP_SELECTOR_NEW_TAB && list_->gp()) {
        new_cont_->clear();
        new_ = new GPWidget(list_->gp().get(), new_cont_);
    }
}

}

