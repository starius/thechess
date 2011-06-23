
#include <boost/foreach.hpp>

#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;

#include "widgets/CompetitionWidget.hpp"
#include "widgets/CompetitionCreateWidget.hpp"
#include "ThechessApplication.hpp"

namespace thechess {
namespace widgets {
using namespace model;

class CompetitionMembers : public Wt::WContainerWidget
{
public:
    CompetitionMembers(CompetitionPtr c)
    {
        setList(true);
        BOOST_FOREACH(UserPtr user, c->members_vector())
        {
            Wt::WContainerWidget* item = new Wt::WContainerWidget(this);
            new Wt::WText(user->username(), item);
        }
    }
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

class CompetitionManager : public Wt::WContainerWidget
{
public:
    CompetitionManager(CompetitionPtr c):
    c_(c)
    {
        if (c->can_join(tApp->user()))
        {
            button_<&Competition::join>("thechess.join");
        }
        else if (c->can_leave(tApp->user()))
        {
            button_<&Competition::leave>("thechess.leave");
        }
        if (c->can_change_parameters(tApp->user()))
        {
            Wt::WPushButton* change = new Wt::WPushButton(tr("thechess.competition.change"), this);
            change->clicked().connect(this, &CompetitionManager::show_change_widget_);
        }
    }

private:
    CompetitionPtr c_;

    void show_change_widget_()
    {
        dynamic_cast<Wt::WPushButton*>(sender())->hide();
        new CompetitionCreateWidget(c_, this);
    }

    typedef void (Competition::*CompetitionMethod)(UserPtr);
    template <CompetitionMethod method>
    void action_()
    {
        dbo::Transaction t(tApp->session());
        c_.reread();
        (c_.modify()->*method)(tApp->user());
        t.commit();
        Object object(CompetitionObject, c_.id());
        tApp->server().tracker().add_or_update_task(object);
    }

    template <CompetitionMethod method>
    void button_(const char* title_id)
    {
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr(title_id), this);
        b->clicked().connect(this, &CompetitionManager::action_<method>);
    }
};

CompetitionWidget::CompetitionWidget(CompetitionPtr competition,
    Wt::WContainerWidget* p):
Wt::WTemplate(tr("thechess.template.CompetitionWidget"), p),
Notifiable(Object(CompetitionObject, competition.id())),
c(competition)
{
    reprint_();
}

void CompetitionWidget::reprint_()
{
    dbo::Transaction t(tApp->session());
    bindInt("id", c.id());
    bindString("name", c->name(), Wt::PlainText);
    bindString("description", c->description());
    bindString("created", c->created().toString());
    bindString("started", c->started().toString());
    bindString("ended", c->ended().toString());
    bindString("state", tr(Competition::state2str(c->state())));

    bindWidget("members", new CompetitionMembers(c));
    bindWidget("winners", new CompetitionWinners());
    bindWidget("terms", new CompetitionTerms());
    bindWidget("view", new CompetitionView());
    bindWidget("manager", new CompetitionManager(c));
    t.commit();
}

void CompetitionWidget::notify()
{
    reprint_();
}

}
}

