/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <list>
#include <utility>
#include <map>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/Wc/rand.hpp>

#include "model/all.hpp"
#include "chess/Board.hpp"
#include "chess/CachedMoves.hpp"
#include "log.hpp"
#include "Planning.hpp"
#include "Options.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::Game);

namespace thechess {

Game::Game() {
}

Game::Game(const GPPtr& gp):
    gp_(gp),
    state_(PROPOSED),
    moves_(gp_->moves()),
    competition_confirmer_(false),
    colors_random_(false),
    competition_stage_(-1),
    pause_proposed_td_(TD_NULL),
    mistake_move_(-1),
    rating_after_(-1),
    norating_(false) {
    gp.modify()->set_games_size(gp->games_size() + 1);
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

void Game::stat_change() {
    if (!real_rating()) {
        return;
    }
    if (is_win()) {
        EloPlayer& other_stat = other_user(winner_).modify()->games_stat();
        winner_.modify()->games_stat().win(&other_stat);
    }
    if (is_draw()) {
        white_.modify()->games_stat().draw(&(black_.modify()->games_stat()));
    }
    rating_after_[Piece::WHITE] = white()->games_stat().elo();
    rating_after_[Piece::BLACK] = black()->games_stat().elo();
}

Wt::WString Game::str_state() const {
    return Wt::WString::tr(state2str_id(state()));
}

Piece::Color Game::color_of(const UserPtr& user) const {
    Piece::Color color = Piece::COLOR_NULL;
    if (user) {
        if (user == white()) {
            color = Piece::WHITE;
        }
        if (user == black()) {
            color = Piece::BLACK;
        }
    }
    return color;
}

bool Game::is_member(const UserPtr& user) const {
    return user && (user == white() || user == black() || user == init());
}

UserPtr Game::user_of(Piece::Color color) const {
    return (color == Piece::WHITE) ?
           white() : ((color == Piece::BLACK) ? black() : UserPtr());
}

UserPtr Game::other_user(const UserPtr& user) const {
    return user_of(Piece::other_color(color_of(user)));
}

void Game::check(Wt::Wc::notify::TaskPtr task, Planning* planning) {
    Td game_max_preactive = Options::instance()->game_max_preactive();
    if (state() == PROPOSED && !competition() &&
            now() - created() > game_max_preactive) {
        finish(CANCELLED);
    }
    if (state() == CONFIRMED && !competition() &&
            now() - confirmed_ > game_max_preactive) {
        finish(CANCELLED);
    }
    if (state() == PROPOSED &&
            competition() && competition()->type() == STAGED &&
            now() - created() > competition()->cp()->relax_time()) {
        confirm();
    }
    if (state() == CONFIRMED && white()->online() && black()->online()) {
        start();
    } else if (state() == CONFIRMED && competition()) {
        if (now() - confirmed_ > competition()->cp()->force_start_delay()) {
            start();
        }
    } else if (state() == PAUSE && now() > pause_until()) {
        stop_pause();
    } else if (state() == ACTIVE && total_limit_now(order_user()) < TD_NULL) {
        const UserPtr& winner = user_of(Piece::other_color(order_color()));
        finish(TIMEOUT, winner);
    }
    if (is_ended() && competition()) {
        Object* c = new Object(COMPETITION, competition().id());
        planning->add(c, now(), false);
    }
    planning->add(task, next_check(), false);
}

Td Game::limit_private(Piece::Color color) const {
    Td result;
    if (state() < Game::ACTIVE) {
        result = gp_->limit_private_init();
    } else if (color != Piece::COLOR_NULL) {
        result = limit_private_[color];
    } else {
        result = TD_NULL;
    }
    return result;
}

Td Game::limit_private(const UserPtr& user) const {
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

Td Game::spent_time(const UserPtr& user) const {
    return (user == order_user()) ? spent_time() : TD_NULL;
}

Td Game::total_limit(const UserPtr& user) const {
    return limit_private(user) + gp_->limit_std();
}

Td Game::total_limit_now(const UserPtr& user) const {
    return total_limit(user) - spent_time(user);
}

Td Game::limit_private_now(const UserPtr& user) const {
    return std::max(TD_NULL,
                    std::min(limit_private(user), total_limit_now(user)));
}

Td Game::limit_std_now(const UserPtr& user) const {
    return std::max(TD_NULL, gp_->limit_std() - spent_time(user));
}

Wt::WDateTime Game::next_check() const {
    Td game_max_preactive = Options::instance()->game_max_preactive();
    Wt::WDateTime result;
    if (state() == ACTIVE) {
        result = lastmove() + gp_->limit_std() +
                 std::min(limit_private(Piece::WHITE),
                          limit_private(Piece::BLACK));
    } else if (state() == PROPOSED && !competition()) {
        result = created() + game_max_preactive;
    } else if (state() == CONFIRMED && !competition()) {
        result = confirmed_ + game_max_preactive;
    } else if (state() == PROPOSED &&
               competition() && competition()->type() == STAGED) {
        result = created() + competition()->cp()->relax_time();
    } else if (state() == CONFIRMED && competition()) {
        result = confirmed_ + competition()->cp()->force_start_delay();
    } else if (state() == PAUSE) {
        result = pause_until();
    }
    return result;
}

void Game::propose_game(const UserPtr& init, const UserPtr& u, Piece::Color c) {
    set_init(init);
    if (c == Piece::COLOR_NULL) {
        set_random(init, u);
    } else {
        set_of_color(init, c);
        set_of_color(u, Piece::other_color(c));
    }
}

void Game::propose_challenge(const UserPtr& init, Piece::Color c) {
    set_init(init);
    if (c != Piece::COLOR_NULL) {
        set_of_color(init, c);
    } else {
        colors_random_ = true;
    }
}

void Game::make_competition_game(const UserPtr& white, const UserPtr& black,
                                 const CompetitionPtr& competition,
                                 int competition_stage, bool random) {
    if (random) {
        set_random(white, black);
    } else {
        white_ = white;
        black_ = black;
    }
    competition_ = competition;
    competition_stage_ = competition_stage;
}

void Game::set_random(const UserPtr& user1, const UserPtr& user2) {
    colors_random_ = true;
    if (Wt::Wc::rr(2) == 0) {
        white_ = user1;
        black_ = user2;
    } else {
        white_ = user2;
        black_ = user1;
    }
}

void Game::set_of_color(const UserPtr& user, Piece::Color color) {
    if (color == Piece::WHITE) {
        set_white(user);
    }
    if (color == Piece::BLACK) {
        set_black(user);
    }
}

bool Game::is_challenge() const {
    return state() == PROPOSED && (!white() || !black());
}

bool Game::is_creation() const {
    return state() == PROPOSED && white() && black() && init();
}

bool Game::can_join(const UserPtr& user) const {
    return user && !is_member(user) && is_challenge() &&
           user->has_permission(GAME_JOINER);
}

void Game::join(const UserPtr& user) {
    if (can_join(user)) {
        if (white()) {
            black_ = user;
        } else if (black()) {
            white_ = user;
        } else {
            set_random(init(), user);
        }
        confirm();
    }
}

bool Game::can_confirm(const UserPtr& user) const {
    return user && is_member(user) && user != init() && is_creation();
}

void Game::confirm(const UserPtr& user) {
    if (can_confirm(user)) {
        confirm();
    }
}

void Game::confirm_by_competition() {
    if (competition()) {
        confirm();
    }
}

void Game::confirm() {
    confirmed_ = now();
    state_ = CONFIRMED;
}

void Game::start() {
    state_ = ACTIVE;
    limit_private_[Piece::WHITE] = gp_->limit_private_init();
    limit_private_[Piece::BLACK] = gp_->limit_private_init();
    pause_limit_ = gp_->pause_limit_init();
    pause_proposer_.reset();
    mistake_proposer_.reset();
    started_ = now();
    lastmove_ = now();
}

void Game::stop_pause() {
    state_ = ACTIVE;
    lastmove_ += pause_proposed_td_;
    pause_proposed_td_ = TD_NULL;
    pause_until_ = Wt::WDateTime();
}

bool Game::can_cancel(const UserPtr& user) const {
    return is_member(user) && user != init() && is_creation();
}

void Game::cancel(const UserPtr& user) {
    if (can_cancel(user)) {
        finish(CANCELLED);
    }
}

bool Game::has_competition_confirmed(const UserPtr& user) const {
    return is_member(user) && competition_confirmer_[color_of(user)];
}

bool Game::can_competition_confirm(const UserPtr& user) const {
    return is_member(user) &&
           competition() &&
           !init() &&
           state_ == PROPOSED &&
           !has_competition_confirmed(user);
}

void Game::competition_confirm(const UserPtr& user) {
    if (can_competition_confirm(user)) {
        competition_confirmer_[color_of(user)] = true;
        if (has_competition_confirmed(other_user(user))) {
            confirm();
        }
    }
}

bool Game::can_competition_discard(const UserPtr& user) const {
    return is_member(user) &&
           competition() &&
           !init() &&
           state_ == PROPOSED &&
           has_competition_confirmed(user);
}

void Game::competition_discard(const UserPtr& user) {
    if (can_competition_discard(user)) {
        competition_confirmer_[color_of(user)] = false;
    }
}

bool Game::can_move(const UserPtr& user) const {
    return state() == ACTIVE && !winner() && user == order_user();
}

void Game::add_move(const HalfMove& half_move,
                    const Board& board_after) {
    if (state() == ACTIVE) {
        draw_discard(order_user());
        Td penalty = spent_time() - gp_->limit_std();
        if (penalty > TD_NULL) {
            limit_private_[order_color()] -= penalty;
        }
        lastmove_ = now();
        Board::FinishState s = board_after.test_end();
        UserPtr order_user_now = order_user();
        push_move(half_move);
        if (s == Board::CHECKMATE) {
            finish(MATE, order_user_now);
        }
        if (s == Board::STALEMATE) {
            if (gp_->first_draw() != NO_DRAW) {
                finish(DRAW_STALEMATE);
            } else {
                finish(NO_DRAW_STALEMATE, black());
            }
        }
        Board::PieceStat stat(board_after);
        if (stat.is_material_draw()) {
            if (gp_->first_draw() != NO_DRAW) {
                finish(DRAW_2_KINGS);
            } else {
                finish(NO_DRAW_STALEMATE, black());
            }
        }
    }
}

bool Game::can_pause_propose(const UserPtr& user) const {
    return state() == ACTIVE &&
           !is_pause_proposed() &&
           pause_limit() > TD_NULL &&
           is_member(user);
}

bool Game::can_pause_propose(const UserPtr& user, const Td& td) const {
    return can_pause_propose(user) && pause_limit() >= td && td > TD_NULL;
}

void Game::pause_propose(const UserPtr& user, const Td& td) {
    if (can_pause_propose(user, td)) {
        pause_proposed_td_ = td;
        pause_proposer_ = user;
    }
}

bool Game::is_pause_proposed() const {
    return pause_proposer() && pause_proposed_td() != TD_NULL;
}

bool Game::admin_can_pause_discard(const UserPtr& user) const {
    return state() == PAUSE && user && !is_member(user) &&
           user->has_permission(TIME_WIZARD);
}

void Game::admin_pause_discard(const UserPtr& user) {
    if (admin_can_pause_discard(user)) {
        pause_proposed_td_ -= pause_until_ - now();
        stop_pause();
    }
}

bool Game::admin_can_pause(const UserPtr& user) const {
    return state() == ACTIVE && user && !is_member(user) &&
           user->has_permission(TIME_WIZARD);
}

void Game::admin_pause(const UserPtr& user, const Td& td) {
    if (admin_can_pause(user)) {
        pause_proposer_.reset();
        pause_proposed_td_ = td;
        state_ = PAUSE;
        pause_until_ = now() + pause_proposed_td();
        pause_limit_ = std::max(TD_NULL, pause_limit_ - pause_proposed_td());
    }
}

Wt::WDateTime Game::pause_started() const {
    return pause_until() - pause_proposed_td();
}

bool Game::can_pause_agree(const UserPtr& user) const {
    return can_pause_discard(user) && pause_proposer() == other_user(user);
}

void Game::pause_agree(const UserPtr& user) {
    if (can_pause_agree(user)) {
        pause_proposer_.reset();
        state_ = PAUSE;
        pause_until_ = now() + pause_proposed_td();
        pause_limit_ -= pause_proposed_td();
    }
}

bool Game::can_pause_discard(const UserPtr& user) const {
    return state() == ACTIVE && is_pause_proposed() && is_member(user);
}

void Game::pause_discard(const UserPtr& user) {
    if (can_pause_discard(user)) {
        pause_proposer_.reset();
        pause_proposed_td_ = TD_NULL;
    }
}

void Game::take_vacation_pause(Td duration) {
    if (state() == ACTIVE) {
        if (competition_) {
            duration = std::min(duration, pause_limit_);
            pause_limit_ -= duration;
        }
        if (duration > TD_NULL) {
            state_ = PAUSE;
            pause_until_ = now() + duration;
            pause_proposed_td_ = duration;
        }
    } else if (state() == PAUSE && pause_until_ < now() + duration) {
        Td addition = now() + duration - pause_until_;
        if (competition_) {
            addition = std::min(addition, pause_limit_);
            pause_limit_ -= addition;
        }
        pause_until_ += addition;
        pause_proposed_td_ += addition;
    }
    Planning::instance()->add(new Object(GAME, id()), now(),
                              /* immediately */ false);
}

bool Game::can_mistake_propose(const UserPtr& user) const {
    return state() == ACTIVE &&
           size_without_init() > 0 &&
           !is_mistake_proposed() &&
           is_member(user);
}

bool Game::can_mistake_propose(const UserPtr& user, int mistake_move) const {
    return can_mistake_propose(user) &&
           mistake_move >= init_moves().size() && mistake_move < moves().size();
}

void Game::mistake_propose(const UserPtr& user, int mistake_move) {
    if (can_mistake_propose(user, mistake_move)) {
        mistake_move_ = mistake_move;
        mistake_proposer_ = user;
    }
}

bool Game::is_mistake_proposed() const {
    return mistake_proposer() && mistake_move() != -1;
}

bool Game::can_mistake_agree(const UserPtr& user) const {
    return can_mistake_discard(user) &&
           mistake_proposer() == other_user(user);
}

void Game::mistake_agree(const UserPtr& user) {
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
        pop_moves(moves().size() - mistake_move());
        mistake_move_ = -1;
    }
}

bool Game::can_mistake_discard(const UserPtr& user) const {
    return state() == ACTIVE && is_mistake_proposed() && is_member(user);
}

void Game::mistake_discard(const UserPtr& user) {
    if (can_mistake_discard(user)) {
        mistake_proposer_.reset();
        mistake_move_ = -1;
    }
}

bool Game::can_draw_propose(const UserPtr& user) const {
    return state() == ACTIVE &&
           meet_first_draw() &&
           !is_draw_proposed() &&
           is_member(user);
}

void Game::draw_propose(const UserPtr& user) {
    if (can_draw_propose(user)) {
        draw_proposer_ = user;
    }
}

bool Game::can_draw_agree(const UserPtr& user) const {
    return can_draw_discard(user) && meet_first_draw();
}

void Game::draw_agree(const UserPtr& user) {
    if (can_draw_agree(user)) {
        draw_proposer_.reset();
        finish(DRAW_AGREED);
    }
}

bool Game::can_draw_discard(const UserPtr& user) const {
    return state() == ACTIVE &&
           is_draw_proposed() &&
           is_member(user) &&
           draw_proposer() == other_user(user);
}

void Game::draw_discard(const UserPtr& user) {
    if (can_draw_discard(user)) {
        draw_proposer_.reset();
    }
}

bool Game::can_surrender(const UserPtr& user) const {
    return state() == ACTIVE &&
           size_without_init() >= gp_->first_draw() &&
           is_member(user);
}

void Game::surrender(const UserPtr& user) {
    if (can_surrender(user)) {
        finish(SURRENDERED, other_user(user));
    }
}

bool Game::can_check_auto_draws(const UserPtr& user) const {
    return state() == ACTIVE && is_member(user) && meet_first_draw();
}

void Game::check_auto_draws(const UserPtr& user) {
    if (can_check_auto_draws(user)) {
        CachedMoves cm(moves());
        if (cm.is_draw_3()) {
            finish(DRAW_3);
        } else if (cm.is_draw_50()) {
            finish(DRAW_50);
        }
    }
}

Piece::Color Game::order_color() const {
    return (moves().size() % 2) ? Piece::BLACK : Piece::WHITE;
}

UserPtr Game::order_user() const {
    UserPtr result;
    if (order_color() == Piece::WHITE) {
        result = white();
    }
    if (order_color() == Piece::BLACK) {
        result = black();
    }
    return result;
}

const Moves& Game::init_moves() const {
    return gp_->moves();
}

int Game::size_without_init() const {
    return size() - init_moves().size();
}

bool Game::meet_first_draw() const {
    return size_without_init() >= gp_->first_draw() &&
           gp_->first_draw() != NO_DRAW;
}

bool Game::real_rating() const {
    return !gp_->norating() && meet_first_draw() && !norating();
}

bool Game::can_mark_norating(const UserPtr& user) const {
    return user && user->has_permission(VIRTUALS_VIEWER) && !is_member(user) &&
           is_ended() && real_rating();
}

void Game::mark_norating(const UserPtr& user) {
    if (can_mark_norating(user)) {
        set_norating(true);
        admin_log("Mark norating " + game_a(id()));
    }
}

void Game::finish(State state, const UserPtr& winner) {
    state_ = state;
    if (winner) {
        winner_ = winner;
    }
    ended_ = now();
    stat_change();
}

void Game::push_move(HalfMove half_move) {
    moves_.push(half_move);
}

void Game::pop_moves(int number) {
    moves_.pop(number);
}

int Game::rating_after(Piece::Color color) const {
    return (color == Piece::COLOR_NULL) ? -1 : rating_after_[color];
}

const Wt::WString& Game::comment() const {
    return name();
}

bool Game::can_comment(const UserPtr& user) const {
    return is_member(user);
}

void Game::set_comment(const UserPtr& user, const Wt::WString& t) {
    if (can_comment(user)) {
        set_name(t);
    }
}

bool Game::has_comment_base() const {
    return comment_base_;
}

const CommentPtr& Game::comment_base() {
    if (!comment_base_) {
        comment_base_ = session()->add(new Comment(true));
        comment_base_.modify()->set_type(Comment::CHAT_ROOT);
        comment_base_.flush();
    }
    return comment_base_;
}

const char* Game::pgn_termination() const {
    if (!is_ended()) {
        return "unterminated";
    } else if (state_ == TIMEOUT) {
        return "time forfeit";
    } else if (state_ == DRAW_50 || state_ == DRAW_3 ||
               state_ == DRAW_2_KINGS) {
        return "adjudication";
    } else if (state_ == CANCELLED) {
        return "abandoned";    // ?? FIXME
    }
    return "normal";
}

void Game::pgn_init_moves(std::ostream& out) const {
    out << "[SetUp \"" << "1" << "\"]" << std::endl;
    int halfmove_clock = 0;
    Moves::const_iterator i = moves_.begin();
    Moves::const_iterator e = moves_.iter(init_moves().size());
    for (; i < e; ++i) {
        HalfMove half_move = *i;
        const Board& board = i.board();
        if (board.letter(half_move.from()) == Piece::PAWN ||
                board.test_takes(half_move)) {
            halfmove_clock = 0;
        } else {
            halfmove_clock += 1;
        }
    }
    ++i;
    int fullmove_number = Moves::moves_number(init_moves().size() + 1);
    out << "[FEN \"";
    const Board& board = i.board();
    board.fen(out, halfmove_clock, fullmove_number);
    out << "\"]" << std::endl;
}

void Game::pgn_additional(std::ostream& out) const {
    out << "[PlyCount \"" << size() << "\"]" << std::endl;
    long t1 = gp_->limit_private_init().total_seconds();
    long t2 = gp_->limit_std().total_seconds();
    out << "[TimeControl \"" << t1 << '/' << t2 << "\"]" << std::endl;
    if (started_.isValid()) {
        out << "[UTCTime \"" << started_.toString("HH:mm:ss") << "\"]";
        out << std::endl;
    }
    out << "[Termination \"" << pgn_termination() << "\"]" << std::endl;
    out << "[Mode \"" << "ICS" << "\"]" << std::endl;
    if (rating_after(Piece::WHITE) != -1) {
        out << "[WhiteElo \"" << rating_after(Piece::WHITE) << "\"]";
        out << std::endl;
        out << "[BlackElo \"" << rating_after(Piece::BLACK) << "\"]";
        out << std::endl;
    }
    if (init_moves().size()) {
        pgn_init_moves(out);
    }
}

// see http://cfajohnson.com/chess/SAN/SAN_DOC/Standard
void Game::pgn(std::ostream& out, bool reduced) const {
    std::string event = competition_ ? competition_->name().toUTF8() : "?";
    const std::string& site = Options::instance()->pgn_site();
    std::string date = started_.isValid() ?
                       started_.toString("yyyy.MM.dd").toUTF8() :
                       "????.??.??";
    int stage = competition_stage_ + 1;
    std::string round = competition_stage_ != -1 ?
                        boost::lexical_cast<std::string>(stage) : "-";
    std::string white = white_ ? white_->username20().toUTF8() : "?";
    std::string black = black_ ? black_->username20().toUTF8() : "?";
    boost::replace_all(white, "\"", "\\\"");
    boost::replace_all(black, "\"", "\\\"");
    std::string result = is_draw() ? "1/2-1/2" : !winner_ ? "*" :
                         winner_ == white_ ? "1-0" : "0-1";
    out << "[Event \"" << event << "\"]" << std::endl;
    out << "[Site \"" << site << "\"]" << std::endl;
    out << "[Date \"" << date << "\"]" << std::endl;
    out << "[Round \"" << round << "\"]" << std::endl;
    out << "[White \"" << white << "\"]" << std::endl;
    out << "[Black \"" << black << "\"]" << std::endl;
    out << "[Result \"" << result << "\"]" << std::endl;
    if (!reduced) {
        pgn_additional(out);
    }
    out << std::endl;
    moves_.pgn(out, result, reduced);
}

}

