/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_IP_BAN_LIST_HPP_
#define THECHESS_WIDGETS_IP_BAN_LIST_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

/** List of all bans.
Current user must have REGISTRATION_BANNER.
*/
class IpBanList : public Wt::WContainerWidget {
public:
    /** Constructor */
    IpBanList(const std::string& ip, Wt::WContainerWidget* parent = 0);

    /** Constructor.
    This shows bans of all IPs.
    */
    IpBanList(Wt::WContainerWidget* parent = 0);
};

}

#endif

