/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_IP_LIST_HPP_
#define THECHESS_WIDGETS_IP_LIST_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

/** List of all IP used by the user with time of last use.
Current user must have REGISTRATION_BANNER.
*/
class IpList : public Wt::WContainerWidget {
public:
    /** Constructor */
    IpList(const UserPtr& user, Wt::WContainerWidget* parent = 0);
};

}

#endif

