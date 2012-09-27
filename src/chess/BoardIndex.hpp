/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_BOARD_INDEX_HPP_
#define THECHESS_BOARD_INDEX_HPP_

#include "chess/global.hpp"

namespace thechess {

/** Map from Board to int.
This storage can be used to perform quick search of games, in which this
board state is involved or which are started with this moves.

\warning This class is thread-unsafe

\ingroup chess
*/
class BoardIndex {
public:
    /* Constructor */
    BoardIndex();

    /** Remove all added mappings.
    \warning clear() must be run after reindex() and before adding.
    */
    void clear();

    /** Return max allowed number of games per board.
    Boards with higher games number will be removed by reindex().
    Default value is 500.
    */
    int max_games() const {
        return max_games_;
    }

    /** Set max allowed number of games per board.
    Call this before reindex().
    */
    void set_max_games(int max_games) {
        max_games_ = max_games;
    }

    /** Add board state.
    \warning reindex() must be run between adding and searchig.
    */
    void add_board(int game, const Board& board);

    /** Add all board states (except start) occured in the moves.
    First \c skip_half_moves half-moves are skipped.
    \warning reindex() must be run between adding and searchig.
    */
    void add_moves(int game, const Moves& moves, int skip_half_moves = 5);

    /** Reindex internal data.
    \warning reindex() must be run between adding and searchig.
    */
    void reindex();

    /** Find all games including this board state.
    May produce False Positives.
    \warning reindex() must be run between adding and searchig.
    */
    void search_board(const Board& board, std::vector<int>& games);

    /** Find games starting from this moves.
    May produce False Positives.
    max_boards indicates max number of board states (or half-moves) searched.
    \warning reindex() must be run between adding and searchig.
    */
    void search_moves(const Moves& moves, std::vector<int>& games,
                      int max_boards = 20);

private:
    struct Item1 {
        int32_t board;
        int game0;
    };

    struct Item2 : public Item1 {
        int game1;
    };

    struct Item3 : public Item2 {
        int game2;
    };

    struct Item4 : public Item3 {
        int game3;
    };

    struct Item5 : public Item4 {
        int game4;
    };

    typedef std::vector<Item1> Arr1;
    typedef std::vector<Item2> Arr2;
    typedef std::vector<Item3> Arr3;
    typedef std::vector<Item4> Arr4;
    typedef std::vector<Item5> Arr5;

    struct ItemN {
        int32_t board;
        int size;
        int* games;
    };

    typedef std::vector<ItemN> ArrN;
    typedef std::vector<int> Games;

private:
    int max_games_;
    Arr1 arr1_;
    Arr2 arr2_;
    Arr3 arr3_;
    Arr4 arr4_;
    Arr5 arr5_;
    ArrN arrN_;
    Games gamesN_;
};

}

#endif

