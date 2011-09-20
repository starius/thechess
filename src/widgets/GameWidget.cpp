/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <iostream>
#include <boost/lexical_cast.hpp>

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
namespace dbo = Wt::Dbo;

#include "widgets/GameCountdown.hpp"
#include "widgets/GameWidget.hpp"
#include "widgets/TimeDeltaWidget.hpp"
#include "widgets/MovesWidget.hpp"
#include "widgets/PleaseLoginWidget.hpp"
#include "model/all.hpp"
#include "ThechessApplication.hpp"
#include "config.hpp"
#include "chess/Moves.hpp"
#include "chess/Piece.hpp"

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
        kw_(game_->str_state(), "tc.game.State", result);
        user_(game_->white(), "tc.game.white", result);
        user_(game_->black(), "tc.game.black", result);
        user_(game_->order_user(), "tc.game.order_user", result);
        user_(game_->init(), "tc.game.init", result);
        kw_(tr(game_->colors_random() ? "tc.game.colors_random" :
               "tc.game.colors_not_random"), "tc.game.colors", result);
        user_(game_->winner(), "tc.common.winner", result);
        time_(game_->created(), "tc.game.created", result);
        time_(game_->confirmed(), "tc.game.confirmed", result);
        time_(game_->started(), "tc.game.started", result);
        time_(game_->lastmove(), "tc.game.lastmove", result);
        time_(game_->pause_until(), "tc.game.pause_until", result);
        time_(game_->ended(), "tc.game.ended", result);
        if (game_->is_ended()) {
            bool_(game_->real_rating(), "tc.game.real_rating_true",
                  "tc.game.real_rating_false", result);
        } else {
            bool_(game_->gp()->norating(), "tc.game.norating_true",
                  "tc.game.norating_false", result);
        }
        if (!game_->is_ended()) {
            item_(Wt::WString::trn("tc.game.First_draw",
                                   game_->gp()->first_draw() / 2).arg(game_->gp()->first_draw() / 2), result);
        }
        CompetitionPtr c = game_->competition();
        if (c) {
            Wt::WContainerWidget* li = new Wt::WContainerWidget(result);
            Wt::WAnchor* a = new Wt::WAnchor(li);
            Wt::WString text = tr("tc.competition.competition") + " ";
            text += c->name().empty() ? boost::lexical_cast<std::string>(c.id()) : c->name();
            a->setText(text);
            a->setRefInternalPath(str(boost::format("/competition/%i/") % c.id()));
        }
        if (game_->state() == Game::CONFIRMED && c) {
            time_(game_->confirmed() + c->cp()->force_start_delay(),
                  "tc.game.force_start", result);
        }
        if (game_->state() == Game::PROPOSED && c &&
                c->type() == STAGED) {
            time_(game_->created() + c->cp()->relax_time(),
                  "tc.game.force_confirm", result);
        }
        t.commit();
    }

private:
    GamePtr game_;

    void item_(const Wt::WString& text, Wt::WContainerWidget* r) {
        Wt::WContainerWidget* li = new Wt::WContainerWidget(r);
        new Wt::WText(text, li);
    }

    void kw_(const Wt::WString& s, const char* tr_id,
             Wt::WContainerWidget* r) {
        item_(tr("tc.common.kw").arg(tr(tr_id)).arg(s), r);
    }

    void time_(const Wt::WDateTime& dt, const char* tr_id,
               Wt::WContainerWidget* r) {
        if (dt.isValid()) {
            kw_(dt.toString(), tr_id, r);
        }
    }

    void user_(UserPtr user, const char* tr_id, Wt::WContainerWidget* r) {
        if (user) {
            kw_(user->username(), tr_id, r);
        }
    }

    void bool_(const bool value, const char* true_id, const char* false_id,
               Wt::WContainerWidget* r) {
        item_(tr(value ? true_id : false_id), r);
    }
};

class GameWidgetImpl : public Wt::WContainerWidget, public Notifiable {
public:
    GameWidgetImpl(GamePtr game) :
        Wt::WContainerWidget(),
        Notifiable(Object(GAME, game.id())),
        game_(game) {
        dbo::Transaction t(tApp->session());
        new Wt::WText(tr("tc.game.Header")
                      .arg((int)game.id()), this);
        int max_moves = -1;
        bool active = game_->can_move(tApp->user());
        bool big = false; // FIXME
        bool append_only = true;
        Piece::Color bottom = game_->color_of(tApp->user());
        const Moves& moves = game_->moves();
        moves_widget_ = new MovesWidget(moves, big, active, max_moves,
                                        append_only, bottom, this);
        moves_widget_->half_move()
        .connect(this, &GameWidgetImpl::move_handler_);
        countdown_container_ = new Wt::WContainerWidget(this);
        manager_ = new Wt::WContainerWidget(this);
        comment_container_ = new Wt::WContainerWidget(this);
        print_comment_();
        game_status_ = new GameStatus(game_, this);
        new Wt::WAnchor(str(boost::format("/pgn/?game=%i") % game.id()),
                        tr("tc.game.Download_pgn"), this);
        status_and_manager_();
        countdown_print_();
        t.commit();
    }

    virtual void notify() {
        moves_widget_->set_active(game_->can_move(tApp->user()));
        countdown_print_();
        if (game_->colors_random() && game_->size_without_init() == 0) {
            moves_widget_->bottom_set(game_->color_of(tApp->user()));
        }
        if (game_->size() < moves_widget_->moves().size()) {
            moves_widget_->set_moves(game_->moves());
            status_and_manager_();
        }
        if (game_->size() - moves_widget_->moves().size() == 1) {
            HalfMove last_move =
                game_->moves().half_move(game_->size() - 1, moves_widget_->board());
            moves_widget_->add_move(last_move);
            if (game_->size_without_init() == 1) {
                print_manager_(); // to show Rollback button
            }
        }
        status_and_manager_();
        print_comment_();
    }

private:
    GamePtr game_;
    MovesWidget* moves_widget_;
    GameStatus* game_status_;
    Wt::WContainerWidget* manager_;
    Wt::WContainerWidget* countdown_container_;
    Wt::WContainerWidget* comment_container_;

    void move_handler_(const HalfMove& half_move) {
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
        Object object(GAME, game_.id());
        ThechessNotifier::app_emit(object);
        if (game_ended) {
            tApp->server().tracker().add_or_update_task(object);
        }
    }

    void status_and_manager_() {
        dbo::Transaction t(tApp->session());
        print_status_();
        print_manager_();
        t.commit();
    }

    void countdown_print_() {
        countdown_container_->clear();
        new GameCountdown(game_, countdown_container_);
    }

    void print_manager_() {
        dbo::Transaction t(tApp->session());
        manager_->clear();
        print_analysis_button_();
        if (tApp->user()) {
            if (game_->state() < Game::ACTIVE) {
                print_before_active_buttons_();
            }
            if (game_->state() == Game::ACTIVE || game_->state() == Game::PAUSE) {
                print_pause_buttons_();
                print_mistake_buttons_();
                print_draw_buttons_();
            }
        } else {
            new PleaseLoginWidget(manager_);
        }
        t.commit();
    }

    void print_analysis_button_() {
        if (game_->state() >= Game::ACTIVE && game_->size()) {
            Wt::WPushButton* b =
                new Wt::WPushButton(tr("tc.game.Analysis"), manager_);
            b->clicked().connect(this, &GameWidgetImpl::show_analysis_);
        }
    }

    void print_before_active_buttons_() {
        if (game_->competition()) {
            if (game_->has_competition_confirmed(game_->other_user(tApp->user()))) {
                new Wt::WText(tr("tc.game.Competition_other_proposed")
                              .arg(game_->other_user(tApp->user())->username()), manager_);
                new Wt::WBreak(manager_);
            }
            if (game_->can_competition_confirm(tApp->user())) {
                new Wt::WText(tr("tc.game.Competition_confirm_welcome"), manager_);
                button_<&Game::competition_confirm>("tc.game.Competition_confirm");
            } else if (game_->can_competition_discard(tApp->user())) {
                new Wt::WText(tr("tc.game.Competition_discard_welcome"), manager_);
                button_<&Game::competition_discard>("tc.game.Competition_discard");
            }
        } else {
            if (game_->can_join(tApp->user())) {
                button_<&Game::join>("tc.common.Join");
            } else if (game_->can_confirm(tApp->user())) {
                button_<&Game::confirm>("tc.common.Confirm");
            }
            if (game_->can_cancel(tApp->user())) {
                button_<&Game::cancel>("tc.common.Cancel");
            }
        }
    }

    void print_pause_buttons_() {
        if (game_->is_pause_proposed()) {
            new Wt::WBreak(manager_);
            new Wt::WText(tr("tc.game.Pause_proposal")
                          .arg(game_->pause_proposer()->username())
                          .arg(td2str(game_->pause_proposed_td())),
                          manager_);
            if (game_->can_pause_agree(tApp->user())) {
                button_<&Game::pause_agree>("tc.common.Agree");
            }
            if (game_->can_pause_discard(tApp->user())) {
                button_<&Game::pause_discard>("tc.common.Discard");
            }
        } else if (game_->can_pause_propose(tApp->user())) {
            new Wt::WBreak(manager_);
            Td max = game_->pause_limit();
            Td d = config::defaults::PAUSE_FACTOR * max;
            TimeDeltaWidget* pause_duration =
                new TimeDeltaWidget(TD_NULL, d, max, manager_);
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.game.Pause_propose"),
                                    manager_);
            b->clicked().connect(boost::bind(
                                     &GameWidgetImpl::pause_propose_, this, pause_duration));
        }
    }

    void print_mistake_buttons_() {
        if (game_->is_mistake_proposed()) {
            new Wt::WBreak(manager_);
            new Wt::WText(tr("tc.game.Mistake_proposal")
                          .arg(game_->mistake_proposer()->username())
                          .arg(Moves::n_to_human(game_->mistake_move()))
                          .arg(tr(Moves::order(game_->mistake_move()) == Piece::WHITE ?
                                  "tc.game.of_white" : "tc.game.of_black")),
                          manager_);
            if (game_->can_mistake_agree(tApp->user())) {
                button_<&Game::mistake_agree>("tc.common.Agree");
            }
            if (game_->can_mistake_discard(tApp->user())) {
                button_<&Game::mistake_discard>("tc.common.Discard");
            }
        } else if (game_->can_mistake_propose(tApp->user())) {
            new Wt::WBreak(manager_);
            new Wt::WText(tr("tc.game.Mistake_welcome"), manager_);
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.game.Mistake_propose"),
                                    manager_);
            b->clicked().connect(this, &GameWidgetImpl::mistake_propose_);
        }
    }

    void print_draw_buttons_() {
        if (game_->is_draw_proposed()) {
            new Wt::WBreak(manager_);
            new Wt::WText(tr("tc.game.Draw_proposal")
                          .arg(game_->draw_proposer()->username()),
                          manager_);
            if (game_->can_draw_agree(tApp->user())) {
                button_<&Game::draw_agree>("tc.common.Agree");
            }
            if (game_->can_draw_discard(tApp->user())) {
                button_<&Game::draw_discard>("tc.common.Discard");
            }
        } else if (game_->can_draw_propose(tApp->user())) {
            new Wt::WBreak(manager_);
            button_<&Game::draw_propose>("tc.game.Draw_propose");
        }
    }

    void print_status_() {
        game_status_->update();
    }

    typedef void (Game::*GameMember)(UserPtr);
    template <GameMember method>
    void action_() {
        dbo::Transaction t(tApp->session());
        game_.reread();
        Game::State state_before = game_->state();
        (game_.modify()->*method)(tApp->user());
        Game::State state_after = game_->state();
        t.commit();
        Object object(GAME, game_.id());
        ThechessNotifier::app_emit(object);
        if (state_after != state_before) {
            tApp->server().tracker().add_or_update_task(object);
        }
        if (method == &Game::join) {
            ThechessNotifier::app_emit(Object(USER, tApp->user().id()));
        }
    }

    template <GameMember method>
    void button_(const char* title_id) {
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr(title_id), manager_);
        b->clicked().connect(this, &GameWidgetImpl::action_<method>);
    }

    void pause_propose_(TimeDeltaWidget* pause_duration) {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()
        ->pause_propose(tApp->user(), pause_duration->value());
        t.commit();
        Object object(GAME, game_.id());
        ThechessNotifier::app_emit(object);
    }

    void mistake_propose_() {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()
        ->mistake_propose(tApp->user(), moves_widget_->current_move());
        t.commit();
        Object object(GAME, game_.id());
        ThechessNotifier::app_emit(object);
    }

    void show_analysis_() {
        dbo::Transaction t(tApp->session());
        Wt::WDialog* analysis = new Wt::WDialog(
            tr("tc.game.Analysis_title").arg((int)game_.id()));
        int max_moves = -1;
        bool active = true;
        bool big = false; // FIXME
        bool append_only = false;
        Piece::Color bottom = game_->color_of(tApp->user());
        const Moves& moves = game_->moves();
        new MovesWidget(moves, big, active, max_moves,
                        append_only, bottom, analysis->contents());
        Wt::WPushButton* close = new Wt::WPushButton(tr("tc.common.X_close"));
        analysis->titleBar()->insertWidget(0, close);
        close->clicked().connect(analysis, &Wt::WDialog::accept);
        analysis->finished()
        .connect(this, &GameWidgetImpl::close_analysis_);
        analysis->setModal(false);
        analysis->show();
        t.commit();
    }

    void close_analysis_() {
        delete sender();
    }

    void comment_handler_(const Wt::WString& text) {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()->set_comment(tApp->user(), text);
        t.commit();
        Object object(GAME, game_.id());
        ThechessNotifier::app_emit(object);
    }

    void print_comment_() {
        dbo::Transaction t(tApp->session());
        comment_container_->clear();
        new Wt::WText(tr("tc.game.comment"), comment_container_);
        new Wt::WText(": ", comment_container_);
        if (game_->can_comment(tApp->user())) {
            Wt::WInPlaceEdit* comment =
                new Wt::WInPlaceEdit(game_->comment(), comment_container_);
            comment->setEmptyText(tr("tc.game.comment_welcome"));
            comment->valueChanged().connect(this,
                                            &GameWidgetImpl::comment_handler_);
        } else {
            new Wt::WText(game_->comment(), comment_container_);
        }
        t.commit();
    }

};

GameWidget::GameWidget(GamePtr game, Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new GameWidgetImpl(game);
    setImplementation(impl_);
}

}

