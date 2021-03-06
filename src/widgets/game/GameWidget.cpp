/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <iostream>
#include <boost/lexical_cast.hpp>

#include <Wt/WEnvironment>
#include <Wt/WLogger>
#include <Wt/WObject>
#include <Wt/WAnchor>
#include <Wt/WContainerWidget>
#include <Wt/WViewWidget>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WInPlaceEdit>
#include <Wt/WDialog>
#include <Wt/WBreak>
#include <Wt/Dbo/Transaction>
#include <Wt/Wc/TimeDurationWidget.hpp>
#include <Wt/Wc/util.hpp>

#include "widgets/Header.hpp"
#include "widgets/game/GameCountdown.hpp"
#include "widgets/game/GameWidget.hpp"
#include "widgets/chess/MovesWidget.hpp"
#include "widgets/chess/LinksDialog.hpp"
#include "widgets/comment/CommentList.hpp"
#include "widgets/comment/comment_base.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "config.hpp"
#include "chess/Moves.hpp"
#include "chess/Piece.hpp"
#include "log.hpp"
#include "config.hpp"

namespace thechess {

class GameStatus : public Wt::WViewWidget {
public:
    GameStatus(const GamePtr& game, Wt::WContainerWidget* parent = 0):
        Wt::WViewWidget(parent), game_(game) {
    }

protected:
    virtual Wt::WWidget* renderView() {
        Wt::WContainerWidget* result = new Wt::WContainerWidget();
        try {
            add_items(result);
        } catch (std::exception& e) {
            tApp->log("warning") << e.what();
        }
        return result;
    }

    void add_items(Wt::WContainerWidget* result) {
        dbo::Transaction t(tApp->session());
        result->setList(true);
        kw(game_->str_state(), "tc.game.State", result, /* bold */ true);
        if (!User::has_s(SWITCH_LESS_GAME_INFO)) {
            user(game_->white(), "tc.game.white", result);
            user(game_->black(), "tc.game.black", result);
            user(game_->order_user(), "tc.game.order_user", result);
            user(game_->init(), "tc.game.init", result);
            kw(tr(game_->colors_random() ? "tc.game.colors_random" :
                  "tc.game.colors_not_random"), "tc.game.colors", result);
        }
        user(game_->winner(), "tc.common.winner", result, /* bold */ true);
        if (!User::has_s(SWITCH_LESS_GAME_INFO)) {
            time(game_->created(), "tc.common.created", result);
            time(game_->confirmed(), "tc.game.confirmed", result);
            time(game_->started(), "tc.game.started", result);
            time(game_->lastmove(), "tc.game.lastmove", result);
            time(game_->pause_until(), "tc.game.pause_until", result);
            time(game_->ended(), "tc.game.ended", result);
            const GPPtr& gp = game_->gp();
            if (game_->is_ended()) {
                bool_item(game_->real_rating(), "tc.game.real_rating_true",
                          "tc.game.real_rating_false", result);
            } else {
                bool_item((gp->norating() || game_->norating()),
                          "tc.game.norating_true",
                          "tc.game.norating_false", result);
            }
            if (!game_->is_ended()) {
                if (gp->first_draw() != NO_DRAW) {
                    int first_draw =  gp->first_draw() / 2;
                    item(Wt::WString::trn("tc.game.First_draw", first_draw)
                         .arg(gp->first_draw() / 2), result);
                } else {
                    item(Wt::WString::tr("tc.game.No_draw"), result);
                }
            }
            {
                Wt::WContainerWidget* li = new Wt::WContainerWidget(result);
                Wt::WAnchor* a = new Wt::WAnchor(li);
                a->setText(tr("tc.game.Parameters") + " " + TO_S(gp.id()));
                a->setLink(tApp->path().gp_view()->get_link(gp.id()));
            }
        }
        const CompetitionPtr& c = game_->competition();
        if (c) {
            Wt::WContainerWidget* li = new Wt::WContainerWidget(result);
            Wt::WAnchor* a = new Wt::WAnchor(li);
            Wt::WString text = tr("tc.competition.competition") + " ";
            text += c->name().empty() ? TO_S(c.id()) : c->name();
            a->setText(text);
            a->setLink(tApp->path().competition_view()->get_link(c.id()));
            int stage = game_->competition_stage() + 1;
            if (stage != 0) {
                new Wt::WText(tr("tc.competition.stage_is").arg(stage), li);
            }
        }
        if (!User::has_s(SWITCH_LESS_GAME_INFO)) {
            if (game_->state() == Game::CONFIRMED && c) {
                time(game_->confirmed() + c->cp()->force_start_delay(),
                     "tc.game.force_start", result);
            }
            if (game_->state() == Game::PROPOSED && c &&
                    game_->created().isValid()) {
                time(game_->created() + c->cp()->relax_time(),
                     "tc.game.force_confirm", result);
            }
        }
    }

private:
    GamePtr game_;

    Wt::WContainerWidget* item(const Wt::WString& text, Wt::WContainerWidget* r,
                               bool bold = false) {
        Wt::WContainerWidget* li = new Wt::WContainerWidget(r);
        Wt::WText* t = new Wt::WText(text, li);
        if (bold) {
            t->decorationStyle().font().setWeight(Wt::WFont::Bolder);
        }
        return li;
    }

    Wt::WContainerWidget* kw(const Wt::WString& s, const char* tr_id,
                             Wt::WContainerWidget* r, bool bold = false) {
        return item(tr("tc.common.kw").arg(tr(tr_id)).arg(s), r, bold);
    }

    void time(const Wt::WDateTime& dt, const char* tr_id,
              Wt::WContainerWidget* r) {
        if (dt.isValid()) {
            kw("<font class='thechess-datetime'>" + time2str(dt) + "</font>",
               tr_id, r);
        }
    }

    void user(const UserPtr& user, const char* tr_id,
              Wt::WContainerWidget* r, bool bold = false) {
        if (user) {
            Wt::WContainerWidget* li;
            li = kw(user->safe_username(), tr_id, r, bold);
            CompetitionPtr c = game_->competition();
            if (c && c->type() == TEAM) {
                TCMPtr tcm = tApp->session().load<TCM>(TCMId(c, user));
                TeamPtr team = tcm->team();
                li->addWidget(new Wt::WText(" ("));
                li->addWidget(team_anchor(team.id()));
                li->addWidget(new Wt::WText(")"));
            }
        }
    }

    void bool_item(const bool value, const char* true_id, const char* false_id,
                   Wt::WContainerWidget* r) {
        item(tr(value ? true_id : false_id), r);
    }
};

class GameWidget::GameWidgetImpl :
    public Wt::WContainerWidget, public Notifiable {
public:
    GameWidgetImpl(const GamePtr& game) :
        Wt::WContainerWidget(),
        Notifiable(Object(GAME, game.id())),
        game_status_(0),
        game_(game),
        comment_container_(0),
        analysis_(0),
        comments_shown_(false) {
        dbo::Transaction t(tApp->session());
        game_.reread();
        new Header(tr("tc.game.Header").arg((int)game.id()), this);
        int max_moves = -1;
        bool active = game_->can_move(tApp->user());
        bool big = false; // FIXME
        bool append_only = true;
        Piece::Color bottom = game_->color_of(tApp->user());
        const Moves& moves = game_->moves();
        moves_widget_ = new MovesWidget(moves, big, active, max_moves,
                                        append_only, bottom, this);
        moves_widget_->set_links_handler(boost::bind(&GameWidgetImpl::links,
                                         this, _1));
        moves_widget_->half_move().connect(this, &GameWidgetImpl::move_handler);
        moves_widget_->set_move_confirmation(true);
        if (User::has_s(SWITCH_LESS_GAME_INFO)) {
            moves_widget_->show_taken(false);
            moves_widget_->show_moves(false);
        }
        countdown_ = new GameCountdown(game_, this);
        manager_ = new Wt::WContainerWidget(this);
        if (game_->can_comment(tApp->user()) ||
                !game_->comment().empty()) {
            comment_container_ = new Wt::WContainerWidget(this);
        }
        print_comment();
        game_status_ = new GameStatus(game_, this);
        if (!User::has_s(SWITCH_LESS_GAME_INFO)) {
            if (game->is_ended()) {
                new Wt::WAnchor(str(boost::format("/pgn/?game=%i") % game.id()),
                                tr("tc.game.Download_pgn"), this);
            }
        }
        print_comment_list();
        status_and_manager();
        countdown_print();
    }

    ~GameWidgetImpl() {
        if (analysis_) {
            analysis_->reject();
        }
    }

    virtual void notify(EventPtr) {
        dbo::Transaction t(tApp->session());
        game_.reread();
        moves_widget_->set_active(game_->can_move(tApp->user()));
        countdown_print();
        if (game_->colors_random() && game_->size_without_init() == 0) {
            moves_widget_->bottom_set(game_->color_of(tApp->user()));
        }
        if (game_->size() < moves_widget_->moves().size()) {
            moves_widget_->set_moves(game_->moves());
        }
        if (game_->size() - moves_widget_->moves().size() == 1) {
            int index = game_->size() - 1;
            const Board& board = moves_widget_->board();
            HalfMove last_move = game_->moves().half_move(index, board);
            moves_widget_->add_move(last_move);
            if (game_->size_without_init() == 1) {
                print_manager(); // to show Rollback button
            }
        }
        status_and_manager();
        print_comment();
        if (!comments_shown_ && game_->has_comment_base()) {
            tApp->path().open(wApp->internalPath());
        }
    }

    void set_half_move(int half_move_index) {
        moves_widget_->set_half_move(half_move_index);
    }

private:
    GamePtr game_;
    MovesWidget* moves_widget_;
    GameStatus* game_status_;
    Wt::WContainerWidget* manager_;
    GameCountdown* countdown_;
    Wt::WContainerWidget* comment_container_;
    Wt::WDialog* analysis_;
    bool comments_shown_;

    void move_handler(const HalfMove& half_move) {
        dbo::Transaction t(tApp->session());
        game_.reread();
        int game_size = game_->moves().size();
        int moves_size = moves_widget_->moves().size();
        // additional protection
        if (game_size == moves_size - 1 && game_->can_move(tApp->user())) {
            game_.modify()->add_move(half_move, moves_widget_->board());
        }
        bool game_ended = game_->is_ended();
        t.commit();
        t_emit(GAME, game_.id());
        if (game_ended) {
            t_task(GAME, game_.id());
        }
    }

    void status_and_manager() {
        dbo::Transaction t(tApp->session());
        print_status();
        print_manager();
    }

    void countdown_print() {
        countdown_->reread();
    }

    void print_manager() {
        dbo::Transaction t(tApp->session());
        manager_->clear();
        print_refresh_button();
        print_analysis_button();
        if (tApp->user()) {
            if (game_->state() < Game::ACTIVE) {
                print_before_active_buttons();
            }
            if (game_->state() == Game::ACTIVE ||
                    game_->state() == Game::PAUSE) {
                print_pause_buttons();
                print_mistake_buttons();
                print_draw_buttons();
            }
            print_ended_buttons();
        }
    }

    void print_refresh_button() {
        if (!wApp->environment().ajax()) {
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.common.Refresh"), manager_);
            b->clicked().connect(this, &GameWidgetImpl::status_and_manager);
        }
    }

    void print_analysis_button() {
        if (game_->state() >= Game::ACTIVE && game_->size()) {
            Wt::WPushButton* b =
                new Wt::WPushButton(tr("tc.game.Analysis"), manager_);
            b->clicked().connect(this, &GameWidgetImpl::show_analysis);
        }
    }

    void print_before_active_buttons() {
        if (game_->competition()) {
            UserPtr competitor = game_->other_user(tApp->user());
            if (game_->state() == Game::PROPOSED &&
                    game_->has_competition_confirmed(competitor)) {
                Wt::WText* p;
                p = new Wt::WText(tr("tc.game.Competition_other_proposed")
                                  .arg(competitor->safe_username()));
                manager_->addWidget(p);
                p->setStyleClass("thechess-proposal");
                new Wt::WBreak(manager_);
            }
            if (game_->can_competition_confirm(tApp->user())) {
                Wt::WText* t = new Wt::WText(manager_);
                t->setText(tr("tc.game.Competition_confirm_welcome"));
                but<&Game::competition_confirm>("tc.game.Competition_confirm",
                                                true);
            } else if (game_->can_competition_discard(tApp->user())) {
                Wt::WText* t = new Wt::WText(manager_);
                t->setText(tr("tc.game.Competition_discard_welcome"));
                but<&Game::competition_discard>("tc.game.Competition_discard");
            }
        } else {
            if (game_->can_join(tApp->user())) {
                but<&Game::join>("tc.common.Join", true);
            } else if (game_->can_confirm(tApp->user())) {
                but<&Game::confirm>("tc.common.Confirm", true);
            }
            if (game_->can_cancel(tApp->user())) {
                but<&Game::cancel>("tc.common.Cancel");
            }
        }
    }

    void print_pause_buttons() {
        if (game_->admin_can_pause_discard(tApp->user())) {
            new Wt::WBreak(manager_);
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.game.Stop_pause"), manager_);
            b->clicked().connect(this, &GameWidgetImpl::discard_pause);
        } else if (game_->admin_can_pause(tApp->user())) {
            new Wt::WBreak(manager_);
            Td max = config::max::PAUSE_LIMIT_INIT;
            Td d = game_->pause_limit();
            Wt::Wc::TimeDurationWidget* pause_duration =
                new Wt::Wc::TimeDurationWidget(TD_NULL, d, max, manager_);
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.game.Pause_propose"), manager_);
            b->clicked().connect(boost::bind(&GameWidgetImpl::pause,
                                             this, pause_duration));
        } else if (game_->can_pause_propose(tApp->user())) {
            new Wt::WBreak(manager_);
            Td max = game_->pause_limit();
            Td d = config::defaults::PAUSE_FACTOR * max;
            Wt::Wc::TimeDurationWidget* pause_duration =
                new Wt::Wc::TimeDurationWidget(TD_NULL, d, max, manager_);
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.game.Pause_propose"),
                                    manager_);
            b->clicked().connect(boost::bind(
                                     &GameWidgetImpl::pause_propose,
                                     this, pause_duration));
        } else if (game_->is_pause_proposed()) {
            new Wt::WBreak(manager_);
            Wt::WText* p;
            p = new Wt::WText(tr("tc.game.Pause_proposal")
                              .arg(game_->pause_proposer()->safe_username())
                              .arg(td2str(game_->pause_proposed_td())),
                              manager_);
            p->setStyleClass("thechess-proposal");
            if (game_->can_pause_agree(tApp->user())) {
                but<&Game::pause_agree>("tc.common.Agree");
            }
            if (game_->can_pause_discard(tApp->user())) {
                but<&Game::pause_discard>("tc.common.Discard");
            }
        }
    }

    void print_mistake_buttons() {
        if (game_->is_mistake_proposed()) {
            new Wt::WBreak(manager_);
            bool w = Moves::order(game_->mistake_move()) == Piece::WHITE;
            Wt::WText* p;
            const char* color = w ? "tc.game.of_white" : "tc.game.of_black";
            p = new Wt::WText(tr("tc.game.Mistake_proposal")
                              .arg(game_->mistake_proposer()->safe_username())
                              .arg(Moves::move_number(game_->mistake_move()))
                              .arg(tr(color)),
                              manager_);
            p->setStyleClass("thechess-proposal");
            if (game_->can_mistake_agree(tApp->user())) {
                but<&Game::mistake_agree>("tc.common.Agree");
            }
            if (game_->can_mistake_discard(tApp->user())) {
                but<&Game::mistake_discard>("tc.common.Discard");
            }
        } else if (game_->can_mistake_propose(tApp->user())) {
            new Wt::WBreak(manager_);
            new Wt::WText(tr("tc.game.Mistake_welcome"), manager_);
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.game.Mistake_propose"),
                                    manager_);
            b->clicked().connect(this, &GameWidgetImpl::mistake_propose);
        }
    }

    void print_draw_buttons() {
        new Wt::WBreak(manager_);
        if (game_->is_draw_proposed()) {
            Wt::WText* p;
            p = new Wt::WText(tr("tc.game.Draw_proposal")
                              .arg(game_->draw_proposer()->safe_username()),
                              manager_);
            p->setStyleClass("thechess-proposal");
            if (game_->can_draw_agree(tApp->user())) {
                but<&Game::draw_agree>("tc.common.Agree");
            }
            if (game_->can_draw_discard(tApp->user())) {
                but<&Game::draw_discard>("tc.common.Discard");
            }
        } else if (game_->can_draw_propose(tApp->user())) {
            but<&Game::draw_propose>("tc.game.Draw_propose");
        }
        if (game_->can_check_auto_draws(tApp->user())) {
            but<&Game::check_auto_draws>("tc.game.Call_auto_judge");
        }
        if (game_->can_surrender(tApp->user())) {
            Wt::WPushButton* s2 = but<&Game::surrender>("tc.common.Confirm");
            s2->addStyleClass("thechess-dangerous");
            s2->hide();
            Wt::WPushButton* s1 = new Wt::WPushButton(tr("tc.game.Surrender"));
            manager_->addWidget(s1);
            s1->clicked().connect(s2, &Wt::WPushButton::show);
            s1->clicked().connect(s1, &Wt::WPushButton::hide);
        }
    }

    void print_ended_buttons() {
        if (game_->can_mark_norating(tApp->user())) {
            new Wt::WBreak(manager_);
            but<&Game::mark_norating>("tc.game.Mark_norating");
        }
    }

    void print_status() {
        game_status_->update();
    }

    typedef void (Game::*GameMember)(const UserPtr&);
    template <GameMember method>
    void action() {
        dbo::Transaction t(tApp->session());
        game_.reread();
        UserPtr(game_->white()).reread();
        UserPtr(game_->black()).reread();
        Game::State state_before = game_->state();
        (game_.modify()->*method)(tApp->user());
        Game::State state_after = game_->state();
        t.commit();
        t_emit(GAME, game_.id());
        if (state_after != state_before) {
            t_task(GAME, game_.id());
        }
        if (method == &Game::join) {
            t_emit(USER, tApp->user().id());
        }
    }

    template <GameMember method>
    Wt::WPushButton* but(const char* title_id, bool main_button = false) {
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr(title_id), manager_);
        b->clicked().connect(this, &GameWidgetImpl::action<method>);
        if (main_button) {
            b->addStyleClass("thechess-main-button");
        }
        return b;
    }

    void pause_propose(Wt::Wc::TimeDurationWidget* pause_duration) {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()
        ->pause_propose(tApp->user(), pause_duration->corrected_value());
        t.commit();
        t_emit(GAME, game_.id());
    }

    void pause(Wt::Wc::TimeDurationWidget* pause_duration) {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()->admin_pause(tApp->user(),
                                    pause_duration->corrected_value());
        admin_log("Pause " + game_a(game_.id()));
        t.commit();
        t_task(GAME, game_.id());
        t_emit(GAME, game_.id());
    }

    void discard_pause() {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()->admin_pause_discard(tApp->user());
        admin_log("Discard pause of " + game_a(game_.id()));
        t.commit();
        t_task(GAME, game_.id());
        t_emit(GAME, game_.id());
    }

    void mistake_propose() {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()
        ->mistake_propose(tApp->user(), moves_widget_->current_move());
        t.commit();
        t_emit(GAME, game_.id());
    }

    void show_analysis() {
        dbo::Transaction t(tApp->session());
        if (analysis_) {
            return;
        }
        analysis_ = new Wt::WDialog(
            tr("tc.game.Analysis_title").arg((int)game_.id()));
        int max_moves = -1;
        bool active = true;
        bool big = false; // FIXME
        bool append_only = false;
        Piece::Color bottom = game_->color_of(tApp->user());
        const Moves& moves = game_->moves();
        MovesWidget* analysis_moves = new MovesWidget(moves,
                big, active, max_moves,
                append_only, bottom, analysis_->contents());
        if (!game_->is_ended()) {
            analysis_moves->set_links_handler(
                boost::bind(&GameWidgetImpl::links, this, _1));
        }
        analysis_->setClosable(true);
        analysis_->finished()
        .connect(this, &GameWidgetImpl::close_analysis);
        analysis_->setModal(false);
        analysis_->show();
        if (tApp->environment().agentIsChrome()) {
            // FIXME
            Wt::Wc::schedule_action(SECOND, Wt::Wc::bound_post(boost::bind(
                                        &GameWidgetImpl::fix_chrome, this)));
        }
    }

    // FIXME
    void fix_chrome() {
        if (!analysis_) {
            return;
        }
        MovesWidget* moves_widget;
        moves_widget = DOWNCAST<MovesWidget*>(analysis_->contents()->widget(0));
        Moves moves = moves_widget->moves();
        moves_widget->set_moves(moves);
        Wt::Wc::updates_trigger();
    }

    void close_analysis() {
        delete analysis_;
        analysis_ = 0;
    }

    void comment_handler(const Wt::WString& text) {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()->set_comment(tApp->user(), text);
        admin_log("Edit comment of " + game_a(game_.id()));
        t.commit();
        t_emit(GAME, game_.id());
    }

    void print_comment() {
        if (!comment_container_) {
            return;
        }
        dbo::Transaction t(tApp->session());
        comment_container_->clear();
        new Wt::WText(tr("tc.game.comment"), comment_container_);
        new Wt::WText(": ", comment_container_);
        if (game_->can_comment(tApp->user())) {
            Wt::WInPlaceEdit* comment =
                new Wt::WInPlaceEdit(game_->comment(), comment_container_);
            comment->setEmptyText(tr("tc.game.comment_welcome"));
            comment->valueChanged().connect(this,
                                            &GameWidgetImpl::comment_handler);
        } else {
            new Wt::WText(game_->comment(), comment_container_);
        }
    }

    void print_comment_list() {
        dbo::Transaction t(tApp->session());
        if (game_->has_comment_base()) {
            print_comment_list_impl();
        } else {
            new Wt::WBreak(this);
            Wt::WPushButton* add = new Wt::WPushButton(this);
            add->setText(tr("tc.comment.Add"));
            add->clicked().connect(this,
                                   &GameWidgetImpl::print_comment_list_impl);
            add->clicked().connect(add, &Wt::WWidget::hide);
            add->clicked().connect(add, &Wt::WWidget::disable);
        }
    }

    void print_comment_list_impl() {
        dbo::Transaction t(tApp->session());
        CommentPtr comment_base = game_->comment_base();
        if (!comment_base) {
            game_.reread();
            comment_base = game_.modify()->comment_base();
            t.commit();
            t_emit(GAME, game_.id());
        } else {
            comments_shown_ = true;
            addWidget(new CommentList(Comment::CHAT_MESSAGE, comment_base));
        }
    }

    void links(LinksDialog* dialog) {
        dbo::Transaction t(tApp->session());
        if (game_->is_ended()) {
            moves_widget_->links(dialog);
        }
        int move_n = Moves::move_number(moves_widget_->current_move());
        dialog->add_game_move(game_.id(), move_n);
        dialog->add_game(game_.id());
    }
};

GameWidget::GameWidget(const GamePtr& game, Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new GameWidgetImpl(game);
    setImplementation(impl_);
}

void GameWidget::set_half_move(int half_move_index) {
    impl_->set_half_move(half_move_index);
}

}

