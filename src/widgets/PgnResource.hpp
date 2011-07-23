/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_PGN_RESOURCE_HPP_
#define THECHESS_WIDGETS_PGN_RESOURCE_HPP_

#include <Wt/WResource>

#include "model/Game.hpp"
#include "ThechessSession.hpp"

namespace thechess {
class ThechessServer;
namespace widgets {

class PgnResource : public Wt::WResource
{
public:
    PgnResource(model::GamePtr game, ThechessServer& server, Wt::WObject* p=0);
    PgnResource(ThechessServer& server, Wt::WObject* p=0);
    ~PgnResource();

    virtual void handleRequest(const Wt::Http::Request& request,
        Wt::Http::Response& response);

private:
    model::GamePtr game_;
    ThechessSession session_;
};

}
}

#endif

