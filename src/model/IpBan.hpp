/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_IP_BAN_HPP_
#define THECHESS_MODEL_IP_BAN_HPP_

#include "model/model.hpp"

namespace thechess {

/** Model of a ban by IP.

\ingroup model
*/
class IpBan : public dbo::Dbo<IpBan> {
public:
    /** Default constructor.
    Should be used only by Wt::Dbo itself.
    */
    IpBan();

    /** Create instance to be added to database */
    IpBan(bool);

    /** Return translation id for ban state */
    static const char* state2str(BanState state);

    /** Get IP address */
    const std::string& ip() const {
        return ip_;
    }

    /** Set IP address */
    void set_ip(const std::string& ip) {
        ip_ = ip;
    }

    /** Get state of the ban.
    Defaults to LIMITED_NEW.
    */
    BanState state() const {
        return state_;
    }

    /** Set ban state */
    void set_state(BanState state) {
        state_ = state;
    }

    /** Get start time.
    Defaults to now().
    */
    const Wt::WDateTime& start() const {
        return start_;
    }

    /** Set start time */
    void set_start(const Wt::WDateTime& start) {
        start_ = start;
    }

    /** Get stop time */
    const Wt::WDateTime& stop() const {
        return stop_;
    }

    /** Set stop time */
    void set_stop(const Wt::WDateTime& stop) {
        stop_ = stop;
    }

    /** Get reason of ban */
    const Wt::WString& reason() const {
        return reason_;
    }

    /** Set reason of ban */
    void set_reason(const Wt::WString& reason) {
        reason_ = reason;
    }

    /** Get the user, who created this ban */
    const UserPtr& creator() const {
        return creator_;
    }

    /** Set the user, who created this ban */
    void set_creator(const UserPtr& creator) {
        creator_ = creator;
    }

    /** Return if IP is banned.
    \warning If tApp is not set, false is returned.
    */
    static BanState is_banned(const std::string& ip);

    /** Return if current wApp is banned.
    \warning If tApp is not set, false is returned.
    */
    static BanState am_i_banned();

    /** Run self-checks for planned action.
    Old bans will be disabled.
    */
    void check(Wt::Wc::notify::TaskPtr task);

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        dbo::field(a, ip_, "ip");
        dbo::field(a, start_, "start");
        dbo::field(a, stop_, "stop");
        dbo::field(a, reason_, "reason");
        dbo::belongsTo(a, creator_, "creator");
        dbo::field(a, state_, "state");
    }

private:
    std::string ip_;
    Wt::WDateTime start_;
    Wt::WDateTime stop_;
    Wt::WString reason_;
    UserPtr creator_;
    BanState state_;
};

}

DBO_EXTERN_TEMPLATES(thechess::IpBan);

#endif

