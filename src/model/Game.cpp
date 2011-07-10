/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <list>
#include <utility>
#include <map>

#include "model/Game.hpp"
#include "model/EloPlayer.hpp"
#include "rand.hpp"
#include "TaskTracker.hpp"

namespace thechess {
namespace model {

Game::Game()
{
}

Game::Game(bool):
GameParameters(true),
state_(proposed),
colors_random_(false),
created_(now()),
competition_stage_(-1),
pause_proposed_td_(td_null),
mistake_move_(-1)
{
    rating_after_[chess::white] = -1;
    rating_after_[chess::black] = -1;
    competition_confirmer_[chess::white] = false;
    competition_confirmer_[chess::black] = false;
}

bool Game::is_ended() const
{
    return is_draw() || is_win() || state() == cancelled;
}

bool Game::is_draw() const
{
    return state() == draw_stalemate || state() == draw_agreed ||
        state() == draw_50 || state() == draw_3 || state() == draw_2_kings;
}

bool Game::is_win() const
{
    return state() == surrendered || state() == timeout ||
        state() == mate || state() == no_draw_stalemate;
}

const char* Game::state2str_id(State state)
{
    if (state == proposed) { return "thechess.state.proposed"; }
    if (state == confirmed) { return "thechess.state.confirmed"; }
    if (state == active) { return "thechess.state.active"; }
    if (state == pause) { return "thechess.state.pause"; }
    if (state == draw_stalemate){return "thechess.state.draw_stalemate";}
    if (state == draw_agreed) { return "thechess.state.draw_agreed"; }
    if (state == draw_50) { return "thechess.state.draw_50"; }
    if (state == draw_3) { return "thechess.state.draw_3"; }
    if (state == draw_2_kings) { return "thechess.state.draw_2_kings"; }
    if (state == surrendered) { return "thechess.state.surrendered"; }
    if (state == timeout) { return "thechess.state.timeout"; }
    if (state == cancelled) { return "thechess.state.cancelled"; }
    if (state == mate) { return "thechess.state.mate"; }
    if (state == no_draw_stalemate) { return "thechess.state.no_draw_stalemate"; }
    return "thechess.state.state";
}

Wt::WString Game::str_state() const
{
    return Wt::WString::tr(state2str_id(state()));
}

chess::Color Game::color_of(const UserPtr user) const
{
    chess::Color color = chess::color_null;
    if (user)
    {
        if (user == white())
        {
            color = chess::white;
        }
        if (user == black())
        {
            color = chess::black;
        }
    }
    return color;
}

bool Game::is_member(const UserPtr user) const
{
    return user && (user == white() || user == black() || user == init());
}

UserPtr Game::user_of(chess::Color color) const
{
    return (color == chess::white) ?
        white() : ((color == chess::black) ? black() : UserPtr());
}

UserPtr Game::other_user(const UserPtr user) const
{
    return user_of(chess::other_color(color_of(user)));
}

void Game::check(Objects& objects)
{
    if (state() == proposed && competition() && competition()->type() == STAGED &&
        now() - created_ > competition()->relax_time())
    {
        confirm_();
    }
    if (state() == confirmed && white()->online() && black()->online())
    {
        start_();
    }
    else if (state() == confirmed && competition())
    {
        if (now() - confirmed_ > competition()->force_start_delay())
        {
            start_();
        }
    }
    else if (state() == pause && now() > pause_until())
    {
        stop_pause_();
    }
    else if (state() == active && total_limit_now(order_user()) < td_null)
    {
        UserPtr winner = user_of(chess::other_color(order_color()));
        finish_(timeout, winner);
    }
    if (is_ended() && competition())
    {
        objects.push_back(Object(CompetitionObject, competition().id()));
    }
}

Td Game::limit_private(chess::Color color) const
{
    Td result;
    if (state() < Game::active)
    {
        result = limit_private_init();
    }
    else if (color != chess::color_null)
    {
        result = limit_private_[color];
    }
    else
    {
        result = td_null;
    }
    return result;
}

Td Game::limit_private(UserPtr user) const
{
    return limit_private(color_of(user));
}

Td Game::spent_time() const
{
    if (state() == active)
    {
        return now() - lastmove();
    }
    else if (state() == pause)
    {
        return pause_started() - lastmove();
    }
    else if (is_ended())
    {
        return ended() - lastmove();
    }
    else
    {
        return td_null;
    }
}

Td Game::spent_time(UserPtr user) const
{
    return (user == order_user()) ? spent_time() : td_null;
}

Td Game::total_limit(UserPtr user) const
{
    return limit_private(user) + limit_std();
}

Td Game::total_limit_now(UserPtr user) const
{
    return total_limit(user) - spent_time(user);
}

Td Game::limit_private_now(UserPtr user) const
{
    return std::max(td_null,
        std::min(limit_private(user), total_limit_now(user)));
}

Td Game::limit_std_now(UserPtr user) const
{
    return std::max(td_null, limit_std() - spent_time(user));
}

Wt::WDateTime Game::next_check() const
{
    Wt::WDateTime result;
    if (state() == active)
    {
        result = lastmove() + limit_std() +
            std::min(limit_private(chess::white),
            limit_private(chess::black));
    }
    else if (state() == proposed && competition() && competition()->type() == STAGED)
    {
        result = created_ + competition()->relax_time();
    }
    else if (state() == confirmed && competition())
    {
        result = confirmed_ + competition()->force_start_delay();
    }
    else if (state() == pause)
    {
        result = pause_until();
    }
    return result;
}

void Game::propose_game(UserPtr init, UserPtr u, chess::Color c)
{
    init_ = init;
    if (c == chess::color_null)
    {
        set_random_(init, u);
    }
    else
    {
        set_of_color_(init, c);
        set_of_color_(u, chess::other_color(c));
    }
}

void Game::propose_challenge(UserPtr init, chess::Color c)
{
    init_ = init;
    if (c != chess::color_null)
    {
        set_of_color_(init, c);
    }
    else
    {
        colors_random_ = true;
    }
}

void Game::make_competition_game(UserPtr white, UserPtr black,
    CompetitionPtr competition, int competition_stage, bool random)
{
    if (random)
    {
        set_random_(white, black);
    }
    else
    {
        white_ = white;
        black_ = black;
    }
    competition_ = competition;
    competition_stage_ = competition_stage;
}

void Game::set_random_(UserPtr user1, UserPtr user2)
{
    colors_random_ = true;
    if (random::rr(2) == 0)
    {
        white_ = user1;
        black_ = user2;
    }
    else
    {
        white_ = user2;
        black_ = user1;
    }
}

void Game::set_of_color_(UserPtr user, chess::Color color)
{
    if (color == chess::white)
    {
        set_white_(user);
    }
    if (color == chess::black)
    {
        set_black_(user);
    }
}

bool Game::is_challenge() const
{
    return state() == proposed && (!white() || !black());
}

bool Game::is_creation() const
{
    return state() == proposed && white() && black() && init();
}

bool Game::can_join(UserPtr user) const
{
    return user && user != init() && is_challenge();
}

void Game::join(UserPtr user)
{
    if (can_join(user))
    {
        if (white())
        {
            black_ = user;
        }
        else if (black())
        {
            white_ = user;
        }
        else
        {
            set_random_(init(), user);
        }
        confirm_();
    }
}

bool Game::can_confirm(UserPtr user) const
{
    return user && user != init() && is_creation();
}

void Game::confirm(UserPtr user)
{
    if (can_confirm(user))
    {
        confirm_();
    }
}

void Game::confirm_by_competition()
{
    if (competition())
    {
        confirm_();
    }
}

void Game::confirm_()
{
    confirmed_ = now();
    state_ = confirmed;
}

void Game::start_()
{
    state_ = active;
    limit_private_[chess::white] = limit_private_init();
    limit_private_[chess::black] = limit_private_init();
    pause_limit_ = pause_limit_init();
    pause_proposer_.reset();
    mistake_proposer_.reset();
    started_ = now();
    lastmove_ = now();
}

void Game::stop_pause_()
{
    state_ = active;
    lastmove_ += pause_proposed_td_;
    pause_proposed_td_ = td_null;
    pause_until_ = Wt::WDateTime();
}

bool Game::can_cancel(UserPtr user) const
{
    return is_member(user) && user != init() && is_creation();
}

void Game::cancel(UserPtr user)
{
    if (can_cancel(user))
    {
        finish_(cancelled);
    }
}

bool Game::has_competition_confirmed(UserPtr user) const
{
    return is_member(user) && competition_confirmer_[color_of(user)];
}

bool Game::can_competition_confirm(UserPtr user) const
{
    return is_member(user) &&
        competition() &&
        !init() &&
        state_ == proposed &&
        !has_competition_confirmed(user);
}

void Game::competition_confirm(UserPtr user)
{
    if (can_competition_confirm(user))
    {
        competition_confirmer_[color_of(user)] = true;
        if (has_competition_confirmed(other_user(user)))
        {
            confirm_();
        }
    }
}

bool Game::can_competition_discard(UserPtr user) const
{
    return is_member(user) &&
        competition() &&
        !init() &&
        state_ == proposed &&
        has_competition_confirmed(user);
}

void Game::competition_discard(UserPtr user)
{
    if (can_competition_discard(user))
    {
        competition_confirmer_[color_of(user)] = false;
    }
}

bool Game::can_move(UserPtr user) const
{
    return state() == active && !winner() && user == order_user();
}

void Game::add_move(const chess::Move& move,
    const chess::Board& board_after)
{
    if (state() == active)
    {
        draw_discard(order_user());
        Td penalty = spent_time() - limit_std();
        if (penalty > td_null)
        {
            limit_private_[order_color()] -= penalty;
        }
        lastmove_ = now();
        chess::FinishState s = board_after.test_end();
        UserPtr order_user_now = order_user();
        push_move_(move);
        if (s == chess::checkmate)
        {
            finish_(mate, order_user_now);
        }
        if (s == chess::stalemate)
        {
            if (first_draw() != NO_DRAW)
            {
                finish_(draw_stalemate);
            }
            else
            {
                finish_(no_draw_stalemate, black());
            }
        }
    }
}

bool Game::can_pause_propose(const UserPtr user) const
{
    return state() == active &&
        !is_pause_proposed() &&
        pause_limit() > td_null &&
        is_member(user);
}

bool Game::can_pause_propose(const UserPtr user, const Td& td) const
{
    return can_pause_propose(user) && pause_limit() >= td && td > td_null;
}

void Game::pause_propose(const UserPtr user, const Td& td)
{
    if (can_pause_propose(user, td))
    {
        pause_proposed_td_ = td;
        pause_proposer_ = user;
    }
}

bool Game::is_pause_proposed() const
{
    return pause_proposer() && pause_proposed_td() != td_null;
}

Wt::WDateTime Game::pause_started() const
{
    return pause_until() - pause_proposed_td();
}

bool Game::can_pause_agree(const UserPtr user) const
{
    return can_pause_discard(user) && pause_proposer() == other_user(user);
}

void Game::pause_agree(const UserPtr user)
{
    if (can_pause_agree(user))
    {
        pause_proposer_.reset();
        state_ = pause;
        pause_until_ = now() + pause_proposed_td();
    }
}

bool Game::can_pause_discard(const UserPtr user) const
{
    return state() == active && is_pause_proposed() && is_member(user);
}

void Game::pause_discard(const UserPtr user)
{
    if (can_pause_discard(user))
    {
        pause_proposer_.reset();
        pause_proposed_td_ = td_null;
    }
}

bool Game::can_mistake_propose(const UserPtr user) const
{
    return state() == active &&
        size_without_init() > 0 &&
        !is_mistake_proposed() &&
        is_member(user);
}

bool Game::can_mistake_propose(const UserPtr user, int mistake_move) const
{
    return can_mistake_propose(user) &&
        mistake_move >= moves_init() && mistake_move < moves().size();
}

void Game::mistake_propose(const UserPtr user, int mistake_move)
{
    if (can_mistake_propose(user, mistake_move))
    {
        mistake_move_ = mistake_move;
        mistake_proposer_ = user;
    }
}

bool Game::is_mistake_proposed() const
{
    return mistake_proposer() && mistake_move() != -1;
}

bool Game::can_mistake_agree(const UserPtr user) const
{
    return can_mistake_discard(user) &&
        mistake_proposer() == other_user(user);
}

void Game::mistake_agree(const UserPtr user)
{
    if (can_mistake_agree(user))
    {
        Wt::WDateTime cached_now = now();
        Td spent = cached_now - lastmove_;
        if (spent <= pause_limit_)
        {
            pause_limit_ -= spent;
        }
        else
        {
            spent -= pause_limit_;
            pause_limit_ = td_null;
            limit_private_[order_color()] -= spent;
        }
        lastmove_ = cached_now;
        mistake_proposer_.reset();
        pop_moves_(moves().size() - mistake_move());
        mistake_move_ = -1;
    }
}

bool Game::can_mistake_discard(const UserPtr user) const
{
    return state() == active && is_mistake_proposed() && is_member(user);
}

void Game::mistake_discard(const UserPtr user)
{
    if (can_mistake_discard(user))
    {
        mistake_proposer_.reset();
        mistake_move_ = -1;
    }
}

bool Game::can_draw_propose(const UserPtr user) const
{
    return state() == active &&
        meet_first_draw() &&
        !is_draw_proposed() &&
        is_member(user);
}

void Game::draw_propose(const UserPtr user)
{
    if (can_draw_propose(user))
    {
        draw_proposer_ = user;
    }
}

bool Game::can_draw_agree(const UserPtr user) const
{
    return can_draw_discard(user) && meet_first_draw();
}

void Game::draw_agree(const UserPtr user)
{
    if (can_draw_agree(user))
    {
        draw_proposer_.reset();
        finish_(draw_agreed);
    }
}

bool Game::can_draw_discard(const UserPtr user) const
{
    return state() == active &&
        is_draw_proposed() &&
        is_member(user) &&
        draw_proposer() == other_user(user);
}

void Game::draw_discard(const UserPtr user)
{
    if (can_draw_discard(user))
    {
        draw_proposer_.reset();
    }
}

chess::Color Game::order_color() const
{
    return (moves().size() % 2) ? chess::black : chess::white;
}

UserPtr Game::order_user() const
{
    UserPtr result;
    if (order_color() == chess::white) result = white();
    if (order_color() == chess::black) result = black();
    return result;
}

int Game::size_without_init() const
{
    return size() - moves_init() ;
}

bool Game::meet_first_draw() const
{
    return size_without_init() >= first_draw() && first_draw() != NO_DRAW;
}

bool Game::real_rating() const
{
    return !norating() && meet_first_draw();
}

void Game::finish_(State state, UserPtr winner)
{
    state_ = state;
    if (winner)
    {
        winner_ = winner;
    }
    ended_ = now();
    if (real_rating())
    {
        elo_change_();
    }
}

void Game::elo_change_()
{
    white_.reread();
    black_.reread();
    if (is_win())
    {
        winner_.modify()->games_stat().win(&(other_user(winner_).modify()->games_stat()));
    }
    if (is_draw())
    {
        white_.modify()->games_stat().draw(&(black_.modify()->games_stat()));
    }
    rating_after_[chess::white] = white()->games_stat().elo();
    rating_after_[chess::black] = black()->games_stat().elo();
}

void Game::push_move_(chess::Move move)
{
    moves_.push_move(move);
}

void Game::pop_moves_(int number)
{
    moves_.pop_moves(number);
}

int Game::rating_after(chess::Color color) const
{
    return (color == chess::color_null) ? -1 : rating_after_[color];
}

bool Game::can_comment(const UserPtr user) const
{
    return is_member(user);
}

void Game::set_comment(const UserPtr user, const Wt::WString& t)
{
    if (can_comment(user))
    {
        comment_ = t;
    }
}

}
}

