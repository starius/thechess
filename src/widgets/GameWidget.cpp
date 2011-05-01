#include <iostream>

#include <Wt/WObject>
#include <Wt/WContainerWidget>
#include <Wt/WViewWidget>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WInPlaceEdit>
#include <Wt/WTimer>
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

using model::UserPtr;
using model::GamePtr;
using model::Game;

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

class GameWidgetImpl : public Wt::WContainerWidget
{
public:
    GameWidgetImpl(GamePtr game) :
    Wt::WContainerWidget(), game_(game), app_(tApp)
    {
        dbo::Transaction t(tApp->session());
        game_.modify()->check();

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
        game_->signal()
            .connect(this, &GameWidgetImpl::game_handler_);

        countdown_container_ = new Wt::WContainerWidget(this);
        manager_ = new Wt::WContainerWidget(this);
        new Wt::WText(tr("thechess.comment"), this);
        new Wt::WText(": ", this);
        comment_ = new Wt::WInPlaceEdit(game_->comment(), this);
        comment_->setEmptyText(tr("thechess.comment_welcome"));
        comment_->valueChanged().connect(this,
            &GameWidgetImpl::comment_handler_);
        game_status_ = new GameStatus(game_, this);
        status_and_manager_();
        countdown_print_();
        timer_ = new Wt::WTimer(this);
        timer_->timeout().connect(this, &GameWidgetImpl::handle_timer_);
        setup_timer_();
        t.commit();
    }

private:
    GamePtr game_;
    MovesWidget* moves_widget_;
    GameStatus* game_status_;
    Wt::WContainerWidget* manager_;
    Wt::WContainerWidget* countdown_container_;
    ThechessApplication* app_;
    Wt::WTimer* timer_;
    Wt::WInPlaceEdit* comment_;

    void move_handler_(const chess::Move& move)
    {
        int game_size = game_->moves().size();
        int moves_size = moves_widget_->moves().size();
        // additional protection
        if (game_size == moves_size - 1 && game_->can_move(tApp->user()))
        {
            dbo::Transaction t(tApp->session());
            game_.modify()->add_move(move, moves_widget_->board());
            t.commit();
        }
    }

    void game_handler_(model::Game::Event event, const chess::Move& move)
    {
        dbo::Transaction t(tApp->session());
        Wt::WApplication::UpdateLock lock(app_);
        if (lock)
        {
            moves_widget_->set_active(game_->can_move(app_->user()));
            countdown_print_();
            if (event == model::Game::e_confirm && game_->colors_random())
            {
                moves_widget_->bottom_set(game_->color_of(tApp->user()));
            }
            if (event == model::Game::e_mistake)
            {
                moves_widget_->set_moves(game_->moves());
            }
            if (event == model::Game::e_move)
            {
                moves_widget_->add_move(move);
            }
            if (event == model::Game::e_comment)
            {
                comment_print_();
            }
            status_and_manager_();
            setup_timer_();
            app_->triggerUpdate();
        }
        t.commit();
    }

    void status_and_manager_()
    {
        dbo::Transaction t(tApp->session());
        print_status_();
        manager_->clear();
        if (tApp->user())
        {
            print_manager_();
        }
        else
        {
            new PleaseLoginWidget(manager_);
        }
        t.commit();
    }

    void handle_timer_()
    {
        dbo::Transaction t(tApp->session());
        game_.modify()->check();
        setup_timer_();
        t.commit();
    }

    void setup_timer_()
    {
        dbo::Transaction t(tApp->session());
        timer_->stop();
        if (game_->next_check().isValid())
        {
            timer_->setInterval((game_->next_check() - now()).total_milliseconds());
            timer_->start();
        }
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
        print_analysis_button_();
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
        if (game_->can_join(tApp->user()))
        {
            button_<&Game::join>("thechess.join");
        }
        if (game_->can_confirm(tApp->user()))
        {
            button_<&Game::confirm>("thechess.confirm");
        }
        if (game_->can_cancel(tApp->user()))
        {
            button_<&Game::cancel>("thechess.cancel");
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

    typedef void (model::Game::*GameMember)(UserPtr);
    template <GameMember method>
    void action_()
    {
        dbo::Transaction t(tApp->session());
        (game_.modify()->*method)(tApp->user());
        t.commit();
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
        game_.modify()
            ->pause_propose(tApp->user(), pause_duration->timedelta());
        t.commit();
    }

    void mistake_propose_()
    {
        dbo::Transaction t(tApp->session());
        game_.modify()
            ->mistake_propose(tApp->user(), moves_widget_->current_move());
        t.commit();
    }

    void show_analysis_()
    {
        dbo::Transaction t(tApp->session());
        Wt::WDialog* analysis = new Wt::WDialog(
            tr("thechess.format.analysis").arg((int)game_->id()));
        int max_moves = -1;
        bool active = true;
        bool big = false; // FIXME
        bool append_only = false;
        chess::Color bottom = game_->color_of(tApp->user());
        const chess::Moves& moves = game_->moves();
        new MovesWidget(moves, big, active, max_moves,
            append_only, bottom, analysis->contents());
        Wt::WPushButton* close = new Wt::WPushButton(tr("thechess.close"),
            analysis->contents());
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

    void comment_handler_()
    {
        dbo::Transaction t(tApp->session());
        game_.modify()->set_comment(tApp->user(), comment_->text());
        t.commit();
    }

    void comment_print_()
    {
        dbo::Transaction t(tApp->session());
        comment_->setText(game_->comment());
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
