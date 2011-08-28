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
#include <boost/foreach.hpp>

#include "model/Game.hpp"
#include "model/EloPlayer.hpp"
#include "rand.hpp"
#include "TaskTracker.hpp"
#include "chess/Board.hpp"

namespace thechess {

Game::Game() {
}

Game::Game(bool):
    GameParameters(true),
    state_(PROPOSED),
    colors_random_(false),
    created_(now()),
    competition_stage_(-1),
    pause_proposed_td_(TD_NULL),
    mistake_move_(-1) {
    rating_after_[WHITE] = -1;
    rating_after_[BLACK] = -1;
    competition_confirmer_[WHITE] = false;
    competition_confirmer_[BLACK] = false;
}

bool Game::is_ended() const {
    return is_draw() || is_win() || state() == CANCELLED;
}

bool Game::is_draw() const {
    return state() == DRAW_STALEMATE || state() == DRAW_AGREED ||
           state() == DRAW_50 || state() == DRAW_3 || state() == DRAW_2_KINGS;
}

bool Game::is_win() const {
    return state() == SURRENDERED || state() == TIMEOUT ||
           state() == MATE || state() == NO_DRAW_STALEMATE;
}

const char* Game::state2str_id(State state) {
    if (state == PROPOSED) {
        return "tc.game.State_proposed";
    }
    if (state == CONFIRMED) {
        return "tc.game.State_confirmed";
    }
    if (state == ACTIVE) {
        return "tc.game.State_active";
    }
    if (state == PAUSE) {
        return "tc.game.State_pause";
    }
    if (state == DRAW_STALEMATE) {
        return "tc.game.State_draw_stalemate";
    }
    if (state == DRAW_AGREED) {
        return "tc.game.State_draw_agreed";
    }
    if (state == DRAW_50) {
        return "tc.game.State_draw_50";
    }
    if (state == DRAW_3) {
        return "tc.game.State_draw_3";
    }
    if (state == DRAW_2_KINGS) {
        return "tc.game.State_draw_2_kings";
    }
    if (state == SURRENDERED) {
        return "tc.game.State_surrendered";
    }
    if (state == TIMEOUT) {
        return "tc.game.State_timeout";
    }
    if (state == CANCELLED) {
        return "tc.game.State_cancelled";
    }
    if (state == MATE) {
        return "tc.game.State_mate";
    }
    if (state == NO_DRAW_STALEMATE) {
        return "tc.game.State_no_draw_stalemate";
    }
    return "tc.game.State_state";
}

Wt::WString Game::str_state() const {
    return Wt::WString::tr(state2str_id(state()));
}

Color Game::color_of(const UserPtr user) const {
    Color color = COLOR_NULL;
    if (user) {
        if (user == white()) {
            color = WHITE;
        }
        if (user == black()) {
            color = BLACK;
        }
    }
    return color;
}

bool Game::is_member(const UserPtr user) const {
    return user && (user == white() || user == black() || user == init());
}

UserPtr Game::user_of(Color color) const {
    return (color == WHITE) ?
           white() : ((color == BLACK) ? black() : UserPtr());
}

UserPtr Game::other_user(const UserPtr user) const {
    return user_of(other_color(color_of(user)));
}

void Game::check(Objects& objects) {
    if (state() == PROPOSED && competition() && competition()->type() == STAGED &&
            now() - created_ > competition()->relax_time()) {
        confirm_();
    }
    if (state() == CONFIRMED && white()->online() && black()->online()) {
        start_();
    } else if (state() == CONFIRMED && competition()) {
        if (now() - confirmed_ > competition()->force_start_delay()) {
            start_();
        }
    } else if (state() == PAUSE && now() > pause_until()) {
        stop_pause_();
    } else if (state() == ACTIVE && total_limit_now(order_user()) < TD_NULL) {
        UserPtr winner = user_of(other_color(order_color()));
        finish_(TIMEOUT, winner);
    }
    if (is_ended() && competition()) {
        objects.push_back(Object(COMPETITION, competition().id()));
    }
}

Td Game::limit_private(Color color) const {
    Td result;
    if (state() < Game::ACTIVE) {
        result = limit_private_init();
    } else if (color != COLOR_NULL) {
        result = limit_private_[color];
    } else {
        result = TD_NULL;
    }
    return result;
}

Td Game::limit_private(UserPtr user) const {
    return limit_private(color_of(user));
}

Td Game::spent_time() const {
    if (state() == ACTIVE) {
        return now() - lastmove();
    } else if (state() == PAUSE) {
        return pause_started() - lastmove();
    } else if (is_ended()) {
        return ended() - lastmove();
    } else {
        return TD_NULL;
    }
}

Td Game::spent_time(UserPtr user) const {
    return (user == order_user()) ? spent_time() : TD_NULL;
}

Td Game::total_limit(UserPtr user) const {
    return limit_private(user) + limit_std();
}

Td Game::total_limit_now(UserPtr user) const {
    return total_limit(user) - spent_time(user);
}

Td Game::limit_private_now(UserPtr user) const {
    return std::max(TD_NULL,
                    std::min(limit_private(user), total_limit_now(user)));
}

Td Game::limit_std_now(UserPtr user) const {
    return std::max(TD_NULL, limit_std() - spent_time(user));
}

Wt::WDateTime Game::next_check() const {
    Wt::WDateTime result;
    if (state() == ACTIVE) {
        result = lastmove() + limit_std() +
                 std::min(limit_private(WHITE),
                          limit_private(BLACK));
    } else if (state() == PROPOSED && competition() && competition()->type() == STAGED) {
        result = created_ + competition()->relax_time();
    } else if (state() == CONFIRMED && competition()) {
        result = confirmed_ + competition()->force_start_delay();
    } else if (state() == PAUSE) {
        result = pause_until();
    }
    return result;
}

void Game::propose_game(UserPtr init, UserPtr u, Color c) {
    init_ = init;
    if (c == COLOR_NULL) {
        set_random_(init, u);
    } else {
        set_of_color_(init, c);
        set_of_color_(u, other_color(c));
    }
}

void Game::propose_challenge(UserPtr init, Color c) {
    init_ = init;
    if (c != COLOR_NULL) {
        set_of_color_(init, c);
    } else {
        colors_random_ = true;
    }
}

void Game::make_competition_game(UserPtr white, UserPtr black,
                                 CompetitionPtr competition, int competition_stage, bool random) {
    if (random) {
        set_random_(white, black);
    } else {
        white_ = white;
        black_ = black;
    }
    competition_ = competition;
    competition_stage_ = competition_stage;
}

void Game::set_random_(UserPtr user1, UserPtr user2) {
    colors_random_ = true;
    if (rr(2) == 0) {
        white_ = user1;
        black_ = user2;
    } else {
        white_ = user2;
        black_ = user1;
    }
}

void Game::set_of_color_(UserPtr user, Color color) {
    if (color == WHITE) {
        set_white_(user);
    }
    if (color == BLACK) {
        set_black_(user);
    }
}

bool Game::is_challenge() const {
    return state() == PROPOSED && (!white() || !black());
}

bool Game::is_creation() const {
    return state() == PROPOSED && white() && black() && init();
}

bool Game::can_join(UserPtr user) const {
    return user && !is_member(user) && is_challenge();
}

void Game::join(UserPtr user) {
    if (can_join(user)) {
        if (white()) {
            black_ = user;
        } else if (black()) {
            white_ = user;
        } else {
            set_random_(init(), user);
        }
        confirm_();
    }
}

bool Game::can_confirm(UserPtr user) const {
    return user && is_member(user) && is_creation();
}

void Game::confirm(UserPtr user) {
    if (can_confirm(user)) {
        confirm_();
    }
}

void Game::confirm_by_competition() {
    if (competition()) {
        confirm_();
    }
}

void Game::confirm_() {
    confirmed_ = now();
    state_ = CONFIRMED;
}

void Game::start_() {
    state_ = ACTIVE;
    limit_private_[WHITE] = limit_private_init();
    limit_private_[BLACK] = limit_private_init();
    pause_limit_ = pause_limit_init();
    pause_proposer_.reset();
    mistake_proposer_.reset();
    started_ = now();
    lastmove_ = now();
}

void Game::stop_pause_() {
    state_ = ACTIVE;
    lastmove_ += pause_proposed_td_;
    pause_proposed_td_ = TD_NULL;
    pause_until_ = Wt::WDateTime();
}

bool Game::can_cancel(UserPtr user) const {
    return is_member(user) && user != init() && is_creation();
}

void Game::cancel(UserPtr user) {
    if (can_cancel(user)) {
        finish_(CANCELLED);
    }
}

bool Game::has_competition_confirmed(UserPtr user) const {
    return is_member(user) && competition_confirmer_[color_of(user)];
}

bool Game::can_competition_confirm(UserPtr user) const {
    return is_member(user) &&
           competition() &&
           !init() &&
           state_ == PROPOSED &&
           !has_competition_confirmed(user);
}

void Game::competition_confirm(UserPtr user) {
    if (can_competition_confirm(user)) {
        competition_confirmer_[color_of(user)] = true;
        if (has_competition_confirmed(other_user(user))) {
            confirm_();
        }
    }
}

bool Game::can_competition_discard(UserPtr user) const {
    return is_member(user) &&
           competition() &&
           !init() &&
           state_ == PROPOSED &&
           has_competition_confirmed(user);
}

void Game::competition_discard(UserPtr user) {
    if (can_competition_discard(user)) {
        competition_confirmer_[color_of(user)] = false;
    }
}

bool Game::can_move(UserPtr user) const {
    return state() == ACTIVE && !winner() && user == order_user();
}

void Game::add_move(const HalfMove& half_move,
                    const Board& board_after) {
    if (state() == ACTIVE) {
        draw_discard(order_user());
        Td penalty = spent_time() - limit_std();
        if (penalty > TD_NULL) {
            limit_private_[order_color()] -= penalty;
        }
        lastmove_ = now();
        FinishState s = board_after.test_end();
        UserPtr order_user_now = order_user();
        push_move_(half_move);
        if (s == CHECKMATE) {
            finish_(MATE, order_user_now);
        }
        if (s == STALEMATE) {
            if (first_draw() != NO_DRAW) {
                finish_(DRAW_STALEMATE);
            } else {
                finish_(NO_DRAW_STALEMATE, black());
            }
        }
    }
}

bool Game::can_pause_propose(const UserPtr user) const {
    return state() == ACTIVE &&
           !is_pause_proposed() &&
           pause_limit() > TD_NULL &&
           is_member(user);
}

bool Game::can_pause_propose(const UserPtr user, const Td& td) const {
    return can_pause_propose(user) && pause_limit() >= td && td > TD_NULL;
}

void Game::pause_propose(const UserPtr user, const Td& td) {
    if (can_pause_propose(user, td)) {
        pause_proposed_td_ = td;
        pause_proposer_ = user;
    }
}

bool Game::is_pause_proposed() const {
    return pause_proposer() && pause_proposed_td() != TD_NULL;
}

Wt::WDateTime Game::pause_started() const {
    return pause_until() - pause_proposed_td();
}

bool Game::can_pause_agree(const UserPtr user) const {
    return can_pause_discard(user) && pause_proposer() == other_user(user);
}

void Game::pause_agree(const UserPtr user) {
    if (can_pause_agree(user)) {
        pause_proposer_.reset();
        state_ = PAUSE;
        pause_until_ = now() + pause_proposed_td();
    }
}

bool Game::can_pause_discard(const UserPtr user) const {
    return state() == ACTIVE && is_pause_proposed() && is_member(user);
}

void Game::pause_discard(const UserPtr user) {
    if (can_pause_discard(user)) {
        pause_proposer_.reset();
        pause_proposed_td_ = TD_NULL;
    }
}

bool Game::can_mistake_propose(const UserPtr user) const {
    return state() == ACTIVE &&
           size_without_init() > 0 &&
           !is_mistake_proposed() &&
           is_member(user);
}

bool Game::can_mistake_propose(const UserPtr user, int mistake_move) const {
    return can_mistake_propose(user) &&
           mistake_move >= moves_init() && mistake_move < moves().size();
}

void Game::mistake_propose(const UserPtr user, int mistake_move) {
    if (can_mistake_propose(user, mistake_move)) {
        mistake_move_ = mistake_move;
        mistake_proposer_ = user;
    }
}

bool Game::is_mistake_proposed() const {
    return mistake_proposer() && mistake_move() != -1;
}

bool Game::can_mistake_agree(const UserPtr user) const {
    return can_mistake_discard(user) &&
           mistake_proposer() == other_user(user);
}

void Game::mistake_agree(const UserPtr user) {
    if (can_mistake_agree(user)) {
        Wt::WDateTime cached_now = now();
        Td spent = cached_now - lastmove_;
        if (spent <= pause_limit_) {
            pause_limit_ -= spent;
        } else {
            spent -= pause_limit_;
            pause_limit_ = TD_NULL;
            limit_private_[order_color()] -= spent;
        }
        lastmove_ = cached_now;
        mistake_proposer_.reset();
        pop_moves_(moves().size() - mistake_move());
        mistake_move_ = -1;
    }
}

bool Game::can_mistake_discard(const UserPtr user) const {
    return state() == ACTIVE && is_mistake_proposed() && is_member(user);
}

void Game::mistake_discard(const UserPtr user) {
    if (can_mistake_discard(user)) {
        mistake_proposer_.reset();
        mistake_move_ = -1;
    }
}

bool Game::can_draw_propose(const UserPtr user) const {
    return state() == ACTIVE &&
           meet_first_draw() &&
           !is_draw_proposed() &&
           is_member(user);
}

void Game::draw_propose(const UserPtr user) {
    if (can_draw_propose(user)) {
        draw_proposer_ = user;
    }
}

bool Game::can_draw_agree(const UserPtr user) const {
    return can_draw_discard(user) && meet_first_draw();
}

void Game::draw_agree(const UserPtr user) {
    if (can_draw_agree(user)) {
        draw_proposer_.reset();
        finish_(DRAW_AGREED);
    }
}

bool Game::can_draw_discard(const UserPtr user) const {
    return state() == ACTIVE &&
           is_draw_proposed() &&
           is_member(user) &&
           draw_proposer() == other_user(user);
}

void Game::draw_discard(const UserPtr user) {
    if (can_draw_discard(user)) {
        draw_proposer_.reset();
    }
}

Color Game::order_color() const {
    return (moves().size() % 2) ? BLACK : WHITE;
}

UserPtr Game::order_user() const {
    UserPtr result;
    if (order_color() == WHITE) {
        result = white();
    }
    if (order_color() == BLACK) {
        result = black();
    }
    return result;
}

int Game::size_without_init() const {
    return size() - moves_init() ;
}

bool Game::meet_first_draw() const {
    return size_without_init() >= first_draw() && first_draw() != NO_DRAW;
}

bool Game::real_rating() const {
    return !norating() && meet_first_draw();
}

void Game::finish_(State state, UserPtr winner) {
    state_ = state;
    if (winner) {
        winner_ = winner;
    }
    ended_ = now();
    if (real_rating()) {
        elo_change_();
    }
}

void Game::elo_change_() {
    white_.reread();
    black_.reread();
    if (is_win()) {
        winner_.modify()->games_stat().win(&(other_user(winner_).modify()->games_stat()));
    }
    if (is_draw()) {
        white_.modify()->games_stat().draw(&(black_.modify()->games_stat()));
    }
    rating_after_[WHITE] = white()->games_stat().elo();
    rating_after_[BLACK] = black()->games_stat().elo();
}

void Game::push_move_(HalfMove half_move) {
    moves_.push_move(half_move);
}

void Game::pop_moves_(int number) {
    moves_.pop_moves(number);
}

int Game::rating_after(Color color) const {
    return (color == COLOR_NULL) ? -1 : rating_after_[color];
}

bool Game::can_comment(const UserPtr user) const {
    return is_member(user);
}

void Game::set_comment(const UserPtr user, const Wt::WString& t) {
    if (can_comment(user)) {
        comment_ = t;
    }
}

const char* Game::pgn_termination_() const {
    if (!is_ended()) {
        return "unterminated";
    } else if (state_ == TIMEOUT) {
        return "time forfeit";
    } else if (state_ == DRAW_50 || state_ == DRAW_3 || state_ == DRAW_2_KINGS) {
        return "adjudication";
    } else if (state_ == CANCELLED) {
        return "abandoned";    // ?? FIXME
    }
    return "normal";
}

void Game::pgn_init_moves_(std::ostream& out) const {
    out << "[SetUp \"" << "1" << "\"]" << std::endl;
    Board board;
    int halfmove_clock = 0;
    THECHESS_MOVES_TO (move_it, &(moves_), board, moves_init()) {
        HalfMove half_move(*move_it);
        if (board.letter(half_move.from()) == PAWN || board.test_takes(half_move)) {
            halfmove_clock = 0;
        } else {
            halfmove_clock += 1;
        }
    }
    int fullmove_number = Moves::size_to_human(moves_init() + 1);
    out << "[FEN \"";
    board.fen(out, halfmove_clock, fullmove_number);
    out << "\"]" << std::endl;
}

void Game::pgn_additional_(std::ostream& out) const {
    out << "[PlyCount \"" << size() << "\"]" << std::endl;
    long t1 = limit_private_init().total_seconds();
    long t2 = limit_std().total_seconds();
    out << "[TimeControl \"" << t1 << '/' << t2 << "\"]" << std::endl;
    if (started_.isValid()) {
        out << "[UTCTime \"" << started_.toString("HH:mm:ss") << "\"]" << std::endl;
    }
    out << "[Termination \"" << pgn_termination_() << "\"]" << std::endl;
    out << "[Mode \"" << "ICS" << "\"]" << std::endl;
    if (rating_after(WHITE) != -1) {
        out << "[WhiteElo \"" << rating_after(WHITE) << "\"]" << std::endl;
        out << "[BlackElo \"" << rating_after(BLACK) << "\"]" << std::endl;
    }
    if (moves_init()) {
        pgn_init_moves_(out);
    }
}

// see http://cfajohnson.com/chess/SAN/SAN_DOC/Standard
void Game::pgn(std::ostream& out, bool reduced) const {
    Wt::WString event = competition_ ? competition_->name() : "?";
    Wt::WString site = "FIXME";
    Wt::WString date = started_.isValid() ? started_.toString("yyyy.MM.dd") : "????.??.??";
    Wt::WString round = competition_stage_ != -1 ? boost::lexical_cast<std::string>(competition_ + 1) : "-";
    Wt::WString white = white_ ? white_->username() : "?";
    Wt::WString black = black_ ? black_->username() : "?";
    std::string result = is_draw() ? "1/2-1/2" : !winner_ ? "*" : winner_ == white_ ? "1-0" : "0-1";
    out << "[Event \"" << event << "\"]" << std::endl;
    out << "[Site \"" << site << "\"]" << std::endl;
    out << "[Date \"" << date << "\"]" << std::endl;
    out << "[Round \"" << round << "\"]" << std::endl;
    out << "[White \"" << white << "\"]" << std::endl;
    out << "[Black \"" << black << "\"]" << std::endl;
    out << "[Result \"" << result << "\"]" << std::endl;
    if (!reduced) {
        pgn_additional_(out);
    }
    out << std::endl;
    moves_.pgn(out, result, reduced);
}

}

