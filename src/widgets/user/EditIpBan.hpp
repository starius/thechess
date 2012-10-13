/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_EDIT_IP_BAN_HPP_
#define THECHESS_WIDGETS_EDIT_IP_BAN_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/Wc/global.hpp>

#include "model/all.hpp"

namespace thechess {

/** Widget editing IpBan instance.
Current user must have REGISTRATION_BANNER.
*/
class EditIpBan : public Wt::WContainerWidget {
public:
    /** Constructor */
    EditIpBan(const IpBanPtr& ban, Wt::WContainerWidget* parent = 0);

private:
    IpBanPtr ban_;

    void button(BanState state);
    void save(BanState state);
};

}

#endif

