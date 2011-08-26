/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <boost/assert.hpp>

#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;
#include <Wt/Http/Request>
#include <Wt/Http/Response>

#include "widgets/PgnResource.hpp"
#include "ThechessServer.hpp"

namespace thechess {

PgnResource::PgnResource(GamePtr game, ThechessServer& server, Wt::WObject* p):
    Wt::WResource(p), game_(game), session_(server.pool()) {
    suggestFileName(boost::lexical_cast<std::string>(game_.id()) + ".pgn");
}

PgnResource::PgnResource(ThechessServer& server, Wt::WObject* p):
    Wt::WResource(p), session_(server.pool()) {
    suggestFileName("none.pgn"); // FIXME http://redmine.webtoolkit.eu/issues/920
}

PgnResource::~PgnResource() {
    beingDeleted();
}

void PgnResource::handleRequest(const Wt::Http::Request& request,
                                Wt::Http::Response& response) {
    dbo::Transaction t(session_);
    GamePtr g;
    const std::string* game_id_str = request.getParameter("game");
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
    t.commit();
}

}

