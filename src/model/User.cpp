/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WRandom>
#include <Wt/Utils>

#include "model/all.hpp"
#include "config.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::User);

namespace thechess {

User::User(const Wt::WString& username):
    username_(username),
    rights_(REGULAR_USER),
    sessions_(0),
    classification_(NO_CLASSIFICATION),
    games_stat_(true) {
}

User::User() {
}

AuthInfoPtr User::auth_info() const {
    return auth_infos_.front();
}

bool User::removed() const {
    return auth_info()->status() == Wt::Auth::User::Disabled;
}

void User::set_removed(bool removed) {
    Wt::Auth::User::Status status = removed ?
                                    Wt::Auth::User::Disabled :
                                    Wt::Auth::User::Normal;
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
    return Wt::Utils::htmlEncode(username());
}

bool User::has_permission(Rights perm) const {
    return (rights() & perm) == perm;
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

dbo::Query<GamePtr> User::games() const {
    return session()->find<Game>()
           .where("white_id = ? or black_id = ? or init_id = ?")
           .bind(id()).bind(id()).bind(id());
}

void User::login() {
    if (sessions_ == 0) {
        last_enter_ = now();
    }
    sessions_ += 1;
}

void User::logout() {
    sessions_ -= 1;
    if (sessions_ < 0) {
        sessions_ = 0;
    }
    if (sessions_ == 0) {
        online_time_ += now() - last_enter_;
    }
}

bool User::can_set_classification(const UserPtr& user) const {
    return user && user.id() == id();
}

void User::set_classification(const UserPtr& user,
                              Classification classification) {
    if (can_set_classification(user)) {
        classification_ = classification;
        classification_confirmer_.reset();
    }
}

Classification User::classification() const {
    return classification_;
}

Wt::WString User::classification_str() const {
    return classification2str(classification());
}

bool User::can_confirm_classification(const UserPtr& user) const {
    return user && user->has_permission(CLASSIFICATION_CONFIRMER);
}

void User::confirm_classification(const UserPtr& user) {
    if (can_confirm_classification(user)) {
        classification_confirmer_ = user;
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

}

