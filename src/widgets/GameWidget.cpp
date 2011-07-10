/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <iostream>

#include <Wt/WObject>
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
#include "model/Game.hpp"
#include "ThechessApplication.hpp"
#include "config.hpp"
#include "chess/moves.hpp"
#include "chess/field.hpp"

namespace thechess {
namespace widgets {
using namespace model;

class GameStatus : public Wt::WViewWidget
{
public:
    GameStatus(const GamePtr& game, Wt::WContainerWidget* parent=0):
    Wt::WViewWidget(parent), game_(game)
    {
    }

protected:
    virtual Wt::WWidget *renderView()
    {
        dbo::Transaction t(tApp->session());
        Wt::WContainerWidget* result = new Wt::WContainerWidget();
        result->setList(true);
        kw_(game_->str_state(), "thechess.state", result);
        user_(game_->white(), "thechess.white", result);
        user_(game_->black(), "thechess.black", result);
        user_(game_->order_user(), "thechess.order_user", result);
        user_(game_->init(), "thechess.init", result);
        kw_(tr(game_->colors_random() ? "thechess.colors_random" :
            "thechess.colors_not_random"), "thechess.colors", result);
        user_(game_->winner(), "thechess.winner", result);
        time_(game_->created(), "thechess.created", result);
        time_(game_->when_confirmed(), "thechess.confirmed", result);
        time_(game_->started(), "thechess.started", result);
        time_(game_->lastmove(), "thechess.lastmove", result);
        time_(game_->pause_until(), "thechess.pause_until", result);
        time_(game_->ended(), "thechess.ended", result);
        if (game_->is_ended())
        {
            bool_(game_->real_rating(), "thechess.real_rating_true",
                "thechess.real_rating_false", result);
        }
        else
        {
            bool_(game_->norating(), "thechess.norating_true",
                "thechess.norating_false", result);
        }
        if (!game_->is_ended())
        {
            item_(Wt::WString::trn("thechess.first_draw_full",
                game_->first_draw()/2).arg(game_->first_draw()/2), result);
        }
        if (game_->state() == Game::confirmed && game_->competition())
        {
            time_(game_->when_confirmed()+game_->competition()->force_start_delay(),
                "thechess.GameWidget.force_start", result);
        }
        if (game_->state() == Game::proposed && game_->competition() &&
            game_->competition()->type() == STAGED)
        {
            time_(game_->created() + game_->competition()->relax_time(),
                "thechess.GameWidget.force_confirm", result);
        }
        t.commit();
        return result;
    }

private:
    const GamePtr& game_;

    void item_(const Wt::WString& text, Wt::WContainerWidget* r)
    {
        Wt::WContainerWidget* li = new Wt::WContainerWidget(r);
        new Wt::WText(text, li);
    }

    void kw_(const Wt::WString& s, const char* tr_id,
        Wt::WContainerWidget* r)
    {
        item_(tr("thechess.format.kw").arg(tr(tr_id)).arg(s), r);
    }

    void time_(const Wt::WDateTime& dt, const char* tr_id,
        Wt::WContainerWidget* r)
    {
        if (dt.isValid())
        {
            kw_(dt.toString(), tr_id, r);
        }
    }

    void user_(UserPtr user, const char* tr_id, Wt::WContainerWidget* r)
    {
        if (user)
        {
            kw_(user->username(), tr_id, r);
        }
    }

    void bool_(const bool value, const char* true_id, const char* false_id,
        Wt::WContainerWidget* r)
    {
        item_(tr(value ? true_id : false_id), r);
    }
};

class GameWidgetImpl : public Wt::WContainerWidget, public Notifiable
{
public:
    GameWidgetImpl(GamePtr game) :
    Wt::WContainerWidget(),
    Notifiable(Object(GameObject, game.id())),
    game_(game)
    {
        dbo::Transaction t(tApp->session());

        new Wt::WText(tr("thechess.format.game_header")
            .arg((int)game.id()), this);

        int max_moves = -1;
        bool active = game_->can_move(tApp->user());
        bool big = false; // FIXME
        bool append_only = true;
        chess::Color bottom = game_->color_of(tApp->user());
        const chess::Moves& moves = game_->moves();
        moves_widget_ = new MovesWidget(moves, big, active, max_moves,
            append_only, bottom, this);
        moves_widget_->move()
            .connect(this, &GameWidgetImpl::move_handler_);

        countdown_container_ = new Wt::WContainerWidget(this);
        manager_ = new Wt::WContainerWidget(this);

        comment_container_ = new Wt::WContainerWidget(this);
        print_comment_();

        game_status_ = new GameStatus(game_, this);
        status_and_manager_();
        countdown_print_();
        t.commit();
    }

    virtual void notify()
    {
        moves_widget_->set_active(game_->can_move(tApp->user()));
        countdown_print_();
        if (game_->colors_random() && game_->size_without_init() == 0)
        {
            moves_widget_->bottom_set(game_->color_of(tApp->user()));
        }
        if (game_->size() < moves_widget_->moves().size())
        {
            moves_widget_->set_moves(game_->moves());
            status_and_manager_();
        }
        if (game_->size() - moves_widget_->moves().size() == 1)
        {
            chess::Move last_move =
                game_->moves().move_at(game_->size()-1, moves_widget_->board());
            moves_widget_->add_move(last_move);
            if (game_->size_without_init() == 1)
            {
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

    void move_handler_(const chess::Move& move)
    {
        dbo::Transaction t(tApp->session());
        game_.reread();
        int game_size = game_->moves().size();
        int moves_size = moves_widget_->moves().size();
        // additional protection
        if (game_size == moves_size-1 && game_->can_move(tApp->user()))
        {
            game_.modify()->add_move(move, moves_widget_->board());
        }
        bool game_ended = game_->is_ended();
        t.commit();
        Object object(GameObject, game_.id());
        ThechessNotifier::app_emit(object);
        if (game_ended)
        {
            tApp->server().tracker().add_or_update_task(object);
        }
    }

    void status_and_manager_()
    {
        dbo::Transaction t(tApp->session());
        print_status_();
        print_manager_();
        t.commit();
    }

    void countdown_print_()
    {
        countdown_container_->clear();
        new GameCountdown(game_, countdown_container_);
    }

    void print_manager_()
    {
        dbo::Transaction t(tApp->session());
        manager_->clear();
        print_analysis_button_();
        if (tApp->user())
        {
            if (game_->state() < Game::active)
            {
                print_before_active_buttons_();
            }
            if (game_->state()==Game::active || game_->state()==Game::pause)
            {
                print_pause_buttons_();
                print_mistake_buttons_();
                print_draw_buttons_();
            }
        }
        else
        {
            new PleaseLoginWidget(manager_);
        }
        t.commit();
    }

    void print_analysis_button_()
    {
        if (game_->state() >= Game::active && game_->size())
        {
            Wt::WPushButton* b =
                new Wt::WPushButton(tr("thechess.analysis"), manager_);
            b->clicked().connect(this, &GameWidgetImpl::show_analysis_);
        }
    }

    void print_before_active_buttons_()
    {
        if (game_->competition())
        {
            if (game_->has_competition_confirmed(game_->other_user(tApp->user())))
            {
                new Wt::WText(tr("thechess.GameWidget.competition_other_proposed")
                    .arg(game_->other_user(tApp->user())->username()), manager_);
                new Wt::WBreak(manager_);
            }
            if (game_->can_competition_confirm(tApp->user()))
            {
                new Wt::WText(tr("thechess.GameWidget.competition_confirm_welcome"), manager_);
                button_<&Game::competition_confirm>("thechess.GameWidget.competition_confirm");
            }
            else if (game_->can_competition_discard(tApp->user()))
            {
                new Wt::WText(tr("thechess.GameWidget.competition_discard_welcome"), manager_);
                button_<&Game::competition_discard>("thechess.GameWidget.competition_discard");
            }
        }
        else
        {
            if (game_->can_join(tApp->user()))
            {
                button_<&Game::join>("thechess.join");
            }
            else if (game_->can_confirm(tApp->user()))
            {
                button_<&Game::confirm>("thechess.confirm");
            }
            if (game_->can_cancel(tApp->user()))
            {
                button_<&Game::cancel>("thechess.cancel");
            }
        }
    }

    void print_pause_buttons_()
    {
        if (game_->is_pause_proposed())
        {
            new Wt::WBreak(manager_);
            new Wt::WText(tr("thechess.format.pause_proposal")
                .arg(game_->pause_proposer()->username())
                .arg(td2str(game_->pause_proposed_td())),
                manager_);
            if (game_->can_pause_agree(tApp->user()))
            {
                button_<&Game::pause_agree>("thechess.agree");
            }
            if (game_->can_pause_discard(tApp->user()))
            {
                button_<&Game::pause_discard>("thechess.discard");
            }
        }
        else if (game_->can_pause_propose(tApp->user()))
        {
            new Wt::WBreak(manager_);
            Td max = game_->pause_limit();
            Td d = config::defaults::pause_factor * max;
            TimeDeltaWidget* pause_duration =
                new TimeDeltaWidget(td_null, d, max, manager_);
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("thechess.pause_propose"),
                manager_);
            b->clicked().connect(boost::bind(
                &GameWidgetImpl::pause_propose_, this, pause_duration));
        }
    }

    void print_mistake_buttons_()
    {
        if (game_->is_mistake_proposed())
        {
            using chess::Moves;
            new Wt::WBreak(manager_);
            new Wt::WText(tr("thechess.format.mistake_proposal")
                .arg(game_->mistake_proposer()->username())
                .arg(Moves::n_to_human(game_->mistake_move()))
                .arg(tr(Moves::order(game_->mistake_move()) == chess::white ?
                    "thechess.of_white" : "thechess.of_black")),
                manager_);
            if (game_->can_mistake_agree(tApp->user()))
            {
                button_<&Game::mistake_agree>("thechess.agree");
            }
            if (game_->can_mistake_discard(tApp->user()))
            {
                button_<&Game::mistake_discard>("thechess.discard");
            }
        }
        else if (game_->can_mistake_propose(tApp->user()))
        {
            new Wt::WBreak(manager_);
            new Wt::WText(tr("thechess.mistake_welcome"), manager_);
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("thechess.mistake_propose"),
                manager_);
            b->clicked().connect(this, &GameWidgetImpl::mistake_propose_);
        }
    }

    void print_draw_buttons_()
    {
        if (game_->is_draw_proposed())
        {
            new Wt::WBreak(manager_);
            new Wt::WText(tr("thechess.format.draw_proposal")
                .arg(game_->draw_proposer()->username()),
                manager_);
            if (game_->can_draw_agree(tApp->user()))
            {
                button_<&Game::draw_agree>("thechess.agree");
            }
            if (game_->can_draw_discard(tApp->user()))
            {
                button_<&Game::draw_discard>("thechess.discard");
            }
        }
        else if (game_->can_draw_propose(tApp->user()))
        {
            new Wt::WBreak(manager_);
            button_<&Game::draw_propose>("thechess.draw_propose");
        }
    }

    void print_status_()
    {
        game_status_->update();
    }

    typedef void (Game::*GameMember)(UserPtr);
    template <GameMember method>
    void action_()
    {
        dbo::Transaction t(tApp->session());
        game_.reread();
        Game::State state_before = game_->state();
        (game_.modify()->*method)(tApp->user());
        Game::State state_after = game_->state();
        t.commit();
        Object object(GameObject, game_.id());
        ThechessNotifier::app_emit(object);
        if (state_after != state_before)
        {
            tApp->server().tracker().add_or_update_task(object);
        }
    }

    template <GameMember method>
    void button_(const char* title_id)
    {
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr(title_id), manager_);
        b->clicked().connect(this, &GameWidgetImpl::action_<method>);
    }

    void pause_propose_(TimeDeltaWidget* pause_duration)
    {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()
            ->pause_propose(tApp->user(), pause_duration->value());
        t.commit();
        Object object(GameObject, game_.id());
        ThechessNotifier::app_emit(object);
    }

    void mistake_propose_()
    {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()
            ->mistake_propose(tApp->user(), moves_widget_->current_move());
        t.commit();
        Object object(GameObject, game_.id());
        ThechessNotifier::app_emit(object);
    }

    void show_analysis_()
    {
        dbo::Transaction t(tApp->session());
        Wt::WDialog* analysis = new Wt::WDialog(
            tr("thechess.format.analysis").arg((int)game_.id()));
        int max_moves = -1;
        bool active = true;
        bool big = false; // FIXME
        bool append_only = false;
        chess::Color bottom = game_->color_of(tApp->user());
        const chess::Moves& moves = game_->moves();
        new MovesWidget(moves, big, active, max_moves,
            append_only, bottom, analysis->contents());
        Wt::WPushButton* close = new Wt::WPushButton(tr("thechess.close"));
        analysis->titleBar()->insertWidget(0, close);
        close->clicked().connect(analysis, &Wt::WDialog::accept);
        analysis->finished()
            .connect(this, &GameWidgetImpl::close_analysis_);
        analysis->setModal(false);
        analysis->show();
        t.commit();
    }

    void close_analysis_()
    {
        delete sender();
    }

    void comment_handler_(const Wt::WString& text)
    {
        dbo::Transaction t(tApp->session());
        game_.reread();
        game_.modify()->set_comment(tApp->user(), text);
        t.commit();
        Object object(GameObject, game_.id());
        ThechessNotifier::app_emit(object);
    }

    void print_comment_()
    {
        dbo::Transaction t(tApp->session());
        comment_container_->clear();
        new Wt::WText(tr("thechess.comment"), comment_container_);
        new Wt::WText(": ", comment_container_);
        if (game_->can_comment(tApp->user()))
        {
            Wt::WInPlaceEdit* comment =
                new Wt::WInPlaceEdit(game_->comment(), comment_container_);
            comment->setEmptyText(tr("thechess.comment_welcome"));
            comment->valueChanged().connect(this,
                &GameWidgetImpl::comment_handler_);
        }
        else
        {
            new Wt::WText(game_->comment(), comment_container_);
        }
        t.commit();
    }

};

GameWidget::GameWidget(GamePtr game, Wt::WContainerWidget* parent) :
WCompositeWidget(parent)
{
    impl_ = new GameWidgetImpl(game);
    setImplementation(impl_);
}

}
}
