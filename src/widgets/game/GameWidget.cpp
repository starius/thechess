/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
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
#include <Wt/Wc/TimeDurationWidget.hpp>
#include <Wt/Wc/util.hpp>

#include "widgets/Header.hpp"
#include "widgets/game/GameCountdown.hpp"
#include "widgets/game/GameWidget.hpp"
#include "widgets/chess/MovesWidget.hpp"
#include "widgets/comment/CommentList.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "config.hpp"
#include "chess/Moves.hpp"
#include "chess/Piece.hpp"
#include "notify.hpp"

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
        time_(game_->created(), "tc.common.created", result);
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
                                   game_->gp()->first_draw() / 2)
                  .arg(game_->gp()->first_draw() / 2), result);
        }
        const CompetitionPtr& c = game_->competition();
        if (c) {
            Wt::WContainerWidget* li = new Wt::WContainerWidget(result);
            Wt::WAnchor* a = new Wt::WAnchor(li);
            Wt::WString text = tr("tc.competition.competition") + " ";
            text += c->name().empty() ? TO_S(c.id()) : c->name();
            a->setText(text);
            a->setLink(tApp->path().competition_view()->get_link(c.id()));
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

    void user_(const UserPtr& user, const char* tr_id,
               Wt::WContainerWidget* r) {
        if (user) {
            kw_(user->safe_username(), tr_id, r);
        }
    }

    void bool_(const bool value, const char* true_id, const char* false_id,
               Wt::WContainerWidget* r) {
        item_(tr(value ? true_id : false_id), r);
    }
};

class GameWidget::GameWidgetImpl :
    public Wt::WContainerWidget, public Notifiable {
public:
    GameWidgetImpl(const GamePtr& game) :
        Wt::WContainerWidget(),
        Notifiable(Object(GAME, game.id()), tNot),
        game_(game) {
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
        moves_widget_->half_move()
        .connect(this, &GameWidgetImpl::move_handler_);
        countdown_ = new GameCountdown(game_, this);
        manager_ = new Wt::WContainerWidget(this);
        comment_container_ = new Wt::WContainerWidget(this);
        print_comment();
        game_status_ = new GameStatus(game_, this);
        new Wt::WAnchor(str(boost::format("/pgn/?game=%i") % game.id()),
                        tr("tc.game.Download_pgn"), this);
        print_comment_list();
        status_and_manager();
        countdown_print();
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
    }

private:
    GamePtr game_;
    MovesWidget* moves_widget_;
    GameStatus* game_status_;
    Wt::WContainerWidget* manager_;
    GameCountdown* countdown_;
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
        tNot->emit(new Object(GAME, game_.id()));
        if (game_ended) {
            tApp->server().planning().add(new Object(GAME, game_.id()), now());
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
            if (game_->has_competition_confirmed(competitor)) {
                new Wt::WText(tr("tc.game.Competition_other_proposed")
                              .arg(competitor->safe_username()), manager_);
                new Wt::WBreak(manager_);
            }
            if (game_->can_competition_confirm(tApp->user())) {
                Wt::WText* t = new Wt::WText(manager_);
                t->setText(tr("tc.game.Competition_confirm_welcome"));
                but<&Game::competition_confirm>("tc.game.Competition_confirm");
            } else if (game_->can_competition_discard(tApp->user())) {
                Wt::WText* t = new Wt::WText(manager_);
                t->setText(tr("tc.game.Competition_discard_welcome"));
                but<&Game::competition_discard>("tc.game.Competition_discard");
            }
        } else {
            if (game_->can_join(tApp->user())) {
                but<&Game::join>("tc.common.Join");
            } else if (game_->can_confirm(tApp->user())) {
                but<&Game::confirm>("tc.common.Confirm");
            }
            if (game_->can_cancel(tApp->user())) {
                but<&Game::cancel>("tc.common.Cancel");
            }
        }
    }

    void print_pause_buttons() {
        if (game_->is_pause_proposed()) {
            new Wt::WBreak(manager_);
            new Wt::WText(tr("tc.game.Pause_proposal")
                          .arg(game_->pause_proposer()->safe_username())
                          .arg(td2str(game_->pause_proposed_td())),
                          manager_);
            if (game_->can_pause_agree(tApp->user())) {
                but<&Game::pause_agree>("tc.common.Agree");
            }
            if (game_->can_pause_discard(tApp->user())) {
                but<&Game::pause_discard>("tc.common.Discard");
            }
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
                                     &GameWidgetImpl::pause_propose_,
                                     this, pause_duration));
        }
    }

    void print_mistake_buttons() {
        if (game_->is_mistake_proposed()) {
            new Wt::WBreak(manager_);
            bool w = Moves::order(game_->mistake_move()) == Piece::WHITE;
            new Wt::WText(tr("tc.game.Mistake_proposal")
                          .arg(game_->mistake_proposer()->safe_username())
                          .arg(Moves::move_number(game_->mistake_move()))
                          .arg(tr(w ? "tc.game.of_white" : "tc.game.of_black")),
                          manager_);
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
        if (game_->is_draw_proposed()) {
            new Wt::WBreak(manager_);
            new Wt::WText(tr("tc.game.Draw_proposal")
                          .arg(game_->draw_proposer()->safe_username()),
                          manager_);
            if (game_->can_draw_agree(tApp->user())) {
                but<&Game::draw_agree>("tc.common.Agree");
            }
            if (game_->can_draw_discard(tApp->user())) {
                but<&Game::draw_discard>("tc.common.Discard");
            }
        } else if (game_->can_draw_propose(tApp->user())) {
            new Wt::WBreak(manager_);
            but<&Game::draw_propose>("tc.game.Draw_propose");
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
        Game::State state_before = game_->state();
        (game_.modify()->*method)(tApp->user());
        Game::State state_after = game_->state();
        t.commit();
        tNot->emit(new Object(GAME, game_.id()));
        if (state_after != state_before) {
            tApp->server().planning().add(new Object(GAME, game_.id()), now());
        }
        if (method == &Game::join) {
            tNot->emit(new Object(USER, tApp->user().id()));
        }
    }

    template <GameMember method>
    void but(const char* title_id) {
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr(title_id), manager_);
        b->clicked().connect(this, &GameWidgetImpl::action<method>);
    }

    void pause_propose_(Wt::Wc::TimeDurationWidget* pause_duration) {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()
        ->pause_propose(tApp->user(), pause_duration->corrected_value());
        t.commit();
        tNot->emit(new Object(GAME, game_.id()));
    }

    void mistake_propose() {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()
        ->mistake_propose(tApp->user(), moves_widget_->current_move());
        t.commit();
        tNot->emit(new Object(GAME, game_.id()));
    }

    void show_analysis() {
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
        analysis->setClosable(true);
        analysis->finished()
        .connect(this, &GameWidgetImpl::close_analysis);
        analysis->setModal(false);
        analysis->show();
    }

    void close_analysis() {
        delete sender();
    }

    void comment_handler_(const Wt::WString& text) {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()->set_comment(tApp->user(), text);
        t.commit();
        tNot->emit(new Object(GAME, game_.id()));
    }

    void print_comment() {
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
        CommentPtr comment_base = game_.modify()->comment_base();
        new CommentList(Comment::CHAT_MESSAGE, comment_base, this);
    }
};

GameWidget::GameWidget(const GamePtr& game, Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new GameWidgetImpl(game);
    setImplementation(impl_);
}

}

