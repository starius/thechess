#ifndef THECHESS_MODEL_GAME_H_
#define THECHESS_MODEL_GAME_H_

#include <string>
#include <boost/optional.hpp>

#include <Wt/WObject>
#include <Wt/WString>
#include <Wt/WSignal>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;
#include <Wt/WDateTime>
#include <Wt/Dbo/WtSqlTraits>

namespace thechess {
namespace model {
    class Game;
    typedef dbo::ptr<Game> GamePtr;
    typedef dbo::collection<GamePtr> Games;
}
}

#include "model/GameParameters.hpp"
#include "model/User.hpp"
#include "model/Competition.hpp"
#include "model/td.hpp"

namespace thechess {
namespace model {

class Game : public GameParameters, public dbo::Dbo<Game>
{
public:

    enum Event {
        e_confirm,
        e_start,
        e_move,
        e_pause, // any pause event
        e_mistake, // any mistake event
        e_draw, // any draw event
        e_end,
        e_comment
    };

    typedef Wt::Signal<Event, chess::Move> GameSignal;

    enum State {
        proposed = 0,
        confirmed = 10,
        active = 20,
        pause = 30,
        min_ended = 50,
        draw_stalemate = 50,
        draw_agreed = 51,
        draw_50 = 52,
        draw_3 = 53,
        draw_2_kings = 54,
        surrendered = 61,
        timeout = 62,
        cancelled = 63,
        mate = 64
    };

    template<class Action>
    void persist(Action& a)
    {
        GameParameters::persist(a);
        dbo::field(a, state_, "state");
        dbo::belongsTo(a, white_, "white");
        dbo::belongsTo(a, black_, "black");
        dbo::belongsTo(a, winner_, "winner_game");
        dbo::belongsTo(a, init_, "init_game");
        dbo::field(a, colors_random_, "colors_random");
        dbo::field(a, created_, "created");
        dbo::field(a, confirmed_, "confirmed");
        dbo::field(a, started_, "started");
        dbo::field(a, lastmove_, "lastmove");
        dbo::field(a, ended_, "ended");
        dbo::field(a, limit_private_[0], "limit_private_white");
        dbo::field(a, limit_private_[1], "limit_private_black");
        dbo::belongsTo(a, competition_, "competition");
        dbo::field(a, pause_until_, "pause_until");
        dbo::field(a, pause_limit_, "pause_limit");
        dbo::field(a, pause_proposed_td_, "pause_proposed_td");
        dbo::field(a, pause_proposer_, "pause_proposer");
        dbo::field(a, mistake_move_, "mistake_move");
        dbo::field(a, mistake_proposer_, "mistake_proposer");
        dbo::field(a, draw_proposer_, "draw_proposer");
        dbo::field(a, moves_size_, "moves_size");
        dbo::field(a, rating_after_[0], "rating_after_white");
        dbo::field(a, rating_after_[1], "rating_after_black");
        dbo::field(a, comment_, "comment");
    }

    GameSignal& signal() const;

    void initialize();
    void create_game(UserPtr init, UserPtr u,
        chess::Color c);
    void create_challenge(UserPtr init, chess::Color c);

    void check();
    void check_impl_();

    bool is_challenge() const;
    bool is_creation() const;
    bool can_join(UserPtr user) const;
    void join(UserPtr user);
    bool can_confirm(UserPtr user) const;
    void confirm(UserPtr user);
    void confirm();
    void start();
    bool can_cancel(UserPtr user) const;
    void cancel(UserPtr user);

    bool can_pause_propose(const UserPtr user) const;
    bool can_pause_propose(const UserPtr user, const Td& td) const;
    void pause_propose(const UserPtr user, const Td& td);
    bool is_pause_proposed() const;
    const UserPtr pause_proposer() const { return pause_proposer_; }
    Td pause_limit() const { return pause_limit_; }
    Td pause_proposed_td() const { return pause_proposed_td_; }
    const Wt::WDateTime& pause_until() const { return pause_until_; }
    Wt::WDateTime pause_started() const;
    bool can_pause_agree(const UserPtr user) const;
    void pause_agree(const UserPtr user);
    bool can_pause_discard(const UserPtr user) const;
    void pause_discard(const UserPtr user);

    bool can_mistake_propose(const UserPtr user) const;
    bool can_mistake_propose(const UserPtr user, int mistake_move) const;
    void mistake_propose(const UserPtr user, int mistake_move);
    bool is_mistake_proposed() const;
    const UserPtr mistake_proposer() const { return mistake_proposer_;}
    int mistake_move() const { return mistake_move_; }
    bool can_mistake_agree(const UserPtr user) const;
    void mistake_agree(const UserPtr user);
    bool can_mistake_discard(const UserPtr user) const;
    void mistake_discard(const UserPtr user);

    bool can_draw_propose(const UserPtr user) const;
    void draw_propose(const UserPtr user);
    const UserPtr draw_proposer() const { return draw_proposer_;}
    bool is_draw_proposed() const { return draw_proposer_; }
    bool can_draw_agree(const UserPtr user) const;
    void draw_agree(const UserPtr user);
    bool can_draw_discard(const UserPtr user) const;
    void draw_discard(const UserPtr user);

    chess::Color order_color() const;
    UserPtr order_user() const;

    UserPtr init() const { return init_; }
    UserPtr white() const { return white_; }
    UserPtr black() const { return black_; }
    UserPtr winner() const { return winner_; }
    chess::Color color_of(const UserPtr user) const;
    bool is_member(const UserPtr user) const;
    UserPtr user_of(chess::Color color) const;
    UserPtr other_user(const UserPtr user) const;

    int size() const { return moves_.size(); }
    int size_without_init() const;
    bool meet_first_draw() const;
    bool real_rating() const;
    int human_size() const { return chess::Moves::size_to_human(moves_size_); }

    bool can_move(UserPtr user) const;

    bool is_ended() const;
    bool is_draw() const;
    bool is_win() const;
    static const char* state2str_id(State state);
    Wt::WString str_state() const;
    State state() const { return state_; }
    Td limit_private(chess::Color color) const;
    Td limit_private(UserPtr user) const;
    Td limit_private() const { return limit_private(order_color()); }
    const Wt::WDateTime& created() const { return created_; }
    const Wt::WDateTime& when_confirmed() const { return confirmed_; }
    const Wt::WDateTime& started() const { return started_; }
    const Wt::WDateTime& lastmove() const { return lastmove_; }
    const Wt::WDateTime& ended() const { return ended_; }

    Td spent_time() const;
    Td spent_time(UserPtr user) const;
    Td total_limit(UserPtr user) const;
    Td total_limit_now(UserPtr user) const;
    Td limit_private_now(UserPtr user) const;
    Td limit_std_now(UserPtr user) const;
    Wt::WDateTime next_check() const;

    bool colors_random() const { return colors_random_; }

    void add_move(const chess::Move& move,
        const chess::Board& board_after);

    int rating_after(chess::Color color) const;

    const Wt::WString& comment() const { return comment_; }
    bool can_comment(const UserPtr user) const;
    void set_comment(const UserPtr user, const Wt::WString& t);

    int competition_round() const { return competition_round_; }
    void set_competition_round(int v) { competition_round_ = v; }

private:
    State state_;

    UserPtr white_;
    UserPtr black_;
    UserPtr winner_;

    UserPtr init_;
    bool colors_random_;

    Wt::WDateTime created_;
    Wt::WDateTime confirmed_;
    Wt::WDateTime started_;
    Wt::WDateTime lastmove_;
    Wt::WDateTime ended_;

    Td limit_private_[2];

    CompetitionPtr competition_;
    int competition_round_;

    Wt::WDateTime pause_until_;
    Td pause_limit_;
    Td pause_proposed_td_;
    UserPtr pause_proposer_;

    int mistake_move_; // -1 => no mistake proposed
    UserPtr mistake_proposer_;

    UserPtr draw_proposer_;

    int moves_size_;
    int rating_after_[2];

    Wt::WString comment_;

    void set_white_(UserPtr user) { white_ = user; }
    void set_black_(UserPtr user) { black_ = user; }
    void set_of_color_(UserPtr user, chess::Color color);
    void set_random_(UserPtr user1, UserPtr user2);

    void signal_emit(Event event, const chess::Move& move);
    void signal_emit(Event event);

    void finish_(State state, UserPtr winner=UserPtr());
    void elo_change_();

    void push_move_(chess::Move move);
    void pop_moves_(int number);
};

}
}

#endif // THECHESS_MODEL_GAME_H_
