/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_RIGHTS_EDIT_HPP_
#define THECHESS_WIDGETS_RIGHTS_EDIT_HPP_

#include <map>

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

/** Edit user rights */
class RightsEdit : public Wt::WContainerWidget {
public:
    /** Constructor.
    \param user The user, whose rights will be edited.
    \param parent Parent widget.
    */
    RightsEdit(const UserPtr& user, Wt::WContainerWidget* parent = 0);

private:
    typedef std::map<User::Rights, Wt::WCheckBox*> Items;
    typedef std::map<AdminRights, Wt::WCheckBox*> AdminItems;

    Items items_;
    AdminItems admin_items_;
    UserPtr user_;

    void add_item(User::Rights right);
    void add_admin_item(AdminRights right);
    void save();
};

}

#endif

