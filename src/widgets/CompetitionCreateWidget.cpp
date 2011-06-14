
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WPushButton>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;

#include "widgets/CompetitionCreateWidget.hpp"
#include "widgets/PleaseLoginWidget.hpp"
#include "widgets/CompetitionParametersWidget.hpp"
#include "model/Competition.hpp"
#include "ThechessApplication.hpp"

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
        CompetitionParameters* cp = CompetitionParameters::create_new();
        cpw_ = new CompetitionParametersWidget(cp, /*allow_chechge_type*/ true, this);
        delete cp;

        new Wt::WBreak(this);
        Wt::WPushButton* ok_ = new Wt::WPushButton(tr("thechess.create"), this);
        ok_->clicked().connect(this, &CompetitionCreateWidget::button_handler_);
    }
    else
    {
        new Wt::WText(tr("thechess.competition.cant_create"), this);
    }
    t.commit();
}

void CompetitionCreateWidget::button_handler_()
{
    dbo::Transaction t(tApp->session());
    CompetitionPtr comp = tApp->session().add(Competition::create_new());
    cpw_->apply_parameters(comp.modify());
    comp.modify()->create_competition(tApp->user());
    t.commit();
    // tApp->view(comp); FIXME
}

}
}

