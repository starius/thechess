/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_GLOBAL_HPP_
#define THECHESS_MODEL_GLOBAL_HPP_

#include <string>
#include <vector>
#include <boost/utility/binary.hpp>

#include <Wt/WGlobal>

namespace dbo = Wt::Dbo;

/** \file model/global.hpp Declarations of all models.
This file should be included by hpp.

For UserPtr, etc typedefs dbo templates are used,
but for doxygen simple c++ types (pointers) are used.

\ingroup model
*/

/** \name Declarations of all models */
/* @{ */

namespace thechess {

class User;
class GP;
class Game;
class CP;
class Competition;
class Comment;
class BD;
class IpBan;

#ifndef DOXYGEN_ONLY
typedef dbo::ptr<User> UserPtr;
typedef dbo::ptr<GP> GPPtr;
typedef dbo::ptr<Game> GamePtr;
typedef dbo::ptr<CP> CPPtr;
typedef dbo::ptr<Competition> CompetitionPtr;
typedef dbo::ptr<Comment> CommentPtr;
typedef dbo::ptr<BD> BDPtr;
typedef dbo::ptr<IpBan> IpBanPtr;
#else
/** Dbo pointer to User */
typedef User* UserPtr;

/** Dbo pointer to GP */
typedef GP* GPPtr;

/** Dbo pointer to Game */
typedef Game* GamePtr;

/** Dbo pointer to CP */
typedef CP* CPPtr;

/** Dbo pointer to Competition */
typedef Competition* CompetitionPtr;

/** Dbo pointer to Comment */
typedef Comment* CommentPtr;

/** Dbo pointer to BD */
typedef BD* BDPtr;

/** Dbo pointer to IpBan */
typedef IpBan* IpBanPtr;
#endif

#ifndef DOXYGEN_ONLY
typedef dbo::collection<UserPtr> Users;
typedef dbo::collection<GPPtr> GPs;
typedef dbo::collection<GamePtr> Games;
typedef dbo::collection<CPPtr> CPs;
typedef dbo::collection<CompetitionPtr> Competitions;
typedef dbo::collection<CommentPtr> Comments;
typedef dbo::collection<BDPtr> BDs;
typedef dbo::collection<IpBanPtr> IpBans;
#else
/** User dbo collection */
typedef User* Users;

/** GP dbo collection */
typedef GP* GPs;

/** Game dbo collection */
typedef Game* Games;

/** CP dbo collection */
typedef CP* CPs;

/** Competition dbo collection */
typedef Competition* Competitions;

/** Comment dbo collection */
typedef Comment* Comments;

/** BD dbo collection */
typedef BD* BDs;

/** IpBan dbo collection */
typedef IpBan* IpBans;

#endif

/** User vector */
typedef std::vector<UserPtr> UsersVector;

/** GP vector */
typedef std::vector<GPPtr> GPsVector;

/** Game vector */
typedef std::vector<GamePtr> GamesVector;

/** CP vector */
typedef std::vector<CPPtr> CPsVector;

/** Competition vector */
typedef std::vector<CompetitionPtr> CompetitionsVector;

/** Comment vector */
typedef std::vector<CommentPtr> CommentsVector;

/** BD vector */
typedef std::vector<BDPtr> BDsVector;

/** IpBan vector */
typedef std::vector<IpBanPtr> IpBansVector;

}

/* @} */

/** \name Model-related enums */
/* @{ */

namespace thechess {

/** Competition type */
enum CompetitionType {
    NO_COMPETITION_TYPE = 0, /**< Not a real type. Not in database */
    CLASSICAL = 10, /**< Classical competition */
    STAGED = 20, /**< Staged competition */
    TEAM = 30 /**< Team competition */
};

/** Array of all CompetitionType members */
const CompetitionType COMPETITION_TYPES[] = {CLASSICAL, STAGED, TEAM};

/** Length of COMPETITION_TYPES */
const int COMPETITION_TYPES_SIZE = 3;

/** Classification of player */
enum Classification {
    SUPER_GRANDMASTER = 110, /**< Super grandmaster */
    GRANDMASTER = 100, /**< Grandmaster */
    INTERNATIONAL_MASTER = 90, /**< International master */
    NATIONAL_MASTER = 80, /**< National master */
    EXPERT = 70, /**< Expert (master) */
    CLASSIFICATION_A = 50, /**< A (I) */
    CLASSIFICATION_B = 45, /**< B (II) */
    CLASSIFICATION_C = 40, /**< C (III) */
    CLASSIFICATION_D = 35, /**< D (IV) */
    CLASSIFICATION_E = 30, /**< E (V, newbie) */
    CLASSIFICATION_F = 25, /**< F (newbie) */
    CLASSIFICATION_G = 20, /**< G (newbie) */
    CLASSIFICATION_H = 15, /**< H (newbie) */
    CLASSIFICATION_I = 10, /**< I (newbie) */
    CLASSIFICATION_J = 5, /**< J (newbie) */
    NO_CLASSIFICATION = 0 /**< Player without assigned classification */
};

/** Array of all Classification members */
const Classification CLASSIFICATIONS[] = {
    SUPER_GRANDMASTER, GRANDMASTER, INTERNATIONAL_MASTER,
    NATIONAL_MASTER, EXPERT, CLASSIFICATION_A, CLASSIFICATION_B,
    CLASSIFICATION_C, CLASSIFICATION_D, CLASSIFICATION_E, CLASSIFICATION_F,
    CLASSIFICATION_G, CLASSIFICATION_H, CLASSIFICATION_I, CLASSIFICATION_J,
    NO_CLASSIFICATION
};

/** Length of CLASSIFICATIONS */
const int CLASSIFICATIONS_SIZE = 16;

/** Model type */
enum ObjectType {
    NOEVENT,
    GAME,
    USER,
    COMMENT,
    COMPETITION,
    IP_BAN
};

/** Rights */
enum UserRights {
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

    /** Right to show non-default avatar */
    AVATAR_NONDEFAULT = BOOST_BINARY(00001000 00000000),

    /** Right to show non-default avatar */
    EDIT_DESCRIPTION = BOOST_BINARY(00010000 00000000),

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
                   CLASSIFICATION_CHANGER |
                   AVATAR_NONDEFAULT |
                   EDIT_DESCRIPTION
};

/** Rights */
enum AdminRights {
    /** No permissions */
    NO_ADMIN_RIGHTS = 0,

    /** Right (un)pause games and return user from vacation */
    TIME_WIZARD = BOOST_BINARY(00001000 00000000 00000000),

    /** Right to (un)remove comments, create topic and approve drafts */
    COMMENTS_REMOVER = BOOST_BINARY(00010000 00000000 00000000),

    /** Right to confirm or change user classification */
    CLASSIFICATION_CONFIRMER = BOOST_BINARY(00100000 00000000 00000000),

    /** Right to change or cancel competition or kick users from them */
    COMPETITION_CHANGER = BOOST_BINARY(01000000 00000000 00000000),

    /** Right to (un)remove a user */
    USER_REMOVER = BOOST_BINARY(10000000 00000000 00000000),

    /** Right to view virtuals.
      - allow them in competition,
      - set game.norating,
      - run stats recalculation.
    */
    VIRTUALS_VIEWER = BOOST_BINARY(00000001 00000000 00000000 00000000),

    /** Right to change rights that are part of REGULAR_USER */
    REGULAR_RIGHTS_CHANGER = BOOST_BINARY(0100 00000000 00000000 00000000),

    /** Right to ban registration for the user by IP */
    REGISTRATION_BANNER = BOOST_BINARY(00001000 00000000 00000000 00000000),

    /** \todo Right to edit Game, GP and CP name and description */
    RECORDS_EDITOR = BOOST_BINARY(00010000 00000000 00000000 00000000),

    /** Right to read logs */
    LOGS_READER = BOOST_BINARY(00100000 00000000 00000000 00000000),

    /** Special rights */
    SUPER_RIGHTS = TIME_WIZARD |
                   COMMENTS_REMOVER |
                   CLASSIFICATION_CONFIRMER |
                   COMPETITION_CHANGER |
                   USER_REMOVER |
                   VIRTUALS_VIEWER |
                   REGULAR_RIGHTS_CHANGER |
                   REGISTRATION_BANNER |
                   RECORDS_EDITOR |
                   LOGS_READER,

    /** Modeator */
    MODERATOR = SUPER_RIGHTS,

    /** Right to change rights that are part of SUPER_RIGHTS */
    SUPER_RIGHTS_CHANGER = BOOST_BINARY(1000000 00000000 00000000 00000000),

    /** Admin */
    ADMIN = MODERATOR | SUPER_RIGHTS_CHANGER
};

/** Settings */
enum UserSettings {
    /** No settings */
    SWITCH_NO = 0,

    /** Show big pieces */
    SWITCH_BIG = BOOST_BINARY(00000001),

    /** Highlight lastmove */
    SWITCH_LASTMOVE = BOOST_BINARY(00000010),

    /** Show only validated comments */
    SWITCH_ONLY_OK_COMMENTS = BOOST_BINARY(00000100),

    /** Show only my comments */
    SWITCH_ONLY_MY_COMMENTS = BOOST_BINARY(00001000),

    /** Show only online users */
    SWITCH_ONLY_ONLINE_USERS = BOOST_BINARY(00010000),

    /** Show only not removed users */
    SWITCH_ONLY_NOT_REMOVED_USERS = BOOST_BINARY(00100000),

    /** Show only my competitions */
    SWITCH_ONLY_MY_COMPETITIONS = BOOST_BINARY(01000000),

    /** Show only my games */
    SWITCH_ONLY_MY_GAMES = BOOST_BINARY(10000000),

    /** Show only commented games */
    SWITCH_ONLY_COMMENTED_GAMES = BOOST_BINARY(00000001 00000000),

    /** Show only my gp */
    SWITCH_ONLY_MY_GP = BOOST_BINARY(00000010 00000000),

    /** Show only my cp */
    SWITCH_ONLY_MY_CP = BOOST_BINARY(00000100 00000000),

    /** Show user names instead of game numbers */
    SWITCH_NAMES_IN_MYMENU = BOOST_BINARY(00001000 00000000),

    /** Move confirmation */
    SWITCH_MOVE_CONFIRMATION = BOOST_BINARY(00010000 00000000),

    /** Show email to regular users */
    SWITCH_PUBLIC_EMAIL = BOOST_BINARY(00100000 00000000),

    /** Show game numbers (nstead of score) in classical competition table */
    SWITCH_GAME_NUMBERS_IN_TABLE = BOOST_BINARY(01000000 00000000),

    /** Show only draft comments (for moder) */
    SWITCH_ONLY_DRAFT_COMMENTS = BOOST_BINARY(10000000 00000000),

    /** Hide paused games */
    SWITCH_HIDE_PAUSED_GAMES = BOOST_BINARY(00000001 00000000 00000000),

    /** Hide Online label from games menu */
    SWITCH_HIDE_ONLINE = BOOST_BINARY(00000010 00000000 00000000),

    /** Hide Countdown from games menu */
    SWITCH_HIDE_COUNTDOWN = BOOST_BINARY(00000100 00000000 00000000),

    /** Formatting in chat */
    SWITCH_FORMATTING_CHAT = BOOST_BINARY(00001000 00000000 00000000),

    /** More options when formatting */
    SWITCH_MORE_FORMATTING = BOOST_BINARY(00010000 00000000 00000000),

    /** Less info on game page */
    SWITCH_LESS_GAME_INFO = BOOST_BINARY(00100000 00000000 00000000),

    /** Show competitor's countdown in MyGamesList as well as my one */
    SWITCH_COMPETOR_TIME = BOOST_BINARY(01000000 00000000 00000000),

    /** No move hints in BoardWidget */
    SWITCH_NO_MOVE_HINTS = BOOST_BINARY(10000000 00000000 00000000),

    /** Default value of settings of new user or Anonymous */
    SWITCH_DEFAULT = SWITCH_LASTMOVE |
                     SWITCH_ONLY_NOT_REMOVED_USERS |
                     SWITCH_ONLY_OK_COMMENTS |
                     SWITCH_PUBLIC_EMAIL
};

/** Ban state */
enum BanState {
    /** Ban disabled */
    BAN_DISABLED = 0,

    /** Limited rights of new users and anonymous */
    LIMITED_NEW = 10,

    /** Limited rights of new users and limited rights of anonymous */
    NO_REGISTRATION = 20,

    /** Can not create Application (redirect) */
    ABSOLUTE_BAN = 30
};

}

/* @} */

/** \name Auth */
/* @{ */

namespace thechess {

/** Link beetween the authentication information class and User class */
typedef Wt::Auth::Dbo::AuthInfo<User> AuthInfo;

/** Database pointer to AuthInfo */
typedef dbo::ptr<AuthInfo> AuthInfoPtr;

/** Collection of AuthInfo */
typedef dbo::collection<AuthInfoPtr> AuthInfos;

/** Authentication user database */
typedef Wt::Auth::Dbo::UserDatabase<AuthInfo> UserDatabase;

}

/* @} */

/** \name Models-related classes */
/* @{ */

namespace thechess {

class UserPair;
class StagedCompetition;
class Record;
class Object;
class NewMessage;
class EloPlayer;

template<typename T> struct WhiteBlack;

}

/* @} */

#endif

