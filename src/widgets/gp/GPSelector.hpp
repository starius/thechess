/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GP_SELECTOR_HPP_
#define THECHESS_WIDGETS_GP_SELECTOR_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

#include "model/all.hpp"
#include "widgets/global.hpp"

namespace thechess {

/** Select existing game parameters or create new */
class GPSelector : public Wt::WContainerWidget {
public:
    /** Constructor */
    GPSelector(Wt::WContainerWidget* p = 0);

    /** Get selected gp */
    GPPtr gp() const;

private:
    GPListWidget* list_;
    GPWidget* new_;
    Wt::WTabWidget* tab_;
};

}

#endif
