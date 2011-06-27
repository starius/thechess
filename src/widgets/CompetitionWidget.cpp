
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
public:
    CompetitionWinners(CompetitionPtr c)
    {
        setList(true);
        BOOST_FOREACH(UserPtr user, c->winners_vector())
        {
            Wt::WContainerWidget* item = new Wt::WContainerWidget(this);
            new Wt::WText(user->username(), item);
        }
    }
};

class CompetitionTerms : public Wt::WContainerWidget
{
public:
    CompetitionTerms(CompetitionPtr c)
    {
        kw("thechess.competition.rating", tr("thechess.format.interval")
            .arg(c->min_rating()).arg(c->max_rating()));
        kw("thechess.competition.classification", tr("thechess.format.interval")
            .arg(classification2str(c->min_classification()))
            .arg(classification2str(c->max_classification())));
        kw("thechess.competition.force_start_delay", td2str(c->force_start_delay()));
        if (c->type() == CLASSICAL || c->type() == STAGED)
        {
            kw("thechess.competition.users", tr("thechess.format.interval")
                .arg(c->min_users()).arg(c->max_users()));
            kw("thechess.competition.recruiting_time", tr("thechess.format.interval")
                .arg(td2str(c->min_recruiting_time())).arg(td2str(c->max_recruiting_time())));
        }
        if (c->type() == CLASSICAL)
        {
            kw("thechess.competition.max_simultaneous_games", c->max_simultaneous_games());
            kw("thechess.competition.games_factor", c->games_factor());
        }
        if (c->type() == STAGED)
        {
            kw("thechess.competition.relax_time", td2str(c->relax_time()));
            kw("thechess.competition.min_substages", c->min_substages());
            kw("thechess.competition.increment_substages", c->increment_substages());
        }
    }

    template<typename T>
    void kw(const char* tr_id, const T& s)
    {
        new Wt::WText(tr("thechess.format.kw").arg(tr(tr_id)).arg(s), this);
        new Wt::WBreak(this);
    }
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
    bindString("type", Competition::type2str(c->type()));
    bindString("name", c->name(), Wt::PlainText);
    bindString("description", c->description());
    bindString("created", c->created().toString());
    bindString("started", c->started().toString());
    bindString("ended", c->ended().toString());
    bindString("state", tr(Competition::state2str(c->state())));

    bindWidget("members", new CompetitionMembers(c));
    bindWidget("winners", new CompetitionWinners(c));
    bindWidget("terms", new CompetitionTerms(c));
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

