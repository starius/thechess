/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */


#include <sstream>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/WTreeTable>
#include <Wt/WTreeTableNode>
#include <Wt/WAnchor>
#include <Wt/WCompositeWidget>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;

#include "widgets/CompetitionWidget.hpp"
#include "widgets/CompetitionCreateWidget.hpp"
#include "widgets/PleaseLoginWidget.hpp"
#include "ThechessApplication.hpp"
#include "model/StagedCompetition.hpp"

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
        kw("tc.competition.Rating", tr("tc.common.interval")
            .arg(c->min_rating()).arg(c->max_rating()));
        kw("tc.competition.Members_classification", tr("tc.common.interval")
            .arg(classification2str(c->min_classification()))
            .arg(classification2str(c->max_classification())));
        kw("tc.competition.Force_start_delay", td2str(c->force_start_delay()));
        if (c->type() == CLASSICAL || c->type() == STAGED)
        {
            kw("tc.competition.Users", tr("tc.common.interval")
                .arg(c->min_users()).arg(c->max_users()));
            kw("tc.competition.Recruiting_time", tr("tc.common.interval")
                .arg(td2str(c->min_recruiting_time())).arg(td2str(c->max_recruiting_time())));
        }
        if (c->type() == CLASSICAL)
        {
            kw("tc.competition.Max_simultaneous_games", c->max_simultaneous_games());
            kw("tc.competition.Games_factor", c->games_factor());
        }
        if (c->type() == STAGED)
        {
            kw("tc.competition.Relax_time", td2str(c->relax_time()));
            kw("tc.competition.Min_substages", c->min_substages());
            kw("tc.competition.Increment_substages", c->increment_substages());
        }
    }

    template<typename T>
    void kw(const char* tr_id, const T& s)
    {
        new Wt::WText(tr("tc.common.kw").arg(tr(tr_id)).arg(s), this);
        new Wt::WBreak(this);
    }
};

void game_reference_(GamePtr game, Wt::WContainerWidget* c)
{
    if (c->count())
        new Wt::WText(", ", c);
    Wt::WAnchor* a = new Wt::WAnchor(c);
    a->setText(boost::lexical_cast<std::string>(game.id()));
    a->setRefInternalPath(str(boost::format("/game/%i/") % game.id()));
}

const int NAME_COLUMN = 0;
const int N_COLUMN = 1;
const int N_ROW = 0;
const int LEFT_SHIFT = 1;
const int TOP_SHIFT = 2;

class ClassicalView : public Wt::WContainerWidget
{
public:
    ClassicalView(CompetitionPtr c):
    show_wins_(true)
    {
        gt_ = c->games_table();
        table_ = new Wt::WTable(this);
        table_->setStyleClass("thechess-table-border");
        members = c->members_vector();
        score_column_ = members.size() + TOP_SHIFT;
        headers_();
        scores_(c);
        fill_button_ = new Wt::WPushButton(this);
        fill_button_->clicked().connect(this, &ClassicalView::change_fill_type_);
        fill_table_();
    }

private:
    Wt::WTable* table_;
    GamesTable gt_;
    UsersVector members;
    int score_column_;
    bool show_wins_;
    Wt::WPushButton* fill_button_;

    void headers_()
    {
        table_->elementAt(0, 0)->setColumnSpan(2);
        int i = 0;
        BOOST_FOREACH(UserPtr user, members)
        {
            std::string i_str = boost::lexical_cast<std::string>(i+1);
            table_->elementAt(i + LEFT_SHIFT, NAME_COLUMN)
                ->setStyleClass("thechess-td-right");
            table_->elementAt(i + LEFT_SHIFT, NAME_COLUMN)
                ->addWidget(new Wt::WText(user->username()));
            table_->elementAt(i + LEFT_SHIFT, N_COLUMN)
                ->addWidget(new Wt::WText(i_str));
            table_->elementAt(N_ROW, i + TOP_SHIFT)
                ->addWidget(new Wt::WText(i_str));
            i++;
        }
    }

    void scores_(CompetitionPtr c)
    {
        table_->elementAt(0, score_column_)
            ->addWidget(new Wt::WText(tr("tc.competition.Score")));
        std::map<UserPtr, float> wins;
        Competition::wins_number(c->games_vector(), wins);
        int i = 0;
        BOOST_FOREACH(UserPtr user, members)
        {
            table_->elementAt(i + LEFT_SHIFT, score_column_)
                ->addWidget(new Wt::WText(boost::lexical_cast<std::string>(wins[user])));
            i++;
        }
    }

    void change_fill_type_()
    {
        show_wins_ = !show_wins_;
        fill_table_();
    }

    void fill_table_()
    {
        if (show_wins_)
            fill_button_->setText(tr("tc.competition.Show_games"));
        else
            fill_button_->setText(tr("tc.competition.Show_wins"));
        int members_size = members.size();
        for (int row = 0; row < members_size; ++row)
        {
            UserPtr urow = members[row];
            for (int col = 0; col < members_size; ++col)
            {
                UserPtr ucol = members[col];
                Wt::WTableCell* cell = table_->elementAt(row + LEFT_SHIFT, col + TOP_SHIFT);
                cell->clear();
                if (row == col)
                {
                    new Wt::WText(tr("tc.competition.dash"), cell);
                }
                else if (show_wins_)
                {
                    std::map<UserPtr, float> wins;
                    Competition::wins_number(gt_[urow][ucol], wins);
                    Competition::wins_number(gt_[ucol][urow], wins);
                    new Wt::WText(boost::lexical_cast<std::string>(wins[urow]) +
                        "/" + boost::lexical_cast<std::string>(wins[ucol]), cell);
                }
                else
                {
                    BOOST_FOREACH(GamePtr game, gt_[ucol][urow])
                        game_reference_(game, cell);
                    BOOST_FOREACH(GamePtr game, gt_[urow][ucol])
                        game_reference_(game, cell);
                }
            }
        }
    }
};

const int STAGE_COLUMN = 1;
const int GAMES_COLUMN = 2;

class StagedView : public Wt::WTreeTable
{
public:
    StagedView(CompetitionPtr c):
    c_(c), sc_(&*c)
    {
        calculate_competitors_();
        resize(650, 300);
        addColumn(tr("tc.competition.Stage"), 100);
        addColumn(tr("tc.competition.Games_list"), 300);
        Wt::WTreeTableNode* r = new Wt::WTreeTableNode(tr("tc.competition.Tree"));
        r->expand();
        setTreeRoot(r, tr("tc.competition.Winner"));
        BOOST_FOREACH(const StagedCompetition::Paires::value_type& stage_and_pair, sc_.paires())
        {
            int stage = stage_and_pair.first;
            const UserPair& pair = stage_and_pair.second;
            if (sc_.winners().find(pair) == sc_.winners().end())
                print_pair_(stage, pair, r);
        }
        BOOST_FOREACH(const StagedCompetition::States::value_type& user_and_state, sc_.states())
        {
            UserPtr user = user_and_state.first;
            StagedCompetition::State state = user_and_state.second;
            if (state == StagedCompetition::UNPAIRED || state == StagedCompetition::WINNER)
                print_(sc_.stages().find(user)->second-1, user, r);
        }
    }

private:
    CompetitionPtr c_;
    StagedCompetition sc_;
    std::map<int, std::map<UserPtr, UserPtr> > competitors_;

    void calculate_competitors_()
    {
        BOOST_FOREACH(const StagedCompetition::Paires::value_type& stage_and_pair, sc_.paires())
        {
            int stage = stage_and_pair.first;
            const UserPair& pair = stage_and_pair.second;
            competitors_[stage][pair.first()] = pair.second();
            competitors_[stage][pair.second()] = pair.first();
        }
    }

    void print_pair_(int stage, const UserPair& pair, Wt::WTreeTableNode* parent)
    {
        Wt::WString title;
        StagedCompetition::Winners::const_iterator winner = sc_.winners().find(pair);
        if (winner == sc_.winners().end() || stage == 0)
            title = tr("tc.competition.user_pair")
                .arg(pair.first()->username()).arg(pair.second()->username());
        else
            title = winner->second->username();
        Wt::WTreeTableNode* n = new Wt::WTreeTableNode(title, 0, parent);
        n->setColumnWidget(STAGE_COLUMN,
            new Wt::WText(boost::lexical_cast<std::string>(stage+1)));
        Wt::WContainerWidget* games = new Wt::WContainerWidget();
        StagedCompetition::Games::const_iterator g = sc_.games().find(pair);
        if (g != sc_.games().end())
        {
            BOOST_FOREACH(GamePtr game, g->second)
                game_reference_(game, games);
        }
        n->setColumnWidget(GAMES_COLUMN, games);
        if (stage > 0)
        {
            print_(stage-1, pair.first(), n);
            print_(stage-1, pair.second(), n);
        }
    }

    void print_user_(int stage, UserPtr user, Wt::WTreeTableNode* parent)
    {
        Wt::WTreeTableNode* n = new Wt::WTreeTableNode(user->username(), 0, parent);
        n->setColumnWidget(STAGE_COLUMN,
            new Wt::WText(boost::lexical_cast<std::string>(stage+1)));
        if (stage > 0)
            print_(stage-1, user, n);
    }

    void print_(int stage, UserPtr user, Wt::WTreeTableNode* parent)
    {
        UserPtr competitor = competitors_[stage][user];
        if (competitor)
            print_pair_(stage, UserPair(user, competitor), parent);
        else
            print_user_(stage, user, parent);
    }
};

class CompetitionView : public Wt::WCompositeWidget
{
public:
    CompetitionView(CompetitionPtr c)
    {
        if (c->state() >= Competition::ACTIVE)
        {
            if (c->type() == CLASSICAL)
                setImplementation(new ClassicalView(c));
            if (c->type() == STAGED)
                setImplementation(new StagedView(c));
        }
        if (!implementation())
            setImplementation(new Wt::WContainerWidget());
    }
};

class CompetitionManager : public Wt::WContainerWidget
{
public:
    CompetitionManager(CompetitionPtr c):
    c_(c), is_editing_(false)
    {
        if (tApp->user())
        {
            if (c->can_join(tApp->user()))
            {
                button_<&Competition::join>("tc.common.Join");
            }
            else if (c->can_leave(tApp->user()))
            {
                button_<&Competition::leave>("tc.game.Leave");
            }
            if (c->can_change_parameters(tApp->user()))
            {
                Wt::WPushButton* change =
                    new Wt::WPushButton(tr("tc.competition.Change"), this);
                change->clicked().connect(this,
                    &CompetitionManager::show_change_widget_);
            }
        }
        else
        {
            new PleaseLoginWidget(this);
        }
    }

    bool is_editing() const { return is_editing_; }

private:
    CompetitionPtr c_;
    bool is_editing_;

    void show_change_widget_()
    {
        dynamic_cast<Wt::WPushButton*>(sender())->hide();
        is_editing_ = true;
        CompetitionCreateWidget* ccw = new CompetitionCreateWidget(c_, this);
        ccw->saved().connect(this, &CompetitionManager::save_handler_);
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

    void save_handler_()
    {
        is_editing_ = false;
    }
};

CompetitionWidget::CompetitionWidget(CompetitionPtr competition,
    Wt::WContainerWidget* p):
Wt::WTemplate(tr("tc.competition.widget_template"), p),
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
    bindWidget("view", new CompetitionView(c));
    if (!resolveWidget("manager") ||
        !dynamic_cast<CompetitionManager*>(resolveWidget("manager"))->is_editing())
        bindWidget("manager", new CompetitionManager(c));
    t.commit();
}

void CompetitionWidget::notify()
{
    reprint_();
}

}
}

