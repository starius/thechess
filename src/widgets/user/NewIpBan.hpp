/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_NEW_IP_BAN_HPP_
#define THECHESS_WIDGETS_NEW_IP_BAN_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/Wc/global.hpp>

namespace thechess {

/** Widget adding new IpBan instance.
Current user must have REGISTRATION_BANNER.
*/
class NewIpBan : public Wt::WContainerWidget {
public:
    /** Constructor */
    NewIpBan(const std::string& ip, const Wt::WString& reason,
             Wt::WContainerWidget* parent = 0);

private:
    std::string ip_;
    Wt::Wc::TimeDurationWidget* duration_;
    Wt::WTextArea* reason_;

    void add();
};

}

#endif

