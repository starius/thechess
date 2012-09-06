/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_USER_H_
#define THECHESS_MODEL_USER_H_

#include <boost/function.hpp>

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
    typedef UserRights Rights;

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
        dbo::field(a, competitions_stat_, "compteitions_stat");
        dbo::field(a, comment_base_, "comment_base");
        dbo::field(a, settings_, "settings");
        dbo::field(a, karma_, "karma");
        dbo::field(a, registration_date_, "registration_date");
        dbo::field(a, avatar_path_, "avatar_path");
        dbo::field(a, filter_min_online_, "filter_min_online");
        dbo::field(a, description_, "description");
        dbo::field(a, locale_, "locale", /* size */ 5);
    }

    /** Return (first of) auth info */
    AuthInfoPtr auth_info() const;

    /** Return the Auth::User corresponding to a Dbo user.
    \warning tApp must be defined.
    */
    Wt::Auth::User auth_user() const;

    /** Return verified e-mail */
    std::string email() const;

    /** Set unverified e-mail address and send confirmation mail.
    \warning tApp must be defined.
    */
    void set_email(const std::string& email);

    /** Return if the user was removed */
    bool removed() const;

    /** Set user removed status.
    Removed user loses all his rights.
    */
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

    /** Return if the user has the permission */
    static bool has_permission(Rights perm, Rights rights);

    /** Set user's permission */
    void set_permission(Rights perm, bool can = true);

    /* Return if \p who can change the right of this user */
    bool can_change_right(Rights perm, const UserPtr& who) const;

    /* Return if \p who can change some right of this user */
    bool can_change_right(const UserPtr& who) const;

    /** Set rights */
    void set_rights(Rights rights) {
        rights_ = rights;
    }

    /** Return translation id for this right */
    static const char* right_to_str(Rights right);

    /** Apply function f to each possible right.
    \note Groups of rights, such as REGULAR_USER, are not iterated.
    */
    static void for_each_right(const boost::function<void(Rights)> f);

    /** Return if it is a regular right */
    static bool is_regular_right(Rights right);

    /** Return if it is a super right */
    static bool is_super_right(Rights right);

    /** Run this when user is logging in */
    void login();

    /** Run this when user is logging out */
    void logout();

    /** Return if the user is online */
    bool online() const {
        return sessions_ != 0;
    }

    /** Return number of sessions, opened by the user */
    int sessions() const {
        return sessions_;
    }

    /** Return date time of last enter */
    const Wt::WDateTime& last_enter() const {
        return last_enter_;
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

    /** Try to discard classification by a user */
    void discard_classification(const UserPtr& user);

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

    /** Get competitions user is member of */
    const Competitions& competitions() const {
        return competitions_;
    }

    /** Get competitions user is winner of */
    const Competitions& won_competitions() const {
        return won_competitions_;
    }

    /** Return string representation of user classification */
    static Wt::WString classification2str(Classification c);

    /** Get competitions statistics */
    const EloPlayer& competitions_stat() const {
        return competitions_stat_;
    }

    /** Access competitions statistics */
    EloPlayer& competitions_stat() {
        return competitions_stat_;
    }

    /** Return if this user has comment base for private messages */
    bool has_comment_base() const;

    /** Return comment base for private messages */
    const CommentPtr& comment_base();

    /** Get sessings */
    UserSettings settings() const {
        return settings_;
    }

    /** Set sessings */
    void set_settings(UserSettings settings) {
        settings_ = settings;
    }

    /** Return if the user has enabled the setting */
    bool has_setting(UserSettings setting) const;

    /** Return if the user has enabled the setting */
    static bool has_setting(UserSettings setting, UserSettings settings);

    /** Return if the user has enabled the setting.
    This variant works even for Anonymous.
    \warning tApp must be defined.
    */
    static bool has_s(UserSettings setting);

    /** Set if the user has enabled the setting.
    This variant works even for Anonymous.
    \warning tApp must be defined.
    */
    static bool set_s(UserSettings setting, bool value);

    /** Enable or disable the setting */
    void set_setting(UserSettings setting, bool value);

    /** Get karma (not used) */
    int karma() const {
        return karma_;
    }

    /** Set karma */
    void set_karma(int karma) {
        karma_ = karma;
    }

    /** Get registration date */
    const Wt::WDateTime& registration_date() const {
        return registration_date_;
    }

    /** Get avatar path (compatibility with old version) */
    const std::string& avatar_path() const {
        return avatar_path_;
    }

    /** Return min online_time of other user to send private message */
    const Td& filter_min_online() const {
        return filter_min_online_;
    }

    /** Set min online_time of other user to send private message */
    void set_filter_min_online(const Td& filter_min_online) {
        filter_min_online_ = filter_min_online;
    }

    /** Get description (written by the user) */
    const Wt::WString& description() const {
        return description_;
    }

    /** Get description (written by the user) or [Deleted] */
    Wt::WString safe_description() const;

    /** Set description (written by the user) */
    void set_description(const Wt::WString& description) {
        description_ = description;
    }

    /** Preferred locale.
    Empty value cause using browser default.
    */
    const std::string& locale() const {
        return locale_;
    }

    /** Set prefered locale */
    void set_locale(const std::string& locale) {
        locale_ = locale;
    }

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

    EloPlayer competitions_stat_;
    CommentPtr comment_base_;
    UserSettings settings_;
    int karma_;
    Wt::WDateTime registration_date_;
    std::string avatar_path_;
    Td filter_min_online_;
    Wt::WString description_;
    std::string locale_;

    AuthInfos auth_infos_;

    GPs gps_;
    CPs cps_;
};

}

DBO_EXTERN_TEMPLATES(thechess::User);

#endif

