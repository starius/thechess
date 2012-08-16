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
    static void wins_number(const GamesVector& games,
                            std::map<UserPtr, float>& wins);

    /** Return a vector of users obtained max number of wins */
    static UsersVector winners_of_games(const GamesVector& games);

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        Record::persist(a);
        dbo::belongsTo(a, cp_, "cp");
        dbo::belongsTo(a, gp_, "gp");
        dbo::field(a, state_, "state");
        dbo::hasMany(a, members_, dbo::ManyToMany, "members_competitions");
        dbo::belongsTo(a, virtual_allower_, "virtual_allower");
        dbo::hasMany(a, games_, dbo::ManyToOne, "competition");
        dbo::field(a, started_, "started");
        dbo::field(a, ended_, "ended");
        dbo::hasMany(a, winners_, dbo::ManyToMany, "winners_competition");
    }

    /** Get competition parameters */
    const CPPtr& cp() const {
        return cp_;
    }

    /** Set competition parameters */
    void set_cp(const CPPtr& cp) {
        cp_ = cp;
    }

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

    /** Return if there are virtuals among competition members */
    bool has_virtuals() const;

    /* @} */

    /** \name Checking */
    /* @{ */

    /** Run self-checks for planned action.
    Possible changes caused by this method:
     - RECRUITING into ACTIVE, if min_users() and min_recruiting_time() are met
     - RECRUITING into CANCELLED, if time > max_recruiting_time()
     - ACTIVE into ENDED
    */
    void check(Wt::Wc::notify::TaskPtr task, Planning* planning);

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

    /** Return if the user can allow virtuals in the competitions */
    bool can_allow_virtuals(const UserPtr& user) const;

    /** Try to allow virtuals in the competitions by the user */
    void allow_virtuals(const UserPtr& user);

    /** Return the user allowed virtuals */
    const UserPtr& virtual_allower() const {
        return virtual_allower_;
    }

    /* @} */

private:
    // common attributes
    CPPtr cp_;
    GPPtr gp_;
    State state_;

    Users members_;

    UserPtr virtual_allower_;

    Games games_;

    Wt::WDateTime started_;
    Wt::WDateTime ended_;

    Users winners_;

    bool can_start() const;
    void start(Planning* planning);
    void create_games_classical_(Planning* planning);
    void cancel();

    void process_(Planning* planning);
    void process_classical_(Planning* planning);

    void finish_(const UsersVector& winners, Planning* planning);

    GamePtr create_game_(const UserPtr& white, const UserPtr& black,
                         int stage = -1, bool no_draw = false);

    friend class StagedCompetition;
};

}

DBO_EXTERN_TEMPLATES(thechess::Competition);

#endif

