/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_GAME_H_
#define THECHESS_MODEL_GAME_H_

#include <string>
#include <vector>

#include <Wt/WObject>
#include <Wt/WString>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;
#include <Wt/WDateTime>
#include <Wt/Dbo/WtSqlTraits>

namespace thechess {
class Game;
typedef dbo::ptr<Game> GamePtr;
typedef dbo::collection<GamePtr> Games;
typedef std::vector<GamePtr> GamesVector;
}

#include "model/GameParameters.hpp"
#include "model/User.hpp"
#include "model/Competition.hpp"
#include "model/td.hpp"
#include "model/Object.hpp"

namespace thechess {

/** \brief Dbo-model that represents one game beetween two users.

How to add new game to database:
\code
GamePtr game = session.add(new Game(true));
\endcode

*/

class Game : public GameParameters, public dbo::Dbo<Game> {
public:
    enum State {
        PROPOSED = 0, /**< was proposed by user or planned by competition */
        CONFIRMED = 10, /**< is to be started when both users are online */
        ACTIVE = 20, /**< is running, clock is ticking */
        PAUSE = 30, /**< is paused until predefined time, clock is also paused */
#ifndef DOXYGEN_ONLY
        MIN_ENDED = 50,
#endif
        DRAW_STALEMATE = 50, /**< ended with stalemate */
        DRAW_AGREED = 51, /**< ended with draw by agreement */
        DRAW_50 = 52, /**< ended because of 50 without pawn or takes */
        DRAW_3 = 53, /**< ended because same position repeated thrice */
        DRAW_2_KINGS = 54, /**< ended with low material draw */
        SURRENDERED = 61, /**< ended because one of users surrendered */
        TIMEOUT = 62, /**< ended because of timeout */
        CANCELLED = 63, /**< ended because user cancelled it */
        MATE = 64, /**< ended with checkmate */
        NO_DRAW_STALEMATE = 65 /**< ended with stalemate, black wins */
    };

#ifndef DOXYGEN_ONLY
    Game();
#endif

    /** \name Creation of game */
    /* @{ */

    /** Create new game to be added to database. */
    Game(bool);

#ifndef DOXYGEN_ONLY
    template<class Action>
    void persist(Action& a) {
        GameParameters::persist(a);
        dbo::field(a, state_, "state");
        dbo::belongsTo(a, white_, "white");
        dbo::belongsTo(a, black_, "black");
        dbo::belongsTo(a, winner_, "winner_game");
        dbo::belongsTo(a, init_, "init_game");
        dbo::field(a, competition_confirmer_[WHITE], "competition_confirmer_white");
        dbo::field(a, competition_confirmer_[BLACK], "competition_confirmer_black");
        dbo::field(a, colors_random_, "colors_random");
        dbo::field(a, created_, "created");
        dbo::field(a, confirmed_, "confirmed");
        dbo::field(a, started_, "started");
        dbo::field(a, lastmove_, "lastmove");
        dbo::field(a, ended_, "ended");
        dbo::field(a, limit_private_[WHITE], "limit_private_white");
        dbo::field(a, limit_private_[BLACK], "limit_private_black");
        dbo::belongsTo(a, competition_, "competition");
        dbo::field(a, competition_stage_, "competition_stage");
        dbo::field(a, pause_until_, "pause_until");
        dbo::field(a, pause_limit_, "pause_limit");
        dbo::field(a, pause_proposed_td_, "pause_proposed_td");
        dbo::field(a, pause_proposer_, "pause_proposer");
        dbo::field(a, mistake_move_, "mistake_move");
        dbo::field(a, mistake_proposer_, "mistake_proposer");
        dbo::field(a, draw_proposer_, "draw_proposer");
        dbo::field(a, rating_after_[WHITE], "rating_after_white");
        dbo::field(a, rating_after_[BLACK], "rating_after_black");
        dbo::field(a, comment_, "comment");
    }
#endif

    /** Turn this game into game proposed by one user to another.
    \param init the creator of game
    \param u user to who game is proposed
    \param c color of creator, or none color to set random colors
    */
    void propose_game(UserPtr init, UserPtr u,
                      Color c);

    /** Turn this game into challenge by one user.
    \param init the creator of game
    \param c color of creator, none color meens random color
    */
    void propose_challenge(UserPtr init, Color c);

    /** Turn this game into competition game.
    \param white the user playing white pieces
    \param black the user playing black pieces
    \param competition the parent competition
    \param competition_stage stage of competition or \c -1 if not defined
    \param random if colors of pieces will be randomized
    */
    void make_competition_game(UserPtr white, UserPtr black,
                               CompetitionPtr competition, int competition_stage = -1, bool random = false);

    /* @} */

    /** \name Checking */
    /* @{ */

    /** Run self-checks for planned action.
    \param objects collection to expand with other modified dbo \ref Object.

    Possible changes caused by this method:
     - \ref PROPOSED into \ref CONFIRMED, if \ref relax_time is out
     - \ref CONFIRMED into \ref ACTIVE, if both users are online
     - \ref CONFIRMED into \ref ACTIVE, if \ref FORCE_START_DELAY is out
     - \ref paused into \ref ACTIVE, if current time is after \ref pause_until
     - \ref ACTIVE into \ref TIMEOUT, if time is over

    In the latter case, if the game is attributed to the competition,
    the competition is added to \p objects.
    */
    void check(Objects& objects);

    /** Return the datetime of next expected check */
    Wt::WDateTime next_check() const;

    /* @} */

    /** \name Methods of pre-started state */
    /* @{ */

    /** Return if the game is a challenge */
    bool is_challenge() const;

    /** Return if the game is a creation */
    bool is_creation() const;

    /** Return if the user can join the game.
    Non-anonymous user can join to challenge made by other user.
    */
    bool can_join(UserPtr user) const;

    /** Try to join the user.
    On success the game changes its state to \ref CONFIRMED
    */
    void join(UserPtr user);

    /** Return if user can confirm the game.
    Non-init member of creation can confirm it
    */
    bool can_confirm(UserPtr user) const;

    /** The user tries to confirm the game.
    On success the game changes its state to \ref CONFIRMED
    */
    void confirm(UserPtr user);

    /** Return if user can cancel the game.
    Non-init member of creation can confirm it
    */
    bool can_cancel(UserPtr user) const;

    /** The user tries to cancel the game.
    On success the game changes its state to \ref CANCELLED
    */
    void cancel(UserPtr user);

    /* @} */

    /** \name Pause management */
    /* @{ */

    /** Return if user can propose a pause */
    bool can_pause_propose(const UserPtr user) const;

    /** Return if user can propose the pause for specified time */
    bool can_pause_propose(const UserPtr user, const Td& td) const;

    /** The user tries to propose the pause */
    void pause_propose(const UserPtr user, const Td& td);

    /** Return if the pause has been proposed */
    bool is_pause_proposed() const;

    /** Return the user proposed the pause */
    const UserPtr pause_proposer() const {
        return pause_proposer_;
    }

    /** Return unused time that can be used for paused */
    Td pause_limit() const {
        return pause_limit_;
    }

    /** Return duration of the proposed pause */
    Td pause_proposed_td() const {
        return pause_proposed_td_;
    }

    /** Return datetime when pause will be finished */
    const Wt::WDateTime& pause_until() const {
        return pause_until_;
    }

    /** Return datetime when pause has been started */
    Wt::WDateTime pause_started() const;

    /** Return if the user can agree with proposed pause */
    bool can_pause_agree(const UserPtr user) const;

    /** The user tries to agree with proposed pause */
    void pause_agree(const UserPtr user);

    /** Return if the user can discard proposed pause */
    bool can_pause_discard(const UserPtr user) const;

    /** The user tries to discard proposed pause */
    void pause_discard(const UserPtr user);

    /* @} */

    /** \name Mistake and rollback management */
    /* @{ */

    /** Return if the user can propose to rollback mistake */
    bool can_mistake_propose(const UserPtr user) const;

    /** Return if the user can propose to rollback mistake at given move */
    bool can_mistake_propose(const UserPtr user, int mistake_move) const;

    /** The user tries to propose to rollback mistake */
    void mistake_propose(const UserPtr user, int mistake_move);

    /** Return if the mistake is proposed to be rollbacked */
    bool is_mistake_proposed() const;

    /** Return the user proposed to rollback the mistake */
    const UserPtr mistake_proposer() const {
        return mistake_proposer_;
    }

    /** Return the move which is proposed to be rollbacked*/
    int mistake_move() const {
        return mistake_move_;
    }

    /** Return if the user can agree to rollback the mistake */
    bool can_mistake_agree(const UserPtr user) const;

    /** The user tries to agree to rollback mistake */
    void mistake_agree(const UserPtr user);

    /** Return if the user can discard rollback the mistake */
    bool can_mistake_discard(const UserPtr user) const;

    /** The user tries to discard rollback mistake */
    void mistake_discard(const UserPtr user);

    /* @} */

    /** \name Draw by agreement */
    /* @{ */

    /** Return if the user can propose a draw by agreement */
    bool can_draw_propose(const UserPtr user) const;

    /** The user tries to propose a draw by agreement */
    void draw_propose(const UserPtr user);

    /** Return the user proposed draw by agreement */
    const UserPtr draw_proposer() const {
        return draw_proposer_;
    }

    /** Return if the user has proposed the draw by agreement */
    bool is_draw_proposed() const {
        return draw_proposer_;
    }

    /** Return if the user can agree with the draw by agreement */
    bool can_draw_agree(const UserPtr user) const;

    /** The user tries to agree with draw by agreement */
    void draw_agree(const UserPtr user);

    /** Return if the user can discard the draw by agreement */
    bool can_draw_discard(const UserPtr user) const;

    /** The user tries to discard the draw by agreement */
    void draw_discard(const UserPtr user);

    /* @} */

    /** \name Information about members */
    /* @{ */

    /** Return the active color */
    Color order_color() const;

    /** Return the active user */
    UserPtr order_user() const;

    /** Return the user created the game */
    UserPtr init() const {
        return init_;
    }

    /** Return the user playing white pieces */
    UserPtr white() const {
        return white_;
    }

    /** Return the user playing black pieces */
    UserPtr black() const {
        return black_;
    }

    /** Return the winner */
    UserPtr winner() const {
        return winner_;
    }

    /** Return the pieces color of given user */
    Color color_of(const UserPtr user) const;

    /** Return if the user is member of the game (black, white or init) */
    bool is_member(const UserPtr user) const;

    /** Return the user playing pieces of given color */
    UserPtr user_of(Color color) const;

    /** Return another member of the game */
    UserPtr other_user(const UserPtr user) const;

    /* @} */

    /** \name Information about game */
    /* @{ */

    /** Return the number of half-moves of the game */
    int size() const {
        return moves_.size();
    }

    /** Return the number of non-predetermined half-moves */
    int size_without_init() const;

    /** Return if the game reached size, required for draw by agreement */
    bool meet_first_draw() const;

    /** Return if the game is affecting rating of users */
    bool real_rating() const;

    /** Return the number of moves of the game */
    int human_size() const {
        return Moves::size_to_human(size());
    }

    /** Return if the game is ended, ie is a win, draw or cancelled */
    bool is_ended() const;

    /** Return if the game is a draw */
    bool is_draw() const;

    /** Return if the game is a win */
    bool is_win() const;

    /** Return string representation of state */
    Wt::WString str_state() const;

    /** Return state */
    State state() const {
        return state_;
    }

    /** Return if pieces colors were distributed randomly */
    bool colors_random() const {
        return colors_random_;
    }

    /** Return elo rating of the user after the game if changed or -1 */
    int rating_after(Color color) const;

    /* @} */

    /** \name Making a move */
    /* @{ */

    /** Return if given user can add a move to this game */
    bool can_move(UserPtr user) const;

    /** Add move to game.
    Method was added to avoid additional checking of move (optimization).
    You must check move yourself before call this method!
    */
    void add_move(const Move& move,
                  const Board& board_after);

    /* @} */

    /** \name Datetimes and time durations */
    /* @{ */

    /** Return private time limit of user of given color */
    Td limit_private(Color color) const;

    /** Return private time limit of given user */
    Td limit_private(UserPtr user) const;

    /** Return private time limit of active user */
    Td limit_private() const {
        return limit_private(order_color());
    }

    /** Return datetime when game was created */
    const Wt::WDateTime& created() const {
        return created_;
    }

    /** Return datetime when game was confirmed */
    const Wt::WDateTime& confirmed() const {
        return confirmed_;
    }

    /** Return datetime when game was started */
    const Wt::WDateTime& started() const {
        return started_;
    }

    /** Return (virtual) datetime of last move.
    If the game was paused, this datetime is modified to reflect pause
    */
    const Wt::WDateTime& lastmove() const {
        return lastmove_;
    }

    /** Return datetime when game was started */
    const Wt::WDateTime& ended() const {
        return ended_;
    }

    /** Return td that would be subtracted from limit of the active user */
    Td spent_time() const;

    /** Return td that would be subtracted from limit of the given user */
    Td spent_time(UserPtr user) const;

    /** Return sum of limits of given user */
    Td total_limit(UserPtr user) const;

    /** Return sum of limits of given user taking spent time into account */
    Td total_limit_now(UserPtr user) const;

    /** Return private time limit taking spent time into account */
    Td limit_private_now(UserPtr user) const;

    /** Return standard time limit taking spent time into account */
    Td limit_std_now(UserPtr user) const;

    /* @} */

    /** \name Comment-related methods */
    /* @{ */

    /** Return text of comment of this game by members of game */
    const Wt::WString& comment() const {
        return comment_;
    }

    /** Return if the user can change the comment of the game */
    bool can_comment(const UserPtr user) const;

    /** The user tries to set the comment */
    void set_comment(const UserPtr user, const Wt::WString& t);

    /* @} */

    /** \name Competition-related methods */
    /* @{ */

    /** Return the competition to which the game is attributed */
    CompetitionPtr competition() const {
        return competition_;
    }

    /** Return stage of the competition if attributed else -1 */
    int competition_stage() const {
        return competition_stage_;
    }

    /** Return if user has confimed the competition game */
    bool has_competition_confirmed(UserPtr user) const;

    /** Return if user can confime the competition game */
    bool can_competition_confirm(UserPtr user) const;

    /** The user tries to confirm the game */
    void competition_confirm(UserPtr user);

    /** Return if user can discard previously confimed competition game */
    bool can_competition_discard(UserPtr user) const;

    /** The user tries to discard previously confirmed competition game */
    void competition_discard(UserPtr user);

    /** Method should be called only from competition processing code.
    On success the game changes its state to \ref CONFIRMED
    */
    void confirm_by_competition();

    /* @} */

    /** Write PGN representation of game to stream.
    \param reduced whether reduced export PGN format is used
    \sa http://cfajohnson.com/chess/SAN/SAN_DOC/Standard
    \sa http://www.chessclub.com/help/PGN-spec
    */
    void pgn(std::ostream& out, bool reduced = false) const;

    /** Convert given state to i18n id */
    static const char* state2str_id(State state);

private:
    State state_;

    UserPtr white_;
    UserPtr black_;
    UserPtr winner_;

    UserPtr init_;
    bool competition_confirmer_[2]; // used with competition games only
    bool colors_random_;

    Wt::WDateTime created_;
    Wt::WDateTime confirmed_;
    Wt::WDateTime started_;
    Wt::WDateTime lastmove_;
    Wt::WDateTime ended_;

    Td limit_private_[2];

    CompetitionPtr competition_;
    int competition_stage_;

    Wt::WDateTime pause_until_;
    Td pause_limit_;
    Td pause_proposed_td_;
    UserPtr pause_proposer_;

    int mistake_move_; // -1 => no mistake proposed
    UserPtr mistake_proposer_;

    UserPtr draw_proposer_;

    int rating_after_[2];

    Wt::WString comment_;

    void set_white_(UserPtr user) {
        white_ = user;
    }
    void set_black_(UserPtr user) {
        black_ = user;
    }
    void set_of_color_(UserPtr user, Color color);
    void set_random_(UserPtr user1, UserPtr user2);

    void confirm_();
    void start_();
    void stop_pause_();
    void finish_(State state, UserPtr winner = UserPtr());
    void elo_change_();

    void push_move_(Move move);
    void pop_moves_(int number);

    const char* pgn_termination_() const;
    void pgn_init_moves_(std::ostream& out) const;
    void pgn_additional_(std::ostream& out) const;
};

}

#endif

