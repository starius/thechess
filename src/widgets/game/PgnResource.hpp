/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_PGN_RESOURCE_HPP_
#define THECHESS_WIDGETS_PGN_RESOURCE_HPP_

#include <Wt/WResource>
#include <Wt/WFileResource>

#include "model/all.hpp"
#include "thechess-global.hpp"
#include "Session.hpp"

namespace thechess {

class PgnResource : public Wt::WResource {
public:
    PgnResource(const GamePtr& game, Server& server, Wt::WObject* p = 0);
    PgnResource(Server& server, Wt::WObject* p = 0);
    ~PgnResource();

    virtual void handleRequest(const Wt::Http::Request& request,
                               Wt::Http::Response& response);

private:
    GamePtr game_;
    Session session_;
};

class AllPgnResource : public Wt::WFileResource {
public:
    AllPgnResource(Server& server, Wt::WObject* p = 0);
    ~AllPgnResource();

    virtual void handleRequest(const Wt::Http::Request& request,
                               Wt::Http::Response& response);

private:
    Wt::WDateTime last_rebuild_;
};

}

#endif

