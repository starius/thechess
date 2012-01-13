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
    GP gp(true);
    new_ = new GPWidget(&gp);
    tab_ = new Wt::WTabWidget(this);
    tab_->addTab(list_, tr("tc.common.Select_existing"));
    tab_->addTab(new_, tr("tc.common.Create_new"));
}

GPPtr GPSelector::gp() const {
    GPPtr result;
    if (tab_->currentIndex() == GP_SELECTOR_LIST_TAB) {
        result = list_->gp();
    } else if (tab_->currentIndex() == GP_SELECTOR_NEW_TAB) {
        GP* new_gp = new GP(true);
        new_->apply_parameters(new_gp);
        new_gp->set_init(tApp->user());
        result = tApp->session().add(new_gp);
    }
    return result;
}

}

