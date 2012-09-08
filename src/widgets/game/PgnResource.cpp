/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include <Wt/Dbo/Transaction>
#include <Wt/Http/Request>
#include <Wt/Http/Response>

#include "widgets/game/PgnResource.hpp"
#include "Server.hpp"

namespace thechess {

PgnResource::PgnResource(const GamePtr& game, Server& server, Wt::WObject* p):
    Wt::WResource(p), game_(game), session_(server.pool()) {
    suggestFileName(boost::lexical_cast<std::string>(game_.id()) + ".pgn");
}

PgnResource::PgnResource(Server& server, Wt::WObject* p):
    Wt::WResource(p), session_(server.pool()) {
}

PgnResource::~PgnResource() {
    beingDeleted();
}

void PgnResource::handleRequest(const Wt::Http::Request& request,
                                Wt::Http::Response& response) {
    dbo::Transaction t(session_);
    const std::string* game_id_str = request.getParameter("game");
    if (game_id_str && *game_id_str == "all") {
        suggestFileName("all.pgn");
        // TODO frequency check
        Games games = session_.find<Game>();
        BOOST_FOREACH (GamePtr g, games) {
            g->pgn(response.out());
            response.out() << std::endl;
        }
        return;
    }
    GamePtr g;
    if (game_id_str) {
        suggestFileName((*game_id_str) + ".pgn");
        try {
            int game_id = boost::lexical_cast<int>(*game_id_str);
            g = session_.load<Game>(game_id);
        } catch (...)
        { }
    } else if (game_) {
        g = game_;
        g.reread();
    }
    if (!g) {
        suggestFileName("none.pgn");
        response.out() << "No such game" << std::endl;
        return;
    }
    g->pgn(response.out());
}

}

