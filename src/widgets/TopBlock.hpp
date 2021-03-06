/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_TOP_BLOCK_HPP_
#define THECHESS_TOP_BLOCK_HPP_

#include <Wt/WGlobal>
#include <Wt/WText>

#include "model/object.hpp"

namespace thechess {

/** Widget shown right to AuthWidget when user ias logged in */
class TopBlock : public Wt::WText, public Notifiable {
public:
    /** Constructor */
    TopBlock(Wt::WContainerWidget* parent = 0);

    /** Update contents */
    void notify(EventPtr);

private:
    void update_contents();
    void set_message();
    void set_email_unverified();
};

}

#endif

