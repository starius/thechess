/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_COMPETITION_HPP_
#define THECHESS_MODEL_COMPETITION_HPP_

#include <map>

#include "model/model.hpp"
#include "model/record.hpp"

namespace thechess {

/** 2D map: [white][black] => games vector */
typedef std::map<UserPtr, std::map<UserPtr, GamesVector> > GamesTable;

/** Model of competition.

\ingroup model
*/
class Competition : public dbo::Dbo<Competition>, public Record {
public:
    /** Type of competition */
    typedef CompetitionType Type;

    /** Map user to float number */
    typedef std::map<UserPtr, float> User2float;

    /** Map team to float number */
    typedef std::map<TeamPtr, float> Team2float;

    /** State of competition */
    enum State {
        RECRUITING = 10, /**< Recruiting, users are joining */
        ACTIVE = 30, /**< Active, game are played */
        ENDED = 50, /**< Ended, games are finished, winners are attributed */
        CANCELLED = 70 /**< Cancelled */
    };

    /** Default constructor.
    Should be used only by Wt::Dbo itself.
    */
    Competition();

    /** Create instance to be added to database */
    Competition(bool);

    /** Return string representation of the competition type */
    static Wt::WString type2str(Type type);

    /** Return if all specified games are ended */
    static bool all_ended(const GamesVector& games);

    /** Increase numbers of won game in map.
    \param games Games to consider
    \param wins Map (user to number of wins) to modify
    \note Draw is considered as 0.5 for white and 0.5 for black.
    */
    static void wins_number(const GamesVector& games, User2float& wins);

    /** Increase numbers of won game in map.
    \param user_wins Map (user to number of wins)
    \param u2t Map (user to team)
    \param team_wins Map (team to number of wins) to modify
    */
    static void team_wins_number(const User2float& user_wins,
                                 const User2Team& u2t,
                                 Team2float& team_wins);

    /** Return a vector of users obtained max number of wins */
    static UsersVector winners_of_games(const GamesVector& games);

    /** Return a vector of teams obtained max number of wins */
    TeamsVector winner_teams_of_games(const GamesVector& games);

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        Record::persist(a);
        dbo::belongsTo(a, cp_, "cp");
        dbo::belongsTo(a, gp_, "gp");
        dbo::field(a, state_, "state");
        dbo::hasMany(a, members_, dbo::ManyToMany, "members_competitions");
        dbo::hasMany(a, winners_, dbo::ManyToMany, "winners_competition");
        dbo::belongsTo(a, virtual_allower_, "virtual_allower");
        dbo::hasMany(a, games_, dbo::ManyToOne, "competition");
        dbo::field(a, started_, "started");
        dbo::field(a, ended_, "ended");
        dbo::field(a, comment_base_, "comment_base");
        dbo::hasMany(a, teams_, dbo::ManyToMany, "teams_competitions");
        dbo::hasMany(a, winner_teams_, dbo::ManyToMany,
                     "winner_teams_competitions");
    }

    /** Get competition parameters */
    const CPPtr& cp() const {
        return cp_;
    }

    /** Set competition parameters */
    void set_cp(const CPPtr& cp);

    /** Get game parameters.
    If self gp is defined, return self gp.
    If self gp is null, returns cp()->gp().
    */
    const GPPtr& gp() const;

    /** Get self game parameters.
    This may be a null pointer.
    */
    const GPPtr& self_gp() const {
        return gp_;
    }

    /** Set game parameters.
    By default, no game parametrs are set,
    which means "use default game parameters of the competition parameters".
    */
    void set_gp(const GPPtr& gp) {
        gp_ = gp;
    }

    /** Get type */
    Type type() const;

    /** Get state */
    State state() const {
        return state_;
    }

    /** Set state to RECRUITING if it is CANCELLED */
    void revive();

    /** Return string representation of the competition state */
    static const char* state2str(State state);

    /** \name Dates */
    /* @{ */

    /** Get datetime when competition was started */
    const Wt::WDateTime& started() const {
        return started_;
    }

    /** Get datetime when competition was ended */
    const Wt::WDateTime& ended() const {
        return ended_;
    }

    /* @} */

    /** \name Getters for users and games */
    /* @{ */

    /** Return if the user if a member of the competition */
    bool is_member(const UserPtr& user) const;

    /** Return if the user if a winner of the competition (or one of winners) */
    bool is_winner(const UserPtr& user) const;

    /** Get members collection */
    const Users& members() const {
        return members_;
    }

    /** Get games collection */
    const Games& games() const {
        return games_;
    }

    /** Get winners collection */
    const Users& winners() const {
        return winners_;
    }

    /** Get members vector */
    UsersVector members_vector() const;

    /** Get games vector */
    GamesVector games_vector() const;

    /** Get winners vector */
    UsersVector winners_vector() const;

    /** Calculate games table */
    GamesTable games_table() const;

    /** Return collection of games of the competition played by the user */
    Games games_with(const UserPtr& user) const;

    /** Return vector of games of the competition played by the user.
    \param user User.
    \param gt Table of games is used to avoid database requests.
    */
    GamesVector games_with(const UserPtr& user, GamesTable& gt) const;

    /** Return data matches between members of the competition */
    void virtuals_pairs(dbo::Query<BDPair>& pairs) const;

    /** Return if there are virtuals among competition members */
    bool has_virtuals() const;

    /** Return list of virtuals involved in this competition */
    UsersVector virtuals() const;

    /** Update competittions stats of members.
    This is called automaticaly when game is finished.
    If you call this manually, you should call this method of
    all the competitions, ordered by ended().
    */
    void stat_change() const;

    /* @} */

    /** \name Checking */
    /* @{ */

    /** Run self-checks for planned action.
    Possible changes caused by this method:
     - RECRUITING into ACTIVE, if min_users() and min_recruiting_time() are met
     - RECRUITING into CANCELLED, if time > max_recruiting_time()
     - ACTIVE into ENDED
    */
    void check(Wt::Wc::notify::TaskPtr task);

    /** Return the datetime of next expected check */
    Wt::WDateTime next_check() const;

    /* @} */

    /** \name Methods of recruiting state */
    /* @{ */

    /** Return if the user can create competition */
    static bool can_create_competition(const UserPtr& user);

    /** Set the user as competition creator.
    \note You should check user wuth can_create_competition by yourself
    */
    void create_competition(const UserPtr& user);

    /** Return if the user can join */
    bool can_join(const UserPtr& user) const;

    /** Try to join the user */
    void join(const UserPtr& user);

    /** Return if the user can join (team competition) */
    bool can_team_join(const UserPtr& user, const TeamPtr& team) const;

    /** Try to join the user (team competition) */
    void team_join(const UserPtr& user, const TeamPtr& team);

    /** Return if active user can add passive user to the team competition */
    bool can_add_user(const UserPtr& active, const UserPtr& passive,
                      const TeamPtr& team) const;

    /** Try by active user to add passive user to the team competition */
    void add_user(const UserPtr& active, const UserPtr& passive,
                  const TeamPtr& team);

    /** Return if the user can leave */
    bool can_leave(const UserPtr& user) const;

    /** Try to leave the user */
    void leave(const UserPtr& user);

    /** Return if kicker user can kick kicked user */
    bool can_kick(const UserPtr& kicker, const UserPtr& kicked) const;

    /** Try to kick kicked user by kicker user */
    void kick(const UserPtr& kicker, const UserPtr& kicked);

    /** Return if the user can change parameters */
    bool can_change_parameters(const UserPtr& user) const;

    /** Return if the user can cancel the competitions */
    bool can_cancel(const UserPtr& user) const;

    /** Try to cancel the competition by the user */
    void cancel(const UserPtr& user);

    /** Return if the user can force start the competition */
    bool can_force_start(const UserPtr& user) const;

    /** Try to force start the competition */
    void force_start(const UserPtr& user);

    /** Return if the user can allow virtuals in the competitions */
    bool can_allow_virtuals(const UserPtr& user) const;

    /** Try to allow virtuals in the competitions by the user */
    void allow_virtuals(const UserPtr& user);

    /** Return the user allowed virtuals */
    const UserPtr& virtual_allower() const {
        return virtual_allower_;
    }

    /** Return if games in pairs of these two players can be exchanged.
    Only for PAIR_TEAM competition in Competition::ACTIVE state.
    Only for PROPOSED games.
    Users must be in one team.
    */
    bool can_exchange(const UserPtr& a, UserPtr& b) const;

    /** Return if the user can do exchange */
    bool can_exchange(const UserPtr& user) const;

    /** Try to exchange games in pairs of these two players */
    void exchange(const UserPtr& a, UserPtr& b);

    /* @} */

    /** \name Methods of comments */
    /* @{ */

    /** Return if there is a comment base  */
    bool has_comment_base() const;

    /** Return comment base.
    Lazy created.
    */
    const CommentPtr& comment_base();

    /** Get comment base (may return 0) */
    const CommentPtr& comment_base() const {
        return comment_base_;
    }

    /* @} */

    /** \name Methods of teams */
    /* @{ */

    /** Get teams collection */
    const Teams& teams() const {
        return teams_;
    }

    /** Get winner teams collection */
    const Teams& winner_teams() const {
        return winner_teams_;
    }

    /** If the user can add the team to the competition */
    bool can_add_team(const UserPtr& user, const TeamPtr& team) const;

    /** Try to add the team to the competition */
    void add_team(const UserPtr& user, const TeamPtr& team);

    /** If the user can remove the team from the competition */
    bool can_remove_team(const UserPtr& user, const TeamPtr& team) const;

    /** Try to remove the team from the competition */
    void remove_team(const UserPtr& user, const TeamPtr& team);

    /* @} */

private:
    // common attributes
    CPPtr cp_;
    GPPtr gp_;
    State state_;

    Users members_;
    Users winners_;

    UserPtr virtual_allower_;

    Games games_;

    Wt::WDateTime started_;
    Wt::WDateTime ended_;

    CommentPtr comment_base_;

    Teams teams_;
    Teams winner_teams_;

    bool can_start() const;
    void start();
    void create_games_classical();
    int calculate_white_games_per_team() const;
    void create_games_team();
    void create_games_pair_team();
    void cancel_impl();

    void process();
    void process_classical();

    void finish(const UsersVector& winners);
    void finish(const TeamsVector& winners);
    void finish();

    GamePtr create_game(const UserPtr& white, const UserPtr& black,
                        int stage = -1, bool no_draw = false);

    bool can_join_common(const UserPtr& user) const;
    void remove_user(const UserPtr& user);

    friend class StagedCompetition;
};

}

DBO_EXTERN_TEMPLATES(thechess::Competition);

#endif

