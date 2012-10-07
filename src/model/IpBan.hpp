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

    /** Get IP address */
    const std::string& ip() const {
        return ip_;
    }

    /** Set IP address */
    void set_ip(const std::string& ip) {
        ip_ = ip;
    }

    /** Get if this ban is enabled.
    Enabled by default.
    */
    bool enabled() const {
        return enabled_;
    }

    /** Set if this ban is enabled */
    void set_enabled(bool enabled) {
        enabled_ = enabled;
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
    static bool is_banned(const std::string& ip);

    /** Return if current wApp is banned.
    \warning If tApp is not set, false is returned.
    */
    static bool am_i_banned();

    /** Run self-checks for planned action.
    Old bans will be disabled.
    */
    void check(Wt::Wc::notify::TaskPtr task);

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        dbo::field(a, ip_, "ip");
        dbo::field(a, enabled_, "enabled");
        dbo::field(a, start_, "start");
        dbo::field(a, stop_, "stop");
        dbo::field(a, reason_, "reason");
        dbo::belongsTo(a, creator_, "creator");
    }

private:
    std::string ip_;
    bool enabled_;
    Wt::WDateTime start_;
    Wt::WDateTime stop_;
    Wt::WString reason_;
    UserPtr creator_;
};

}

DBO_EXTERN_TEMPLATES(thechess::IpBan);

#endif

