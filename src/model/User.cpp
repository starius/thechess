/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WRandom>
#include <Wt/Utils>

#include "model/all.hpp"
#include "config.hpp"
#include "Application.hpp" // FIXME
#include "Server.hpp" // FIXME
#include "Options.hpp" // FIXME

DBO_INSTANTIATE_TEMPLATES(thechess::User);

namespace thechess {

User::User(const Wt::WString& username):
    username_(username),
    rights_(NONE),
    admin_rights_(NO_ADMIN_RIGHTS),
    last_online_(now()),
    classification_(NO_CLASSIFICATION),
    games_stat_(true),
    competitions_stat_(true),
    settings_(SWITCH_DEFAULT),
    karma_(0),
    registration_date_(now()) {
    if (IpBan::am_i_banned() == BAN_DISABLED) {
        set_rights(Options::instance()->regular_user_rights());
    } else {
        set_rights(Options::instance()->banned_ip_user_rights());
    }
}

User::User() {
}

AuthInfoPtr User::auth_info() const {
    return auth_infos_.size() ? auth_infos_.front() : AuthInfoPtr();
}

Wt::Auth::User User::auth_user() const {
    return tApp->session().user_database().find(auth_info());
}

std::string User::email() const {
    return auth_info() ? auth_info()->email() : "";
}

void User::set_email(const std::string& email) {
    Server::instance()->auth_service().verifyEmailAddress(auth_user(), email);
}

bool User::removed() const {
    return rights_ == NONE;
}

void User::set_removed(bool removed) {
    if (removed) {
        set_rights(NONE);
    } else {
        set_rights(Options::instance()->regular_user_rights());
    }
    if (!auth_info()) {
        return;
    }
    Wt::Auth::User::Status status = removed ?
                                    Wt::Auth::User::Disabled :
                                    Wt::Auth::User::Normal;
    auth_info().reread();
    auth_info().modify()->setStatus(status);
}

bool User::can_remove(const UserPtr& victim) const {
    return has_permission(USER_REMOVER) && victim &&
           !victim->has_permission(USER_REMOVER) &&
           self() != victim;
}

Wt::WString User::username20() const {
    std::wstring u = username().value();
    if (u.size() > 20) {
        u.resize(20);
    }
    return Wt::WString(u);
}

Wt::WString User::safe_username() const {
    if (removed() &&
            (!tApp->user() || !tApp->user()->has_permission(USER_REMOVER))) {
        return Wt::WString::tr("tc.user.Removed_message");
    } else {
        std::string name = Wt::Utils::htmlEncode(username20()).toUTF8();
        boost::replace_all(name, " ", "&nbsp;");
        return Wt::WString::fromUTF8(name);
    }
}

bool User::has_permission(Rights perm) const {
    return has_permission(perm, rights());
}

bool User::has_permission(AdminRights perm) const {
    return has_permission(perm, admin_rights());
}

UserRights User::rights() const {
    Rights result = rights_;
    if (vacation_until_.isValid() && vacation_until_ > now()) {
        result = Rights(result & Options::instance()->vacation_rights());
    }
    return result;
}

bool User::has_permission(Rights perm, Rights rights) {
    return (rights & perm) == perm;
}

bool User::has_permission(AdminRights perm, AdminRights rights) {
    return (rights & perm) == perm;
}

void User::set_permission(Rights perm, bool can) {
    Rights r = rights();
    if (can) {
        r = Rights(r | perm);
    } else {
        r = Rights(r & (~perm));
    }
    set_rights(r);
}

void User::set_permission(AdminRights perm, bool can) {
    AdminRights r = admin_rights();
    if (can) {
        r = AdminRights(r | perm);
    } else {
        r = AdminRights(r & (~perm));
    }
    set_rights(r);
}

bool User::can_change_right(Rights r, const UserPtr& who) const {
    if (!who || who->removed() || removed()) {
        return false;
    }
    if (who != self() && admin_rights()) {
        return false;
    }
    return who->has_permission(REGULAR_RIGHTS_CHANGER);
}

bool User::can_change_right(AdminRights r, const UserPtr& who) const {
    if (!who || who->removed() || removed()) {
        return false;
    }
    if (who != self() && has_permission(SUPER_RIGHTS_CHANGER)) {
        return false;
    }
    return who->has_permission(SUPER_RIGHTS_CHANGER);
}

static void rights_check(bool& result, const User* me,
                         User::Rights r, const UserPtr& who) {
    if (me->can_change_right(r, who)) {
        result = true;
    }
}

static void admin_rights_check(bool& result, const User* me,
                               AdminRights r, const UserPtr& who) {
    if (me->can_change_right(r, who)) {
        result = true;
    }
}

bool User::can_change_right(const UserPtr& who) const {
    if (!who) {
        return false;
    }
    if (!who->has_permission(REGULAR_RIGHTS_CHANGER) &&
            !who->has_permission(SUPER_RIGHTS_CHANGER)) {
        return false;
    }
    bool result = false;
    for_each_right(boost::bind(rights_check, boost::ref(result), this,
                               _1, who));
    for_each_admin(boost::bind(admin_rights_check, boost::ref(result), this,
                               _1, who));
    return result;
}

const char* User::right_to_str(Rights right) {
    if (right == GAME_CREATOR) {
        return "tc.user.game_creator";
    } else if (right == GAME_JOINER) {
        return "tc.user.game_joiner";
    } else if (right == CHAT_WRITER) {
        return "tc.user.chat_writer";
    } else if (right == PRIVATE_WRITER) {
        return "tc.user.private_writer";
    } else if (right == FORUM_POST_CREATOR) {
        return "tc.user.forum_post_creator";
    } else if (right == FORUM_COMMENT_CREATOR) {
        return "tc.user.forum_comment_creator";
    } else if (right == COMPETITION_CREATOR) {
        return "tc.user.competition_creator";
    } else if (right == GP_CREATOR) {
        return "tc.user.gp_creator";
    } else if (right == CP_CREATOR) {
        return "tc.user.cp_creator";
    } else if (right == COMPETITION_JOINER) {
        return "tc.user.competition_joiner";
    } else if (right == CLASSIFICATION_CHANGER) {
        return "tc.user.classification_changer";
    } else if (right == AVATAR_NONDEFAULT) {
        return "tc.user.avatar_nondefault";
    } else if (right == EDIT_DESCRIPTION) {
        return "tc.user.edit_description";
    } else if (right == CREATE_TEAM) {
        return "tc.user.create_team";
    } else if (right == JOIN_TEAM) {
        return "tc.user.join_team";
    }
    return "tc.user.unknown_right";
}

const char* User::right_to_str(AdminRights right) {
    if (right == TIME_WIZARD) {
        return "tc.user.time_wizard";
    } else if (right == TEAM_CHANGER) {
        return "tc.user.team_changer";
    } else if (right == COMMENTS_REMOVER) {
        return "tc.user.comments_remover";
    } else if (right == CLASSIFICATION_CONFIRMER) {
        return "tc.user.classification_confirmer";
    } else if (right == COMPETITION_CHANGER) {
        return "tc.user.competition_changer";
    } else if (right == USER_REMOVER) {
        return "tc.user.user_remover";
    } else if (right == VIRTUALS_VIEWER) {
        return "tc.user.virtuals_viewer";
    } else if (right == REGULAR_RIGHTS_CHANGER) {
        return "tc.user.regular_rights_changer";
    } else if (right == REGISTRATION_BANNER) {
        return "tc.user.registration_banner";
    } else if (right == RECORDS_EDITOR) {
        return "tc.user.records_editor";
    } else if (right == LOGS_READER) {
        return "tc.user.logs_reader";
    } else if (right == SUPER_RIGHTS_CHANGER) {
        return "tc.user.super_rights_changer";
    }
    return "tc.user.unknown_right";
}

void User::for_each_right(const boost::function<void(Rights)> f) {
    f(GAME_CREATOR);
    f(GAME_JOINER);
    f(CHAT_WRITER);
    f(PRIVATE_WRITER);
    f(FORUM_POST_CREATOR);
    f(FORUM_COMMENT_CREATOR);
    f(COMPETITION_CREATOR);
    f(GP_CREATOR);
    f(CP_CREATOR);
    f(COMPETITION_JOINER);
    f(CLASSIFICATION_CHANGER);
    f(AVATAR_NONDEFAULT);
    f(EDIT_DESCRIPTION);
    f(CREATE_TEAM);
    f(JOIN_TEAM);
}

void User::for_each_admin(const boost::function<void(AdminRights)> f) {
    f(TEAM_CHANGER);
    f(TIME_WIZARD);
    f(COMMENTS_REMOVER);
    f(CLASSIFICATION_CONFIRMER);
    f(COMPETITION_CHANGER);
    f(USER_REMOVER);
    f(VIRTUALS_VIEWER);
    f(REGULAR_RIGHTS_CHANGER);
    f(REGISTRATION_BANNER);
    f(RECORDS_EDITOR);
    f(LOGS_READER);
    f(SUPER_RIGHTS_CHANGER);
}

bool User::online() const {
    return (now() - last_online_) < Options::instance()->away_timeout();
}

void User::update_last_online() {
    if (online()) {
        online_time_ += (now() - last_online_);
    }
    last_online_ = now();
}

void User::logout_last_online() {
    last_online_ = now() - Options::instance()->away_timeout() - SECOND;
}

dbo::Query<GamePtr> User::games() const {
    return session()->find<Game>()
           .where("white_id = ? or black_id = ? or init_id = ?")
           .bind(id()).bind(id()).bind(id());
}

void User::set_classification(Classification classification) {
    classification_ = classification;
    classification_confirmer_.reset();
}

Classification User::classification() const {
    return classification_;
}

Wt::WString User::classification_str() const {
    return classification2str(classification());
}

bool User::can_confirm_classification(const UserPtr& user) const {
    return user && user->has_permission(CLASSIFICATION_CONFIRMER) &&
           user != self();
    // FIXME http://redmine.webtoolkit.eu/boards/2/topics/4200
}

void User::confirm_classification(const UserPtr& user) {
    if (can_confirm_classification(user)) {
        classification_confirmer_ = user;
    }
}

void User::discard_classification(const UserPtr& user) {
    if (can_confirm_classification(user)) {
        classification_confirmer_.reset();
    }
}

bool User::classification_confirmed() const {
    return classification_confirmer_;
}

const UserPtr& User::classification_confirmer() const {
    return classification_confirmer_;
}

Wt::WString User::classification2str(Classification c) {
    if (c == SUPER_GRANDMASTER) {
        return Wt::WString::tr("tc.user.classification_super_grandmaster");
    }
    if (c == GRANDMASTER) {
        return Wt::WString::tr("tc.user.classification_grandmaster");
    }
    if (c == INTERNATIONAL_MASTER) {
        return Wt::WString::tr("tc.user.classification_intarnational_master");
    }
    if (c == NATIONAL_MASTER) {
        return Wt::WString::tr("tc.user.classification_national_master");
    }
    if (c == EXPERT) {
        return Wt::WString::tr("tc.user.classification_expert");
    }
    if (c == CLASSIFICATION_A) {
        return Wt::WString::tr("tc.user.classification_a");
    }
    if (c == CLASSIFICATION_B) {
        return Wt::WString::tr("tc.user.classification_b");
    }
    if (c == CLASSIFICATION_C) {
        return Wt::WString::tr("tc.user.classification_c");
    }
    if (c == CLASSIFICATION_D) {
        return Wt::WString::tr("tc.user.classification_d");
    }
    if (c == CLASSIFICATION_E) {
        return Wt::WString::tr("tc.user.classification_e");
    }
    if (c == CLASSIFICATION_F) {
        return Wt::WString::tr("tc.user.classification_f");
    }
    if (c == CLASSIFICATION_G) {
        return Wt::WString::tr("tc.user.classification_g");
    }
    if (c == CLASSIFICATION_H) {
        return Wt::WString::tr("tc.user.classification_h");
    }
    if (c == CLASSIFICATION_I) {
        return Wt::WString::tr("tc.user.classification_i");
    }
    if (c == CLASSIFICATION_J) {
        return Wt::WString::tr("tc.user.classification_j");
    }
    return Wt::WString::tr("tc.user.classification_none");
}

bool User::has_comment_base() const {
    return comment_base_;
}

const CommentPtr& User::comment_base() {
    if (!comment_base_) {
        comment_base_ = session()->add(new Comment(true));
        comment_base_.modify()->set_type(Comment::CHAT_ROOT);
        comment_base_.flush();
    }
    return comment_base_;
}

bool User::has_setting(UserSettings setting) const {
    return has_setting(setting, settings());
}

bool User::has_setting(UserSettings setting, UserSettings settings) {
    return (settings & setting) == setting;
}

bool User::has_s(UserSettings setting) {
    dbo::Transaction t(tApp->session());
    UserSettings settings = tApp->user() ? tApp->user()->settings() :
                            Options::instance()->default_settings();
    return has_setting(setting, settings);
}

bool User::set_s(UserSettings setting, bool value) {
    dbo::Transaction t(tApp->session());
    if (tApp->user()) {
        tApp->user().reread();
        tApp->user().modify()->set_setting(setting, value);
    }
}

void User::set_setting(UserSettings setting, bool value) {
    UserSettings s = settings();
    if (value) {
        s = UserSettings(s | setting);
    } else {
        s = UserSettings(s & (~setting));
    }
    set_settings(s);
}

Wt::WString User::safe_description() const {
    Wt::WString result = description();
    Wt::Utils::removeScript(result);
    return result;
}

bool User::is_blocked(const UserPtr& bad, const UserPtr& good) {
    return bad && good && good->i_blocked_them().count(bad);
}

void User::add_to_my_filter(const UserPtr& bad) {
    i_blocked_them_.insert(bad);
}

void User::remove_from_my_filter(const UserPtr& bad) {
    i_blocked_them_.erase(bad);
}

bool User::can_send_message(const UserPtr& to) const {
    return to && self() != to &&
           (admin_rights() ||
            (online_time() >= to->filter_min_online() &&
             Comment::can_create(self(), Comment::PRIVATE_MESSAGE) &&
             !is_blocked(self(), to)));
}

void User::send_message(UserPtr from, UserPtr to, const Wt::WString& text) {
    if (!to || from == to) {
        return;
    }
    CommentPtr base = to->comment_base();
    if (!base) {
        to.reread();
        base = to.modify()->comment_base();
        base.flush();
    }
    CommentPtr from_base;
    if (from) {
        from_base = from->comment_base();
        if (!from_base) {
            from.reread();
            from_base = from.modify()->comment_base();
            from_base.flush();
        }
    }
    CommentPtr message = to.session()->add(new Comment(true));
    message.modify()->set_text(text);
    message.modify()->set_init(from);
    message.modify()->set_parent(base);
    message.modify()->set_root(base);
    message.modify()->set_type(Comment::PRIVATE_MESSAGE);
    message.flush();
    t_emit_after(COMMENT, base.id());
    t_emit(COMMENT, from_base.id());
    if (from) {
        t_emit(new NewMessage(to.id(), from.id()));
    }
}

void User::check(Wt::Wc::notify::TaskPtr task) {
    task->set_notify_needed(check_vacation());
    if (vacation_until_.isValid() && vacation_until_ > now()) {
        t_task(task, vacation_until_);
    }
}

bool User::check_vacation() {
    if (vacation_until_.isValid() && vacation_until_ < now()) {
        vacation_until_ = Wt::WDateTime();
        return true;
    } else {
        return false;
    }
}

}

