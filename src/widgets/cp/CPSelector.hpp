/*
 * thechess, chess competition web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_CP_SELECTOR_HPP_
#define THECHESS_WIDGETS_CP_SELECTOR_HPP_

#include <Wt/WGlobal>
#include <Wt/WPanel>

#include "model/all.hpp"
#include "widgets/global.hpp"

namespace thechess {

/** Select existing competition parameters or create new */
class CPSelector : public Wt::WPanel {
public:
    /** Constructor */
    CPSelector(Wt::WContainerWidget* p = 0);

    /** Get selected cp */
    CPPtr cp() const;

    /** Select the cp */
    void set_cp(const CPPtr& cp);

private:
    CPListWidget* list_;
    CPWidget* new_;
    Wt::WContainerWidget* new_cont_;
    Wt::WTabWidget* tab_;

    void tab_handler(int tab_index);
};

}

#endif

