/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
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
#include <Wt/WImage>
#include <Wt/WCompositeWidget>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/Dbo/Transaction>
#include <Wt/Wc/util.hpp>

#include "widgets/competition/CompetitionWidget.hpp"
#include "widgets/competition/StagedCompetitionGraph.hpp"
#include "widgets/competition/CompetitionCreateWidget.hpp"
#include "Application.hpp"
#include "model/all.hpp"
#include "notify.hpp"
#include "config.hpp"

namespace thechess {

class CompetitionMembers : public Wt::WContainerWidget {
public:
    CompetitionMembers(const CompetitionPtr& c) {
        setList(true);
        BOOST_FOREACH (const UserPtr& user, c->members_vector()) {
            Wt::WContainerWidget* item = new Wt::WContainerWidget(this);
            Wt::WAnchor* a = new Wt::WAnchor(item);
            a->setText(user->safe_username());
            a->setLink(tApp->path().user_view()->get_link(user->id()));
        }
    }
};

class CompetitionWinners : public Wt::WContainerWidget {
public:
    CompetitionWinners(const CompetitionPtr& c) {
        setList(true);
        BOOST_FOREACH (const UserPtr& user, c->winners_vector()) {
            Wt::WContainerWidget* item = new Wt::WContainerWidget(this);
            new Wt::WText(user->safe_username(), item);
        }
    }
};

class CompetitionTerms : public Wt::WContainerWidget {
public:
    CompetitionTerms(const CompetitionPtr& c) {
        using namespace config::competition::defaults;
        CPPtr cp = c->cp();
        kw("tc.competition.Rating", tr("tc.common.interval")
           .arg(cp->min_rating()).arg(cp->max_rating()));
        if (cp->min_classification() != MIN_CLASSIFICATION ||
                cp->max_classification() != MAX_CLASSIFICATION) {
            kw("tc.competition.Members_classification", tr("tc.common.interval")
               .arg(User::classification2str(cp->min_classification()))
               .arg(User::classification2str(cp->max_classification())));
        }
        kw("tc.competition.Force_start_delay", td2str(cp->force_start_delay()));
        if (c->type() == CLASSICAL || c->type() == STAGED) {
            kw("tc.competition.Users", tr("tc.common.interval")
               .arg(cp->min_users()).arg(cp->max_users()));
            kw("tc.competition.Recruiting_time", tr("tc.common.interval")
               .arg(td2str(cp->min_recruiting_time()))
               .arg(td2str(cp->max_recruiting_time())));
        }
        if (c->type() == CLASSICAL) {
            kw("tc.competition.Max_simultaneous_games",
               cp->max_simultaneous_games());
            kw("tc.competition.Games_factor", cp->games_factor());
        }
        if (c->type() == STAGED) {
            kw("tc.competition.Relax_time", td2str(cp->relax_time()));
            kw("tc.competition.Min_substages", cp->min_substages());
            kw("tc.competition.Increment_substages", cp->increment_substages());
        }
    }

    template<typename T>
    void kw(const char* tr_id, const T& s) {
        new Wt::WText(tr("tc.common.kw").arg(tr(tr_id)).arg(s), this);
        new Wt::WBreak(this);
    }
};

void game_reference_(const GamePtr& game, Wt::WContainerWidget* c) {
    if (c->count()) {
        new Wt::WText(", ", c);
    }
    Wt::WAnchor* a = new Wt::WAnchor(c);
    a->setText(TO_S(game.id()));
    a->setLink(tApp->path().game_view()->get_link(game.id()));
}

const int NAME_COLUMN = 0;
const int N_COLUMN = 1;
const int N_ROW = 0;
const int LEFT_SHIFT = 1;
const int TOP_SHIFT = 2;

class ClassicalView : public Wt::WContainerWidget {
public:
    ClassicalView(const CompetitionPtr& c):
        show_wins_(true) {
        gt_ = c->games_table();
        table_ = new Wt::WTable(this);
        table_->setStyleClass("thechess-table-border");
        members = c->members_vector();
        score_column_ = members.size() + TOP_SHIFT;
        headers();
        scores_(c);
        fill_button_ = new Wt::WPushButton(this);
        fill_button_->clicked().connect(this, &ClassicalView::change_fill_type);
        fill_table();
    }

private:
    Wt::WTable* table_;
    GamesTable gt_;
    UsersVector members;
    int score_column_;
    bool show_wins_;
    Wt::WPushButton* fill_button_;

    void headers() {
        table_->elementAt(0, 0)->setColumnSpan(2);
        int i = 0;
        BOOST_FOREACH (const UserPtr& user, members) {
            std::string i_str = TO_S(i + 1);
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

    void scores_(const CompetitionPtr& c) {
        table_->elementAt(0, score_column_)
        ->addWidget(new Wt::WText(tr("tc.competition.Score")));
        std::map<UserPtr, float> wins;
        Competition::wins_number(c->games_vector(), wins);
        int i = 0;
        BOOST_FOREACH (const UserPtr& user, members) {
            int col = i + LEFT_SHIFT;
            Wt::WTableCell* c = table_->elementAt(col, score_column_);
            c->addWidget(new Wt::WText(TO_S(wins[user])));
            i++;
        }
    }

    void change_fill_type() {
        show_wins_ = !show_wins_;
        fill_table();
    }

    void fill_table() {
        if (show_wins_) {
            fill_button_->setText(tr("tc.competition.Show_games"));
        } else {
            fill_button_->setText(tr("tc.competition.Show_wins"));
        }
        int members_size = members.size();
        for (int row = 0; row < members_size; ++row) {
            const UserPtr& urow = members[row];
            for (int col = 0; col < members_size; ++col) {
                const UserPtr& ucol = members[col];
                Wt::WTableCell* cell = table_->elementAt(row + LEFT_SHIFT,
                                       col + TOP_SHIFT);
                cell->clear();
                if (row == col) {
                    new Wt::WText(tr("tc.competition.dash"), cell);
                } else if (show_wins_) {
                    std::map<UserPtr, float> wins;
                    Competition::wins_number(gt_[urow][ucol], wins);
                    Competition::wins_number(gt_[ucol][urow], wins);
                    new Wt::WText(TO_S(wins[urow]) +
                                  "/" + TO_S(wins[ucol]), cell);
                } else {
                    BOOST_FOREACH (const GamePtr& game, gt_[ucol][urow]) {
                        game_reference_(game, cell);
                    }
                    BOOST_FOREACH (const GamePtr& game, gt_[urow][ucol]) {
                        game_reference_(game, cell);
                    }
                }
            }
        }
    }
};

const int STAGE_COLUMN = 1;
const int GAMES_COLUMN = 2;

class StagedView : public Wt::WTreeTable {
public:
    StagedView(const StagedCompetition* s):
        sc_(*s), competitors_(sc_.competitors()) {
        resize(650, 300);
        addColumn(tr("tc.competition.Stage"), 100);
        addColumn(tr("tc.competition.Games_list"), 300);
        Wt::WTreeTableNode* r;
        r = new Wt::WTreeTableNode(tr("tc.competition.Tree"));
        r->expand();
        setTreeRoot(r, tr("tc.competition.Winner"));
        typedef StagedCompetition::Paires::value_type SAP;
        BOOST_FOREACH (const SAP& stage_and_pair, sc_.paires()) {
            int stage = stage_and_pair.first;
            const UserPair& pair = stage_and_pair.second;
            if (sc_.winners().find(pair) == sc_.winners().end()) {
                print_pair_(stage, pair, r);
            }
        }
        typedef StagedCompetition::States::value_type UAS;
        BOOST_FOREACH (const UAS& user_and_state, sc_.states()) {
            const UserPtr& user = user_and_state.first;
            StagedCompetition::State state = user_and_state.second;
            if (state == StagedCompetition::UNPAIRED ||
                    state == StagedCompetition::WINNER) {
                print(sc_.stages().find(user)->second - 1, user, r);
            }
        }
    }

private:
    const StagedCompetition& sc_;
    std::map<int, std::map<UserPtr, UserPtr> > competitors_;

    void print_pair_(int stage, const UserPair& pair,
                     Wt::WTreeTableNode* parent) {
        Wt::WString title;
        typedef StagedCompetition::Winners::const_iterator It;
        It winner = sc_.winners().find(pair);
        if (winner == sc_.winners().end() || stage == 0)
            title = tr("tc.competition.user_pair")
                    .arg(pair.first()->username())
                    .arg(pair.second()->username());
        else {
            title = winner->second->username();
        }
        Wt::WTreeTableNode* n = new Wt::WTreeTableNode(title, 0, parent);
        n->setColumnWidget(STAGE_COLUMN,
                           new Wt::WText(TO_S(stage + 1)));
        Wt::WContainerWidget* games = new Wt::WContainerWidget();
        StagedCompetition::Games::const_iterator g = sc_.games().find(pair);
        if (g != sc_.games().end()) {
            BOOST_FOREACH (const GamePtr& game, g->second) {
                game_reference_(game, games);
            }
        }
        n->setColumnWidget(GAMES_COLUMN, games);
        if (stage > 0) {
            print(stage - 1, pair.first(), n);
            print(stage - 1, pair.second(), n);
        }
    }

    void print_user_(int stage, const UserPtr& user,
                     Wt::WTreeTableNode* parent) {
        Wt::WTreeTableNode* n;
        n = new Wt::WTreeTableNode(user->username(), 0, parent);
        n->setColumnWidget(STAGE_COLUMN,
                           new Wt::WText(TO_S(stage + 1)));
        if (stage > 0) {
            print(stage - 1, user, n);
        }
    }

    void print(int stage, const UserPtr& user, Wt::WTreeTableNode* parent) {
        const UserPtr& competitor = competitors_[stage][user];
        if (competitor) {
            print_pair_(stage, UserPair(user, competitor), parent);
        } else {
            print_user_(stage, user, parent);
        }
    }
};

class StagedWidget : public Wt::WContainerWidget {
public:
    StagedWidget(const CompetitionPtr& c):
        sc_(c.get()) {
        addWidget(new StagedView(&sc_));
        addWidget(new Wt::WImage(new StagedCompetitionGraph(&sc_, this)));
        // FIXME parent of graph http://redmine.emweb.be/issues/1126
    }

private:
    StagedCompetition sc_;
};

class CompetitionView : public Wt::WCompositeWidget {
public:
    CompetitionView(const CompetitionPtr& c) {
        if (c->state() >= Competition::ACTIVE) {
            if (c->type() == CLASSICAL) {
                setImplementation(new ClassicalView(c));
            }
            if (c->type() == STAGED) {
                setImplementation(new StagedWidget(c));
            }
        }
        if (!implementation()) {
            setImplementation(new Wt::WContainerWidget());
        }
    }
};

class CompetitionManager : public Wt::WContainerWidget {
public:
    CompetitionManager(const CompetitionPtr& c):
        c_(c), is_editing_(false) {
        if (tApp->user()) {
            if (c->can_join(tApp->user())) {
                button_<&Competition::join>("tc.common.Join");
            } else if (c->can_leave(tApp->user())) {
                button_<&Competition::leave>("tc.game.Leave");
            }
            if (c->can_change_parameters(tApp->user())) {
                Wt::WPushButton* change =
                    new Wt::WPushButton(tr("tc.competition.Change"), this);
                change->clicked()
                .connect(this, &CompetitionManager::show_change_widget);
            }
        }
    }

    bool is_editing() const {
        return is_editing_;
    }

private:
    CompetitionPtr c_;
    bool is_editing_;

    void show_change_widget() {
        downcast<Wt::WPushButton*>(sender())->hide();
        is_editing_ = true;
        CompetitionCreateWidget* ccw = new CompetitionCreateWidget(c_, this);
        ccw->saved().connect(this, &CompetitionManager::save_handler);
    }

    typedef void (Competition::*CompetitionMethod)(const UserPtr&);
    template <CompetitionMethod method>
    void action() {
        dbo::Transaction t(tApp->session());
        c_.reread();
        (c_.modify()->*method)(tApp->user());
        t.commit();
        tApp->server().planning().add(new Object(COMPETITION, c_.id()), now());
    }

    template <CompetitionMethod method>
    void button_(const char* title_id) {
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr(title_id), this);
        b->clicked().connect(this, &CompetitionManager::action<method>);
    }

    void save_handler() {
        is_editing_ = false;
    }
};

CompetitionWidget::CompetitionWidget(const CompetitionPtr& competition,
                                     Wt::WContainerWidget* p):
    Wt::WTemplate(tr("tc.competition.widget_template"), p),
    Notifiable(Object(COMPETITION, competition.id()), tNot),
    c(competition) {
    reprint();
}

void CompetitionWidget::reprint() {
    dbo::Transaction t(tApp->session());
    c.reread();
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
    Wt::WWidget* manager = resolveWidget("manager");
    if (!manager || !downcast<CompetitionManager*>(manager)->is_editing()) {
        bindWidget("manager", new CompetitionManager(c));
    }
    t.commit();
}

void CompetitionWidget::notify(EventPtr) {
    reprint();
}

}

