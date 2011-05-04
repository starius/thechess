#include <list>
#include <utility>
#include <map>

#include "model/Game.hpp"
#include "model/EloPlayer.hpp"
#include "rand.hpp"
#include "TaskTracker.hpp"

namespace thechess {
namespace model {

typedef std::map<int, Game::GameSignal*> SignalMap;
typedef SignalMap::iterator I;
typedef std::list<I> L;
typedef L::iterator Li;

SignalMap sm;

SignalMap::iterator find_signal(int id)
{
    return sm.find(id);
}

void sm_check()
{
    L to_remove;
    for (I i = sm.begin(); i != sm.end(); ++i)
    {
        Game::GameSignal* s = i->second;
        if (!s->isConnected())
        {
            to_remove.push_back(i);
        }
    }
    for (Li li = to_remove.begin(); li != to_remove.end(); ++li)
    {
        I i = *li;
        Game::GameSignal* s = i->second;
        delete s;
        sm.erase(i);
    }
}

Game::GameSignal& Game::signal() const
{
    int id_ = id() > 0 ? id() : 0;
    I i = find_signal(id_);
    if (i == sm.end())
    {
        sm_check();
        i = sm.insert(std::make_pair(id_, new GameSignal())).first;
    }
    return *(i->second);
}

void Game::signal_emit(Event event, const chess::Move& move)
{
    signal().emit(event, move);
}

void Game::signal_emit(Event event)
{
    signal().emit(event, chess::move_null);
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
    return state() == surrendered || state() == timeout || state() == mate;
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

void Game::check()
{
    check_impl_();
    tracker::check();
}

void Game::check_impl_()
{
    if (state() == confirmed && white()->online() && black()->online())
    {
        start();
    }
    if (state() == pause)
    {
        if (now() > pause_until())
        {
            state_ = active;
            lastmove_ += pause_proposed_td_;
            pause_proposed_td_ = td_null;
            pause_until_ = Wt::WDateTime();
            signal_emit(e_pause);
        }
    }
    if (state() == active)
    {
        if (total_limit_now(order_user()) < td_null)
        {
            UserPtr winner = user_of(chess::other_color(order_color()));
            finish_(timeout, winner);
        }
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
    if (state() == pause)
    {
        result = pause_until();
    }
    return result;
}

void Game::initialize()
{
    GameParameters::initialize();
    state_ = proposed;
    colors_random_ = false;
    mistake_move_ = -1;
    pause_proposed_td_ = td_null;
    created_ = now();
    moves_size_ = moves().size();
    rating_after_[chess::white] = -1;
    rating_after_[chess::black] = -1;
    competition_round_ = -1;
}

void Game::create_game(UserPtr init, UserPtr u, chess::Color c)
{
    initialize();
    init_ = init;
    if (c == chess::color_null)
    {
        colors_random_ = true;
        set_random_(init, u);
    }
    else
    {
        set_of_color_(init, c);
        set_of_color_(u, chess::other_color(c));
    }
}

void Game::create_challenge(UserPtr init, chess::Color c)
{
    initialize();
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

void Game::set_random_(UserPtr user1, UserPtr user2)
{
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
    return state() == proposed && white() && black();
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
        confirm();
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
        confirm();
    }
}

void Game::confirm()
{
    confirmed_ = now();
    state_ = confirmed;
    signal_emit(e_confirm);
    check();
}

void Game::start()
{
    if (state() == confirmed)
    {
        state_ = active;
        limit_private_[0] = limit_private_init();
        limit_private_[1] = limit_private_init();
        pause_limit_ = pause_limit_init();
        pause_proposer_.reset();
        mistake_proposer_.reset();
        started_ = now();
        lastmove_ = now();
        signal_emit(e_start);
    }
}

bool Game::can_cancel(UserPtr user) const
{
    return is_member(user) && user != init() && is_creation();
}

void Game::cancel(UserPtr user)
{
    check();
    if (can_cancel(user))
    {
        finish_(cancelled);
    }
}

bool Game::can_move(UserPtr user) const
{
    return state() == active && !winner() && user == order_user();
}

void Game::add_move(const chess::Move& move,
    const chess::Board& board_after)
{
    check();
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
        signal_emit(e_move, move);
        if (s == chess::checkmate)
        {
            finish_(mate, order_user_now);
        }
        if (s == chess::stalemate)
        {
            finish_(draw_stalemate);
        }
    }
}

bool Game::can_pause_propose(const UserPtr user) const
{
    return state() == active &&
        !is_pause_proposed() &&
        is_member(user);
}

bool Game::can_pause_propose(const UserPtr user, const Td& td) const
{
    return can_pause_propose(user) && pause_limit() >= td && td > td_null;
}

void Game::pause_propose(const UserPtr user, const Td& td)
{
    check();
    if (can_pause_propose(user, td))
    {
        pause_proposed_td_ = td;
        pause_proposer_ = user;
        signal_emit(e_pause);
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
    check();
    if (can_pause_agree(user))
    {
        pause_proposer_.reset();
        state_ = pause;
        pause_until_ = now() + pause_proposed_td();
        signal_emit(e_pause);
        tracker::add_or_update_task(tracker::Game, id());
    }
}

bool Game::can_pause_discard(const UserPtr user) const
{
    return state() == active && is_pause_proposed() && is_member(user);
}

void Game::pause_discard(const UserPtr user)
{
    check();
    if (can_pause_discard(user))
    {
        pause_proposer_.reset();
        pause_proposed_td_ = td_null;
        signal_emit(e_pause);
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
    check();
    if (can_mistake_propose(user, mistake_move))
    {
        mistake_move_ = mistake_move;
        mistake_proposer_ = user;
        signal_emit(e_mistake);
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
    check();
    if (can_mistake_agree(user))
    {
        mistake_proposer_.reset();
        pop_moves_(moves().size() - mistake_move());
        mistake_move_ = -1;
        signal_emit(e_mistake);
    }
}

bool Game::can_mistake_discard(const UserPtr user) const
{
    return state() == active && is_mistake_proposed() && is_member(user);
}

void Game::mistake_discard(const UserPtr user)
{
    check();
    if (can_mistake_discard(user))
    {
        mistake_proposer_.reset();
        mistake_move_ = -1;
        signal_emit(e_mistake);
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
    check();
    if (can_draw_propose(user))
    {
        draw_proposer_ = user;
        signal_emit(e_draw);
    }
}

bool Game::can_draw_agree(const UserPtr user) const
{
    return can_draw_discard(user) && meet_first_draw();
}

void Game::draw_agree(const UserPtr user)
{
    check();
    if (can_draw_agree(user))
    {
        draw_proposer_.reset();
        signal_emit(e_draw);
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
    check();
    if (can_draw_discard(user))
    {
        draw_proposer_.reset();
        signal_emit(e_draw);
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
    return size_without_init() >= first_draw();
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
    signal_emit(e_end);
}

void Game::elo_change_()
{
    if (is_win())
    {
        winner_.modify()->win(other_user(winner_).modify());
    }
    if (is_draw())
    {
        EloPlayer::draw(white().modify(), black().modify());
    }
    rating_after_[chess::white] = white()->elo_rating();
    rating_after_[chess::black] = black()->elo_rating();
}

void Game::push_move_(chess::Move move)
{
    moves_.push_move(move);
    moves_size_ += 1;
}

void Game::pop_moves_(int number)
{
    moves_.pop_moves(number);
    moves_size_ -= 1;
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
        signal_emit(e_comment);
    }
}

}
}
