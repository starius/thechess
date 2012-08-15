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
        /** No permissions */
        NONE = 0,

        /** Right to create a Game */
        GAME_CREATOR = BOOST_BINARY(00000001),

        /** Right to join a Game */
        GAME_JOINER = BOOST_BINARY(00000010),

        /** Right to join write chat */
        CHAT_WRITER = BOOST_BINARY(00000100),

        /** Right to join write chat */
        PRIVATE_WRITER = BOOST_BINARY(00001000),

        /** Right to create forum posts */
        FORUM_POST_CREATOR = BOOST_BINARY(00010000),

        /** Right to create forum comments */
        FORUM_COMMENT_CREATOR = BOOST_BINARY(00100000),

        /** Right to create Competition */
        COMPETITION_CREATOR = BOOST_BINARY(01000000),

        /** Right to create GP */
        GP_CREATOR = BOOST_BINARY(10000000),

        /** Right to create CP */
        CP_CREATOR = BOOST_BINARY(00000001 00000000),

        /** Right to join a Competition */
        COMPETITION_JOINER = BOOST_BINARY(00000010 00000000),

        /** Right to change his classification */
        CLASSIFICATION_CHANGER = BOOST_BINARY(00000100 00000000),

        /** Rights of regular user (given to new user) */
        REGULAR_USER = GAME_CREATOR |
                       GAME_JOINER |
                       CHAT_WRITER |
                       PRIVATE_WRITER |
                       FORUM_POST_CREATOR |
                       FORUM_COMMENT_CREATOR |
                       COMPETITION_CREATOR |
                       GP_CREATOR |
                       CP_CREATOR |
                       COMPETITION_JOINER |
                       CLASSIFICATION_CHANGER,

        /** Right to (un)remove comments and approve draft comments */
        COMMENTS_REMOVER = BOOST_BINARY(00010000 00000000 00000000),

        /** Right to confirm user classification */
        CLASSIFICATION_CONFIRMER = BOOST_BINARY(00100000 00000000 00000000),

        /** Right to change competition */
        COMPETITION_CHANGER = BOOST_BINARY(01000000 00000000 00000000),

        /** Right to (un)remove a user */
        USER_REMOVER = BOOST_BINARY(10000000 00000000 00000000),

        /** Right to view virtuals */
        VIRTUALS_VIEWER = BOOST_BINARY(00000001 00000000 00000000 00000000),

        /** Right to allow the competition to start if with virtuals */
        VIRTUALS_ALLOWER = BOOST_BINARY(00000010 00000000 00000000 00000000),

        /** Right to change rights that are part of REGULAR_USER */
        REGULAR_RIGHTS_CHANGER = BOOST_BINARY(0100 00000000 00000000 00000000),

        /** Right to ban registration for the user by IP */
        REGISTRATION_BANNER = BOOST_BINARY(00001000 00000000 00000000 00000000),

        /** Right to edit Game, GP and CP name and description */
        RECORDS_EDITOR = BOOST_BINARY(00010000 00000000 00000000 00000000),

        /** Special rights */
        SUPER_RIGHTS = COMMENTS_REMOVER |
                       CLASSIFICATION_CONFIRMER |
                       COMPETITION_CHANGER |
                       USER_REMOVER |
                       VIRTUALS_VIEWER |
                       VIRTUALS_ALLOWER |
                       REGULAR_RIGHTS_CHANGER |
                       REGISTRATION_BANNER |
                       RECORDS_EDITOR,

        /** Modeator */
        MODERATOR = REGULAR_USER | SUPER_RIGHTS,

        /** Right to change rights that are part of SUPER_RIGHTS */
        SUPER_RIGHTS_CHANGER = BOOST_BINARY(1000000 00000000 00000000 00000000),

        /** Admin */
        ADMIN = MODERATOR | SUPER_RIGHTS_CHANGER
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

    /** Return if the user was removed */
    bool removed() const;

    /** Set user removed status */
    void set_removed(bool removed = true);

    /** Return if this user can remove the victim */
    bool can_remove(const UserPtr& victim) const;

    /** Get user name */
    const Wt::WString& username() const {
        return username_;
    }

    /** Get user name (up to 20 chars) */
    Wt::WString username20() const;

    /** Get HTML-escaped user name.
    Up to 20 chars.
    Names of removed users are skipped if viewer is not USER_REMOVER.
    */
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

    /** Try to set classification by a user */
    void set_classification(Classification classification);

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

