/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */


#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WPushButton>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;

#include "widgets/CompetitionCreateWidget.hpp"
#include "widgets/PleaseLoginWidget.hpp"
#include "widgets/CompetitionParametersWidget.hpp"
#include "model/Competition.hpp"
#include "model/Object.hpp"
#include "ThechessApplication.hpp"
#include "config.hpp"

namespace thechess {
namespace widgets {
using namespace model;

CompetitionCreateWidget::CompetitionCreateWidget(Wt::WContainerWidget* p):
Wt::WContainerWidget(p)
{
    dbo::Transaction t(tApp->session());
    if (!tApp->user())
    {
        new PleaseLoginWidget(this);
    }
    else if (Competition::can_create_competition(tApp->user()))
    {
        new Wt::WText(tr("thechess.competition.create_welcome"), this);
        Competition* c = new Competition(true);
        cpw_ = new CompetitionParametersWidget2(c, /*allow_change_type*/ true, this);
        delete c;

        new Wt::WBreak(this);
        ok_ = new Wt::WPushButton(tr("thechess.create"), this);
        ok_->clicked().connect(this, &CompetitionCreateWidget::button_handler_);
    }
    else
    {
        new Wt::WText(tr("thechess.competition.cant_create"), this);
    }
    t.commit();
}

CompetitionCreateWidget::CompetitionCreateWidget(model::CompetitionPtr c,
    Wt::WContainerWidget* p):
Wt::WContainerWidget(p), c_(c)
{
    dbo::Transaction t(tApp->session());
    if (!tApp->user())
    {
        new PleaseLoginWidget(this);
    }
    else if (c->can_change_parameters(tApp->user()))
    {
        new Wt::WText(tr("thechess.competition.change_welcome").arg(int(c.id())), this);
        cpw_ = new CompetitionParametersWidget2(&(*c), /*allow_change_type*/ true, this);

        new Wt::WBreak(this);
        ok_ = new Wt::WPushButton(tr("thechess.save"), this);
        ok_->clicked().connect(this, &CompetitionCreateWidget::button_handler_);
    }
    else
    {
        new Wt::WText(tr("thechess.competition.cant_change"), this);
    }
    t.commit();
}

Wt::EventSignal<Wt::WMouseEvent>& CompetitionCreateWidget::saved()
{
    return ok_->clicked();
}

void CompetitionCreateWidget::button_handler_()
{
    dbo::Transaction t(tApp->session());
    CompetitionPtr comp = c_ ? c_ : tApp->session().add(new Competition(true));
    cpw_->apply_parameters(comp.modify());
    comp.modify()->create_competition(tApp->user());
    t.commit();
    tApp->view(comp);
    tApp->server().tracker().add_or_update_task(Object(CompetitionObject, comp.id()));
}

}
}

