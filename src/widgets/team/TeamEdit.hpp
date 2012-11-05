/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_TEAM_EDIT_HPP_
#define THECHESS_WIDGETS_TEAM_EDIT_HPP_

#include <Wt/WGlobal>
#include <Wt/Wc/global.hpp>
#include <Wt/Wc/TableForm.hpp>

#include "widgets/RecordWidgetBase.hpp"
#include "model/all.hpp"

namespace thechess {

/** Widget editing team instance.
Current user must be team leader or moderator.
*/
class TeamEdit : public Wt::Wc::TableForm, RecordWidgetBase {
public:
    /** Constructor */
    TeamEdit(const TeamPtr& team);

private:
    TeamPtr team_;

    void save();
};

}

#endif

