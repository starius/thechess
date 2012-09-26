/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstdio>
#include <fstream>
#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include <Wt/Dbo/Transaction>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Wc/util.hpp>

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
    response.setMimeType("application/x-chess-pgn");
    dbo::Transaction t(session_);
    const std::string* game_id_str = request.getParameter("game");
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
    if (!g->is_ended()) {
        suggestFileName("none.pgn");
        response.out() << "Game is not ended" << std::endl;
        return;
    }
    g->pgn(response.out());
}

AllPgnResource::AllPgnResource(Server& server, Wt::WObject* p):
    Wt::WFileResource(p) {
    setMimeType("application/x-chess-pgn");
    suggestFileName("all.pgn");
}

AllPgnResource::~AllPgnResource() {
    beingDeleted();
    remove(fileName().c_str());
}

const Td REBUILD_FREQ = HOUR;

void AllPgnResource::handleRequest(const Wt::Http::Request& request,
                                   Wt::Http::Response& response) {
    if (fileName().empty()) {
        setFileName(Wt::Wc::unique_filename());
    }
    if (!last_rebuild_.isValid() || now() - last_rebuild_ > REBUILD_FREQ) {
        dbo::SqlConnection* con = Session::new_connection(*Options::instance());
        boost::scoped_ptr<dbo::SqlConnection> ptr(con);
        Session s(*con);
        last_rebuild_ = now();
        std::ofstream file_stream(fileName().c_str());
        dbo::Transaction t(s);
        Games games = s.find<Game>().where("state >= ?").bind(Game::MIN_ENDED);
        BOOST_FOREACH (GamePtr g, games) {
            g->pgn(file_stream);
            file_stream << std::endl;
        }
        setChanged();
    }
    Wt::WFileResource::handleRequest(request, response);
}

}

