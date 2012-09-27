/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include "SharedBoardIndex.hpp"
#include "Session.hpp"
#include "Server.hpp"
#include "model/all.hpp"

namespace thechess {

void SharedBoardIndex::rebuild(Session& session) {
    boost::upgrade_lock<boost::shared_mutex> lock(mutex_);
    boost::upgrade_to_unique_lock<boost::shared_mutex> unique_lock(lock);
    index_.clear();
    dbo::Transaction t(session);
    Games games = session.find<Game>();
    BOOST_FOREACH (GamePtr game, games) {
        if (game.id() % 1000 == 0) {
            std::cerr << "Index game: " << game.id() << std::endl;
        }
        index_.add_moves(game.id(), game->moves());
    }
    index_.reindex();
}

void SharedBoardIndex::search_board(Session& session, const Board& board,
                                    std::vector<int>& games) {
    boost::shared_lock<boost::shared_mutex> lock(mutex_, boost::try_to_lock);
    if (lock) {
        dbo::Transaction t(session);
        std::vector<int> candidates;
        index_.search_board(board, candidates);
        BOOST_FOREACH (int game_id, candidates) {
            GamePtr game = session.load<Game>(game_id);
            if (game->moves().has_board(board)) {
                games.push_back(game_id);
            }
        }
    }
}

void SharedBoardIndex::search_moves(Session& session, const Moves& moves,
                                    std::vector<int>& games, int max_boards,
                                    int stop_games) {
    boost::shared_lock<boost::shared_mutex> lock(mutex_, boost::try_to_lock);
    if (lock) {
        dbo::Transaction t(session);
        std::vector<int> candidates;
        index_.search_moves(moves, candidates);
        BOOST_FOREACH (int game_id, candidates) {
            GamePtr game = session.load<Game>(game_id);
            if (game->moves().starts_with(moves)) {
                games.push_back(game_id);
            }
        }
    }
}

SharedBoardIndex* SharedBoardIndex::instance() {
    return &Server::instance()->shared_board_index();
}

}

