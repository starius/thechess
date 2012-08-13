/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_USER_H_
#define THECHESS_MODEL_USER_H_

#include <boost/utility/binary.hpp>

#include <Wt/Dbo/Query>

#include "model/model.hpp"

namespace thechess {

/** Model of a user.
Each user registered on site is represented with instance of this model.

\ingroup model
*/
class User : public dbo::Dbo<User> {
public:
    /** Rights */
    enum Rights {
        NONE = 0,
        COMMENTS_REMOVER = BOOST_BINARY(00000001),
        CLASSIFICATION_CONFIRMER = BOOST_BINARY(00000010),
        COMPETITION_CHANGER = BOOST_BINARY(00000100),
        RIGHTS_CHANGER = BOOST_BINARY(01000000 00000000 00000000 00000000),
        MODERATOR = COMMENTS_REMOVER |
                    CLASSIFICATION_CONFIRMER |
                    COMPETITION_CHANGER,
        ADMIN = MODERATOR | RIGHTS_CHANGER,
        REGULAR_USER = NONE
    };

    /** Default constructor.
    Should be used only by Wt::Dbo itself.
    */
    User();

    /** Create instance to be added to database */
    User(const Wt::WString& username);

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        dbo::field(a, username_, "username");
        dbo::field(a, rights_, "rights");
        dbo::field(a, sessions_, "sessions");
        dbo::field(a, last_enter_, "last_enter");
        dbo::field(a, online_time_, "online_time");
        dbo::field(a, classification_, "classification");
        dbo::belongsTo(a, classification_confirmer_,
                       "classification_confirmer");
        dbo::hasMany(a, classification_confirmed_,
                     dbo::ManyToOne, "classification_confirmer");
        dbo::hasMany(a, white_games_, dbo::ManyToOne, "white");
        dbo::hasMany(a, black_games_, dbo::ManyToOne, "black");
        dbo::hasMany(a, won_games_, dbo::ManyToOne, "winner_game");
        dbo::hasMany(a, init_games_, dbo::ManyToOne, "init");
        dbo::hasMany(a, competitions_, dbo::ManyToMany, "members_competitions");
        dbo::hasMany(a, init_competitions_, dbo::ManyToOne, "init");
        dbo::hasMany(a, virtual_allower_, dbo::ManyToOne, "virtual_allower");
        dbo::hasMany(a, won_competitions_, dbo::ManyToMany,
                     "winners_competition");
        dbo::field(a, games_stat_, "games_stat");
        dbo::hasMany(a, auth_infos_, dbo::ManyToOne, "user");
        dbo::hasMany(a, gps_, dbo::ManyToOne, "init");
        dbo::hasMany(a, cps_, dbo::ManyToOne, "init");
    }

    /** Return (first of) auth info */
    AuthInfoPtr auth_info() const;

    /** Get user name */
    const Wt::WString& username() const {
        return username_;
    }

    /** Get HTML-escaped user name */
    Wt::WString safe_username() const;

    /** Set user name */
    void set_username(Wt::WString username) {
        username_ = username;
    }

    /** Get rights */
    Rights rights() const {
        return rights_;
    }

    /** Return if the user has the permission */
    bool has_permission(Rights perm) const;

    /** Set user's permission */
    void set_permission(Rights perm, bool can = true);

    /** Set rights */
    void set_rights(Rights rights) {
        rights_ = rights;
    }

    /** Run this when user is logging in */
    void login();

    /** Run this when user is logging out */
    void logout();

    /** Return if the user is online */
    bool online() const {
        return sessions_ != 0;
    }

    /** Get total time spent online */
    const Td& online_time() const {
        return online_time_;
    }

    /** Return query of all games in which the user took part */
    dbo::Query<GamePtr> games() const;

    /** Return if a user can set classification of this user */
    bool can_set_classification(const UserPtr& user) const;

    /** Try to set classification by a user */
    void set_classification(const UserPtr& user, Classification classification);

    /** Get classification */
    Classification classification() const;

    /** Return string representation of classification */
    Wt::WString classification_str() const;

    /** Return if a user can confirm classification of this user */
    bool can_confirm_classification(const UserPtr& user) const;

    /** Try to confirm classification by a user */
    void confirm_classification(const UserPtr& user);

    /** Get if the classification is confirmed */
    bool classification_confirmed() const;

    /** Get user confirmed classification */
    const UserPtr& classification_confirmer() const;

    /** Get games statistics */
    const EloPlayer& games_stat() const {
        return games_stat_;
    }

    /** Access games statistics */
    EloPlayer& games_stat() {
        return games_stat_;
    }

    /** Return string representation of user classification */
    static Wt::WString classification2str(Classification c);

private:
    Wt::WString username_;
    Rights rights_; // default constructor: 0
    int sessions_;
    Wt::WDateTime last_enter_;
    Td online_time_;

    Games white_games_;
    Games black_games_;
    Games won_games_;
    Games init_games_;

    Classification classification_;
    UserPtr classification_confirmer_;
    Users classification_confirmed_;

    Competitions competitions_;
    Competitions init_competitions_;
    Competitions virtual_allower_;
    Competitions won_competitions_;

    EloPlayer games_stat_;

    AuthInfos auth_infos_;

    GPs gps_;
    CPs cps_;
};

}

DBO_EXTERN_TEMPLATES(thechess::User);

#endif

