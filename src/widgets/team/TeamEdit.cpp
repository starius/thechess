/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>
#include <Wt/WPushButton>

#include "widgets/team/TeamEdit.hpp"
#include "Application.hpp"
#include "log.hpp"

namespace thechess {

TeamEdit::TeamEdit(const TeamPtr& team) {
    dbo::Transaction t(tApp->session());
    // TODO check that this user can edit
    add_record_inputs(team.get(), this);
    Wt::WPushButton* save = new Wt::WPushButton(tr("tc.common.Save"));
    item("", "", save);
    save->clicked().connect(this, &TeamEdit::save);
}

void TeamEdit::save() {
    dbo::Transaction t(tApp->session());
    write_record(team_.modify());
    t.commit();
    // TODO open team page
    // TODO emit team event
}

}

