
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WPushButton>
#include <Wt/WLineEdit>
#include <Wt/WTextEdit>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;

#include "widgets/CompetitionCreateWidget.hpp"
#include "widgets/PleaseLoginWidget.hpp"
#include "widgets/CompetitionParametersWidget.hpp"
#include "model/Competition.hpp"
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
        CompetitionParameters* cp = new CompetitionParameters(true);
        cpw_ = new CompetitionParametersWidget(cp, /*allow_chechge_type*/ true, this);
        delete cp;

        name_ = new Wt::WLineEdit();
        name_->setMaxLength(config::competition::max_name);
        cpw_->item(tr("thechess.competition.name"), "", name_, name_);
        description_ = new Wt::WTextEdit();
        cpw_->item(tr("thechess.competition.description"), "", description_, description_);

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
    CompetitionPtr comp = tApp->session().add(new Competition(true));
    cpw_->apply_parameters(comp.modify());

    std::wstring name = name_->text().value();
    if (name.length() > config::competition::max_name)
        name.resize(config::competition::max_name);
    comp.modify()->set_name(name);

    std::wstring description = description_->text().value();
    if (description.length() > config::competition::max_description)
        description.resize(config::competition::max_description);
    comp.modify()->set_description(description);

    comp.modify()->create_competition(tApp->user());
    t.commit();
    tApp->view(comp);
}

}
}

