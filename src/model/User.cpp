/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#define _XOPEN_SOURCE
#include <unistd.h>

#include <Wt/WRandom>

#include "model/all.hpp"
#include "config.hpp"

namespace thechess {

User::User(bool):
    rights_(REGULAR_USER),
    sessions_(0),
    classification_(NO_CLASSIFICATION),
    games_stat_(true) {
}

User::User() {
}

void User::set_password(const std::string& password) {
    std::string salt = "$.$";
    salt[1] = config::CRYPT_ID;
    salt += Wt::WRandom::generateId(config::SALT_LENGTH);
    password_ = crypt(password.c_str(), salt.c_str());
}

bool User::test_password(const std::string& password) const {
    return crypt(password.c_str(), password_.c_str()) == password_;
}

dbo::Query<GamePtr> User::games() const {
    return session()->find<Game>().where("white_id = ? or black_id = ? or init_game_id = ?")
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

bool User::can_set_classification(UserPtr user) const {
    return user && user.id() == id();
}

void User::set_classification(UserPtr user, Classification classification) {
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

bool User::can_confirm_classification(UserPtr user) const {
    return user && user->rights() >= MODERATOR;
}

void User::confirm_classification(UserPtr user) {
    if (can_confirm_classification(user)) {
        classification_confirmer_ = user;
    }
}

bool User::classification_confirmed() const {
    return classification_confirmer_;
}

UserPtr User::classification_confirmer() const {
    return classification_confirmer_;
}

}

