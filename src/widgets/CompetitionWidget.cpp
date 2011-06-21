
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;

#include "widgets/CompetitionWidget.hpp"

namespace thechess {
namespace widgets {
using namespace model;

class CompetitionMembers : public Wt::WContainerWidget
{

};

class CompetitionWinners : public Wt::WContainerWidget
{

};

class CompetitionTerms : public Wt::WContainerWidget
{

};

class CompetitionView : public Wt::WContainerWidget
{

};

CompetitionWidget::CompetitionWidget(CompetitionPtr competition,
    Wt::WContainerWidget* p):
Wt::WTemplate(tr("thechess.template.CompetitionWidget"), p),
c(competition)
{
    reprint_();
}

void CompetitionWidget::reprint_()
{
    bindInt("id", c.id());
    bindString("name", c->name(), Wt::PlainText);
    bindString("description", c->description());
    bindString("created", c->created().toString());
    bindString("started", c->started().toString());
    bindString("ended", c->ended().toString());
    bindString("state", tr(Competition::state2str(c->state())));

    bindWidget("members", new CompetitionMembers());
    bindWidget("winners", new CompetitionWinners());
    bindWidget("terms", new CompetitionTerms());
    bindWidget("view", new CompetitionView());
}

}
}

