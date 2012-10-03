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
#include <Wt/Wc/global.hpp>

#include "model/all.hpp"

namespace thechess {

class IpListModel;

/** List of all IP used by the user with time of last use.
Current user must have REGISTRATION_BANNER.
*/
class IpList : public Wt::WContainerWidget {
public:
    /** Constructor */
    IpList(const UserPtr& user, Wt::WContainerWidget* parent = 0);

    /** Constructor */
    IpList(const std::string& ip, Wt::WContainerWidget* parent = 0);

private:
    Wt::Wc::TimeDurationWidget* last_used_;
    IpListModel* model_;

    void initialize();
    void apply();
};

}

#endif

