/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <sstream>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <Wt/WEnvironment>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/WTreeTable>
#include <Wt/WTreeTableNode>
#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Wt/WLineEdit>
#include <Wt/WIntValidator>
#include <Wt/WCompositeWidget>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WViewWidget>
#include <Wt/Dbo/Transaction>
#include <Wt/Wc/util.hpp>

#include "widgets/competition/CompetitionWidget.hpp"
#include "widgets/competition/StagedCompetitionGraph.hpp"
#include "widgets/competition/CompetitionCreateWidget.hpp"
#include "widgets/user/user_anchor.hpp"
#include "widgets/comment/CommentList.hpp"
#include "widgets/comment/comment_base.hpp"
#include "Application.hpp"
#include "model/all.hpp"
#include "config.hpp"
#include "log.hpp"

namespace thechess {

class CompetitionMembers : public Wt::WContainerWidget {
public:
    CompetitionMembers(const CompetitionPtr& c, const UsersVector& members):
        c_(c) {
        setList(true);
        if (c_->type() == CLASSICAL || c_->type() == STAGED) {
            BOOST_FOREACH (const UserPtr& user, members) {
                add_user_to_list(user, this);
            }
        } else if (c_->type() == TEAM) {
            tcm_map_team_to_users(t2u_, c);
            // list of teams here
            TeamsVector teams(c->teams().begin(), c->teams().end());
            BOOST_FOREACH (const TeamPtr& team, teams) {
                add_team_to_list(team, this);
            }
            t2u_.clear();
        }
    }

private:
    CompetitionPtr c_;
    Team2Users t2u_;

    void kick(UserPtr user) {
        dbo::Transaction t(tApp->session());
        c_.reread();
        c_.modify()->kick(tApp->user(), user);
        admin_log("Discard " + user_a(user.id()) + " from " + comp_a(c_.id()));
        t.commit();
        t_emit(COMPETITION, c_.id());
    }

    void remove_team(TeamPtr team) {
        dbo::Transaction t(tApp->session());
        c_.reread();
        c_.modify()->remove_team(tApp->user(), team);
        admin_log("Remove " + team_a(team.id()) + " from " + comp_a(c_.id()));
        t.commit();
        t_emit(COMPETITION, c_.id());
    }

    void team_join(TeamPtr team) {
        dbo::Transaction t(tApp->session());
        c_.reread();
        c_.modify()->team_join(tApp->user(), team);
        t.commit();
        t_emit(COMPETITION, c_.id());
    }

    void add_user_to_list(const UserPtr& user, Wt::WContainerWidget* list) {
        Wt::WContainerWidget* item = new Wt::WContainerWidget(list);
        user_anchor(user, item);
        if (c_->can_kick(tApp->user(), user)) {
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.common.Discard"), item);
            b->clicked().connect(boost::bind(&CompetitionMembers::kick,
                                             this, user));
        }
    }

    void add_team_to_list(const TeamPtr& team, Wt::WContainerWidget* list) {
        Wt::WContainerWidget* item = new Wt::WContainerWidget(list);
        item->addWidget(team_anchor(team.id()));
        if (c_->can_remove_team(tApp->user(), team)) {
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.common.Discard"), item);
            b->clicked().connect(boost::bind(&CompetitionMembers::remove_team,
                                             this, team));
        }
        if (c_->can_team_join(tApp->user(), team)) {
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.common.Join"), item);
            b->clicked().connect(boost::bind(&CompetitionMembers::team_join,
                                             this, team));
        }
        Wt::WContainerWidget* sub_list = new Wt::WContainerWidget(item);
        sub_list->setList(true);
        BOOST_FOREACH (const UserPtr& user, t2u_[team]) {
            add_user_to_list(user, sub_list);
        }
    }
};

class CompetitionWinners : public Wt::WContainerWidget {
public:
    CompetitionWinners(const CompetitionPtr& c) {
        setList(true);
        if (c->type() == TEAM) {
            TeamsVector w(c->winner_teams().begin(), c->winner_teams().end());
            BOOST_FOREACH (const TeamPtr& team, w) {
                Wt::WContainerWidget* item = new Wt::WContainerWidget(this);
                item->addWidget(team_anchor(team.id()));
            }
        } else if (c->type() == CLASSICAL || c->type() == STAGED) {
            BOOST_FOREACH (const UserPtr& user, c->winners_vector()) {
                Wt::WContainerWidget* item = new Wt::WContainerWidget(this);
                user_anchor(user, item);
            }
        }
    }
};

class CompetitionTerms : public Wt::WContainerWidget {
public:
    CompetitionTerms(const CompetitionPtr& c) {
        using namespace config::competition::defaults;
        CPPtr cp = c->cp();
        GPPtr gp = c->gp();
        kw("tc.competition.Rating", tr("tc.common.interval")
           .arg(cp->min_rating()).arg(cp->max_rating()));
        if (cp->min_classification() != MIN_CLASSIFICATION ||
                cp->max_classification() != MAX_CLASSIFICATION) {
            kw("tc.competition.Members_classification", tr("tc.common.interval")
               .arg(User::classification2str(cp->min_classification()))
               .arg(User::classification2str(cp->max_classification())));
        }
        if (cp->min_online_time() > SECOND || cp->max_online_time() >= SECOND) {
            kw("tc.competition.Online_time", tr("tc.common.interval")
               .arg(td2str(cp->min_online_time()))
               .arg(td2str(cp->max_online_time())));
        }
        kw("tc.competition.Force_start_delay", td2str(cp->force_start_delay()));
        if (c->type() == CLASSICAL || c->type() == STAGED) {
            kw("tc.competition.Users", tr("tc.common.interval")
               .arg(cp->min_users()).arg(cp->max_users()));
        }
        kw("tc.competition.Recruiting_time", tr("tc.common.interval")
           .arg(td2str(cp->min_recruiting_time()))
           .arg(td2str(cp->max_recruiting_time())));
        if (c->type() == CLASSICAL || c->type() == TEAM) {
            kw("tc.competition.Max_simultaneous_games",
               cp->max_simultaneous_games());
            boost::format f("%.3f");
            kw("tc.competition.Games_factor", str(f % cp->games_factor()));
        }
        kw("tc.competition.Relax_time", td2str(cp->relax_time()));
        if (c->type() == STAGED) {
            kw("tc.competition.Min_substages", cp->min_substages());
            kw("tc.competition.Increment_substages", cp->increment_substages());
        }
        {
            Wt::WAnchor* a = new Wt::WAnchor(this);
            a->setText(tr("tc.game.Parameters") + " " + TO_S(gp.id()));
            a->setLink(tApp->path().gp_view()->get_link(gp.id()));
            new Wt::WBreak(this);
        }
        {
            Wt::WAnchor* a = new Wt::WAnchor(this);
            a->setText(tr("tc.competition.Parameters") + " " + TO_S(cp.id()));
            a->setLink(tApp->path().cp_view()->get_link(cp.id()));
            new Wt::WBreak(this);
        }
    }

    template<typename T>
    void kw(const char* tr_id, const T& s) {
        new Wt::WText(tr("tc.common.kw").arg(tr(tr_id)).arg(s), this);
        new Wt::WBreak(this);
    }
};

void game_reference(const GamePtr& game, Wt::WContainerWidget* c) {
    if (c->count()) {
        new Wt::WText(", ", c);
    }
    Wt::WAnchor* a = new Wt::WAnchor(c);
    a->setText(TO_S(game.id()));
    a->setLink(tApp->path().game_view()->get_link(game.id()));
}

enum {
    NAME_COLUMN,
    SCORE_COLUMN,
    N_COLUMN
};

const int N_ROW = 0;
const int LEFT_SHIFT = N_COLUMN + 1;
const int TOP_SHIFT = 1;

class ClassicalViewImpl : public Wt::WContainerWidget {
public:
    ClassicalViewImpl(const CompetitionPtr& c, bool show_wins):
        c_(c), games_(c->games_vector()), show_wins_(show_wins) {
        gt_ = c->games_table();
        table_ = new Wt::WTable(this);
        table_->setStyleClass("thechess-table-border");
        members_ = c->members_vector();
        Competition::wins_number(games_, user_wins_);
        if (c->type() == CLASSICAL) {
            std::sort(members_.begin(), members_.end(),
                      boost::bind(&ClassicalViewImpl::compare_users_classical,
                                  this, _1, _2));
        } else if (c->type() == TEAM) {
            tcm_map_user_to_team(u2t_, c);
            BOOST_FOREACH (const User2Team::value_type& u_t, u2t_) {
                const TeamPtr& team = u_t.second;
                team_members_[team] += 1;
            }
            teams_ = TeamsVector(c->teams().begin(), c->teams().end());
            Competition::team_wins_number(user_wins_, u2t_, team_wins_);
            std::sort(teams_.begin(), teams_.end(),
                      boost::bind(&ClassicalViewImpl::compare_teams,
                                  this, _1, _2));
            for (int i = 0; i < teams_.size(); i++) {
                const TeamPtr& team = teams_[i];
                team_index_[team] = i;
            }
            std::sort(members_.begin(), members_.end(),
                      boost::bind(&ClassicalViewImpl::compare_users_team,
                                  this, _1, _2));
            BOOST_FOREACH (const UserPtr& user, members_) {
                const TeamPtr& team = u2t_[user];
                if (team_first_member_.find(team) == team_first_member_.end()) {
                    team_first_member_[team] = user;
                }
            }
        }
        BOOST_FOREACH (const GamePtr& game, games_) {
            if (game->is_ended()) {
                user_ended_[game->white()] += 1;
                user_ended_[game->black()] += 1;
            }
        }
        headers();
        scores(c);
        fill_table();
    }

private:
    CompetitionPtr c_;
    Wt::WTable* table_;
    GamesTable gt_;
    UsersVector members_;
    GamesVector games_;
    bool show_wins_;
    mutable Competition::User2float user_wins_;
    mutable Competition::User2float user_ended_;
    mutable Competition::Team2float team_wins_;
    mutable TeamsVector teams_;
    mutable User2Team u2t_;
    mutable std::map<TeamPtr, int> team_index_;
    mutable std::map<TeamPtr, int> team_members_;
    mutable std::map<TeamPtr, UserPtr> team_first_member_;

    bool compare_users_classical(const UserPtr& a, const UserPtr& b) const {
        return user_wins_[a] > user_wins_[b];
    }

    bool compare_teams(const TeamPtr& a, const TeamPtr& b) const {
        return team_wins_[a] > team_wins_[b];
    }

    bool compare_users_team(const UserPtr& a, const UserPtr& b) const {
        const TeamPtr& a_team = u2t_[a];
        const TeamPtr& b_team = u2t_[b];
        if (a_team != b_team) {
            return team_index_[a_team] < team_index_[b_team];
        } else {
            return user_wins_[a] > user_wins_[b];
        }
    }

    void headers() {
        int i = 0;
        BOOST_FOREACH (const UserPtr& user, members_) {
            std::string i_str = TO_S(i + 1);
            table_->elementAt(i + TOP_SHIFT, NAME_COLUMN)
            ->setStyleClass("thechess-td-right");
            table_->elementAt(i + TOP_SHIFT, NAME_COLUMN)
            ->addWidget(new Wt::WText(user->safe_username()));
            table_->elementAt(i + TOP_SHIFT, N_COLUMN)
            ->addWidget(new Wt::WText(i_str));
            table_->elementAt(N_ROW, i + LEFT_SHIFT)
            ->addWidget(new Wt::WText(i_str));
            i++;
        }
    }

    void scores(const CompetitionPtr& c) {
        int score_right = members_.size() + LEFT_SHIFT;
        table_->elementAt(0, score_right)
        ->addWidget(new Wt::WText(tr("tc.competition.Score")));
        table_->elementAt(0, SCORE_COLUMN)
        ->addWidget(new Wt::WText(tr("tc.competition.Score")));
        int i = 0;
        BOOST_FOREACH (const UserPtr& user, members_) {
            int row = i + TOP_SHIFT;
            std::string w = TO_S(user_wins_[user]) + "/" +
                            TO_S(user_ended_[user]);
            table_->elementAt(row, score_right)->addWidget(new Wt::WText(w));
            table_->elementAt(row, SCORE_COLUMN)->addWidget(new Wt::WText(w));
            i++;
        }
    }

    void fill_table() {
        int members_size = members_.size();
        for (int row = 0; row < members_size; ++row) {
            const UserPtr& urow = members_[row];
            const TeamPtr& trow = u2t_[urow];
            int trow_odd = team_index_[trow] % 2;
            for (int col = 0; col < members_size; ++col) {
                const UserPtr& ucol = members_[col];
                const TeamPtr& tcol = u2t_[ucol];
                Wt::WTableCell* cell = table_->elementAt(row + TOP_SHIFT,
                                       col + LEFT_SHIFT);
                if (c_->type() == TEAM) {
                    int tcol_odd = team_index_[tcol] % 2;
                    Wt::GlobalColor bgcolor = Wt::white;
                    if (!trow_odd && !tcol_odd) {
                        bgcolor = Wt::white;
                    } else if (trow_odd && !tcol_odd) {
                        bgcolor = Wt::lightGray;
                    } else if (!trow_odd && tcol_odd) {
                        bgcolor = Wt::lightGray;
                    } else if (trow_odd && tcol_odd) {
                        bgcolor = Wt::gray;
                    }
                    cell->decorationStyle().setBackgroundColor(bgcolor);
                }
                cell->clear();
                if (row == col) {
                    cell->setVerticalAlignment(Wt::AlignMiddle);
                    if (trow) {
                        if (team_first_member_[trow] == urow) {
                            int members_number = team_members_[trow];
                            cell->setRowSpan(members_number);
                            cell->setColumnSpan(members_number);
                            new Wt::WText(tr("tc.competition.dash"), cell);
                        }
                    } else {
                        new Wt::WText(tr("tc.competition.dash"), cell);
                    }
                } else if (show_wins_) {
                    std::map<UserPtr, float> wins;
                    Competition::wins_number(gt_[urow][ucol], wins);
                    Competition::wins_number(gt_[ucol][urow], wins);
                    new Wt::WText(TO_S(wins[urow]) +
                                  "/" + TO_S(wins[ucol]), cell);
                } else {
                    BOOST_FOREACH (const GamePtr& game, gt_[ucol][urow]) {
                        game_reference(game, cell);
                    }
                    BOOST_FOREACH (const GamePtr& game, gt_[urow][ucol]) {
                        game_reference(game, cell);
                    }
                }
            }
        }
    }
};

class ClassicalViewView : public Wt::WViewWidget {
public:
    ClassicalViewView(const CompetitionPtr& c, bool show_wins):
        c_(c), show_wins_(show_wins)
    { }

    void set_show_wins(bool show_wins) {
        show_wins_ = show_wins;
        update();
    }

    virtual Wt::WWidget* renderView() {
        dbo::Transaction t(tApp->session());
        return new ClassicalViewImpl(c_, show_wins_);
    }

private:
    CompetitionPtr c_;
    bool show_wins_;
};

class ClassicalView : public Wt::WContainerWidget {
public:
    ClassicalView(const CompetitionPtr& c):
        show_wins_(!User::has_s(SWITCH_GAME_NUMBERS_IN_TABLE)) {
        view_ = new ClassicalViewView(c, show_wins_);
        addWidget(view_);
        fill_button_ = new Wt::WPushButton(this);
        fill_button_->clicked().connect(this, &ClassicalView::change_fill_type);
        update_button_text();
    }

private:
    ClassicalViewView* view_;
    Wt::WPushButton* fill_button_;
    bool show_wins_;

    void change_fill_type() {
        show_wins_ = !show_wins_;
        User::set_s(SWITCH_GAME_NUMBERS_IN_TABLE, !show_wins_);
        view_->set_show_wins(show_wins_);
        update_button_text();
    }

    void update_button_text() {
        if (show_wins_) {
            fill_button_->setText(tr("tc.competition.Show_games"));
        } else {
            fill_button_->setText(tr("tc.competition.Show_wins"));
        }
    }
};

const int STAGE_COLUMN = 1;
const int GAMES_COLUMN = 2;

class StagedTree : public Wt::WTreeTable {
public:
    StagedTree(const StagedCompetition* s):
        sc_(*s), competitors_(sc_.competitors()) {
        resize(650, 300);
        addColumn(tr("tc.competition.Stage"), 100);
        addColumn(tr("tc.competition.Games_list"), 300);
        Wt::WTreeTableNode* r;
        r = new Wt::WTreeTableNode(tr("tc.competition.Tree"));
        setTreeRoot(r, tr("tc.competition.Winner"));
        typedef StagedCompetition::Paires::value_type SAP;
        BOOST_FOREACH (const SAP& stage_and_pair, sc_.paires()) {
            int stage = stage_and_pair.first;
            const UserPair& pair = stage_and_pair.second;
            if (sc_.winners().find(pair) == sc_.winners().end()) {
                print_pair(stage, pair, r);
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
        r->expand();
    }

private:
    const StagedCompetition& sc_;
    std::map<int, std::map<UserPtr, UserPtr> > competitors_;

    void print_pair(int stage, const UserPair& pair,
                    Wt::WTreeTableNode* parent) {
        Wt::WString title;
        typedef StagedCompetition::Winners::const_iterator It;
        It winner = sc_.winners().find(pair);
        if (winner == sc_.winners().end() || stage == 0)
            title = tr("tc.competition.user_pair")
                    .arg(pair.first()->safe_username())
                    .arg(pair.second()->safe_username());
        else {
            title = winner->second->safe_username();
        }
        Wt::WTreeTableNode* n = new Wt::WTreeTableNode(title, 0, parent);
        n->setColumnWidget(STAGE_COLUMN,
                           new Wt::WText(TO_S(stage + 1)));
        Wt::WContainerWidget* games = new Wt::WContainerWidget();
        StagedCompetition::Games::const_iterator g = sc_.games().find(pair);
        if (g != sc_.games().end()) {
            BOOST_FOREACH (const GamePtr& game, g->second) {
                game_reference(game, games);
            }
        }
        n->setColumnWidget(GAMES_COLUMN, games);
        if (stage > 0) {
            print(stage - 1, pair.first(), n);
            print(stage - 1, pair.second(), n);
        }
        n->expand();
    }

    void print_user(int stage, const UserPtr& user,
                    Wt::WTreeTableNode* parent) {
        Wt::WTreeTableNode* n;
        n = new Wt::WTreeTableNode(user->safe_username(), 0, parent);
        n->setColumnWidget(STAGE_COLUMN,
                           new Wt::WText(TO_S(stage + 1)));
        if (stage > 0) {
            print(stage - 1, user, n);
        }
        n->expand();
    }

    void print(int stage, const UserPtr& user, Wt::WTreeTableNode* parent) {
        const UserPtr& competitor = competitors_[stage][user];
        if (competitor) {
            print_pair(stage, UserPair(user, competitor), parent);
        } else {
            print_user(stage, user, parent);
        }
    }
};

class StagedTreeView : public Wt::WViewWidget {
public:
    StagedTreeView(const StagedCompetition* sc):
        sc_(sc)
    { }

    virtual Wt::WWidget* renderView() {
        return new StagedTree(sc_);
    }

private:
    const StagedCompetition* sc_;
};

class StagedWidget : public Wt::WContainerWidget {
public:
    StagedWidget(const CompetitionPtr& c):
        sc_(c.get()) {
        if (wApp->environment().ajax()) {
            addWidget(new StagedTreeView(&sc_));
        } else {
            // FIXME expand doesn't work in both, but in JS it is pre-learned
            addWidget(new StagedTree(&sc_));
        }
        addWidget(new Wt::WImage(new StagedCompetitionGraph(&sc_, this)));
        // FIXME parent of graph http://redmine.emweb.be/issues/1126
    }

private:
    StagedCompetition sc_;
};

class CompetitionView : public Wt::WCompositeWidget {
public:
    CompetitionView(const CompetitionPtr& c) {
        if (c->state() == Competition::ACTIVE ||
                c->state() == Competition::ENDED) {
            if (c->type() == CLASSICAL || c->type() == TEAM) {
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
                button<&Competition::join>("tc.common.Join");
            } else if (c->can_leave(tApp->user())) {
                button<&Competition::leave>("tc.game.Leave");
            }
            if (c->can_change_parameters(tApp->user())) {
                Wt::WPushButton* change =
                    new Wt::WPushButton(tr("tc.competition.Change"), this);
                change->clicked()
                .connect(this, &CompetitionManager::show_change_widget);
            }
            if (c->can_cancel(tApp->user())) {
                button<&Competition::cancel>("tc.common.Cancel");
            }
            if (c->can_force_start(tApp->user())) {
                button<&Competition::force_start>("tc.competition.Force_start");
            }
            if (c->state() == Competition::RECRUITING && c->type() == TEAM) {
                new Wt::WBreak(this);
                team_id_ = new Wt::WLineEdit(this);
                team_id_->setValidator(new Wt::WIntValidator);
                Wt::WPushButton* b;
                b = new Wt::WPushButton(tr("tc.competition.Add_team"), this);
                b->clicked().connect(this, &CompetitionManager::add_team);
            }
        }
        bool admin = tApp->user() && tApp->user()->admin_rights();
        if (c->has_virtuals() && (!c->virtual_allower() || admin)) {
            new Wt::WBreak(this);
            new Wt::WText(tr("tc.competition.Virtuals"), this);
            if (tApp->user() && tApp->user()->has_permission(VIRTUALS_VIEWER)) {
                BOOST_FOREACH (UserPtr user, c->virtuals()) {
                    user_anchor(user, this);
                }
                new Wt::WBreak(this);
                Wt::WAnchor* a = new Wt::WAnchor(this);
                a->setLink(tApp->path().virtuals_in_competition()->link());
                a->setText(tr("tc.user.Virtuals_list"));
            }
            if (c->virtual_allower()) {
                new Wt::WBreak(this);
                new Wt::WText(tr("tc.competition.Virtuals_allowed_by"), this);
                user_anchor(c->virtual_allower(), this);
            } else if (c->can_allow_virtuals(tApp->user())) {
                button<&Competition::allow_virtuals>("tc.common.Allow");
            }
        }
        if (c->state() != Competition::RECRUITING) {
            new Wt::WBreak(this);
            Wt::WAnchor* a = new Wt::WAnchor(this);
            a->setLink(tApp->path().games_of_competition()->link());
            a->setText(tr("tc.game.List"));
        }
    }

    bool is_editing() const {
        return is_editing_;
    }

private:
    CompetitionPtr c_;
    bool is_editing_;
    Wt::WLineEdit* team_id_;

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
        tApp->user().reread();
        (c_.modify()->*method)(tApp->user());
        if (method == &Competition::cancel) {
            admin_log("Cancel " + comp_a(c_.id()));
        } else if (method == &Competition::allow_virtuals) {
            admin_log("Allow virtuals in " + comp_a(c_.id()));
        } else if (method == &Competition::force_start) {
            admin_log("Force start " + comp_a(c_.id()));
        }
        t.commit();
        t_task(COMPETITION, c_.id());
    }

    template <CompetitionMethod method>
    void button(const char* title_id) {
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr(title_id), this);
        b->clicked().connect(this, &CompetitionManager::action<method>);
    }

    void save_handler() {
        is_editing_ = false;
    }

    void add_team() {
        if (!tApp->user()) {
            return;
        }
        if (team_id_->validate() != Wt::WValidator::Valid) {
            return;
        }
        dbo::Transaction t(tApp->session());
        c_.reread();
        tApp->user().reread();
        int team_id = boost::lexical_cast<int>(team_id_->text());
        TeamPtr team;
        try {
            team = tApp->session().load<Team>(team_id);
        } catch (...) {
            team_id_->setText(tr("tc.competition.Team_not_found"));
            return;
        }
        if (!c_->can_add_team(tApp->user(), team)) {
            team_id_->setText(tr("tc.competition.Cant_add_team"));
            return;
        }
        c_.modify()->add_team(tApp->user(), team);
        admin_log("Add " + team_a(team.id()) + " in " + comp_a(c_.id()));
        t.commit();
        t_task(COMPETITION, c_.id());
    }
};

class CompetitionChat : public Wt::WContainerWidget {
public:
    CompetitionChat(const CompetitionPtr& c):
        c_(c), chat_(false) {
        print_comment_list();
    }

    void print_comment_list() {
        if (chat_) {
            return;
        }
        clear();
        dbo::Transaction t(tApp->session());
        if (c_->has_comment_base()) {
            print_comment_list_impl();
        } else {
            new Wt::WBreak(this);
            Wt::WPushButton* add = new Wt::WPushButton(this);
            add->setText(tr("tc.comment.Add"));
            add->clicked().connect(this,
                                   &CompetitionChat::print_comment_list_impl);
            add->clicked().connect(add, &Wt::WWidget::hide);
            add->clicked().connect(add, &Wt::WWidget::disable);
        }
    }

private:
    CompetitionPtr c_;
    bool chat_;

    void print_comment_list_impl() {
        dbo::Transaction t(tApp->session());
        CommentPtr comment_base = c_->comment_base();
        if (!comment_base) {
            c_.reread();
            comment_base = c_.modify()->comment_base();
            t.commit();
            t_emit(COMPETITION, c_.id());
        } else {
            chat_ = true;
            addWidget(new CommentList(Comment::CHAT_MESSAGE, comment_base));
        }
    }
};

CompetitionWidget::CompetitionWidget(const CompetitionPtr& competition,
                                     Wt::WContainerWidget* p):
    Wt::WTemplate(tr("tc.competition.widget_template"), p),
    Notifiable(Object(COMPETITION, competition.id())),
    c(competition) {
    reprint();
}

void CompetitionWidget::reprint() {
    dbo::Transaction t(tApp->session());
    c.reread();
    UsersVector members = c->members_vector();
    bindInt("id", c.id());
    bindString("type", Competition::type2str(c->type()));
    bindString("name", c->name(), Wt::PlainText);
    bindString("description", c->description());
    bindString("created", time2str(c->created()));
    bindString("started", time2str(c->started()));
    bindString("ended", time2str(c->ended()));
    bindString("state", tr(Competition::state2str(c->state())));
    bindString("n_users", tr("tc.common.N_users").arg(members.size()));
    bindWidget("members", new CompetitionMembers(c, members));
    bindWidget("winners", new CompetitionWinners(c));
    bindWidget("terms", new CompetitionTerms(c));
    bindWidget("view", new CompetitionView(c));
    Wt::WWidget* chat = resolveWidget("chat");
    if (chat) {
        downcast<CompetitionChat*>(chat)->print_comment_list();
    } else {
        bindWidget("chat", new CompetitionChat(c));
    }
    Wt::WWidget* manager = resolveWidget("manager");
    if (!manager || !downcast<CompetitionManager*>(manager)->is_editing()) {
        bindWidget("manager", new CompetitionManager(c));
    }
}

void CompetitionWidget::notify(EventPtr) {
    reprint();
}

}

