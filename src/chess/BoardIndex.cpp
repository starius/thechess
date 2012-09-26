/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <algorithm>
#include <iterator>

#include "chess/BoardIndex.hpp"
#include "chess/Board.hpp"
#include "chess/Moves.hpp"

namespace thechess {

BoardIndex::BoardIndex():
    max_games_(500)
{ }

BoardIndex::~BoardIndex() {
    clear();
}

void BoardIndex::clear() {
    arr1_.clear();
    arr2_.clear();
    arr3_.clear();
    arr4_.clear();
    arr5_.clear();
    BOOST_FOREACH (const ItemN& item, arrN_) {
        delete[] item.games;
    }
    arrN_.clear();
}

void BoardIndex::add_board(int game, const Board& board) {
    Item1 item;
    item.board = board.to_int();
    item.game0 = game;
    arr1_.push_back(item);
}

void BoardIndex::add_moves(int game, const Moves& moves) {
    for (Moves::const_iterator it = moves.begin(); it != moves.end(); ++it) {
        add_board(game, it.board());
    }
}

template <class Item>
static struct ItemCompare {
    bool operator()(const Item& a, const Item& b) const {
        return a.board < b.board;
    }
};

void BoardIndex::reindex() {
    std::sort(arr1_.begin(), arr1_.end(), ItemCompare<Item1>());
    int arr1_size = arr1_.size();
    int insert_index = 0;
    for (int i = 0; i < arr1_size - 1; i++) { // -1 here for arr1_[i + 1]
        if (arr1_[i + 1].board == arr1_[i].board) {
            int32_t board = arr1_[i].board;
            std::vector<int> games;
            games.push_back(arr1_[i].game0);
            games.push_back(arr1_[i + 1].game0);
            for (i += 2; i < arr1_size && arr1_[i].board == board; i++) {
                games.push_back(arr1_[i].game0);
            }
            // sort and remove duplicates
            std::sort(games.begin(), games.end());
            games.erase(std::unique(games.begin(), games.end()), games.end());
            if (games.size() == 1) {
                arr1_[insert_index] = games[0];
                insert_index++;
            } else if (2 <= games.size() && games.size() <= 5) {
                Item5 item;
                item.board = board;
                item.game0 = games[0];
                item.game1 = games[1];
                if (games.size() >= 3) {
                    item.game2 = games[2];
                }
                if (games.size() >= 4) {
                    item.game3 = games[3];
                }
                if (games.size() >= 5) {
                    item.game4 = games[4];
                }
                if (games.size() == 2) {
                    arr2_.push_back(item);
                }
                if (games.size() == 3) {
                    arr3_.push_back(item);
                }
                if (games.size() == 4) {
                    arr4_.push_back(item);
                }
                if (games.size() == 5) {
                    arr5_.push_back(item);
                }
            } else if (games.size() < max_games_) {
                // large game list
                ItemN item;
                item.board = board;
                item.size = games.size();
                item.games = new int[games.size()];
                std::copy(games.begin(), games.end(), item.games);
                arrN_.push_back(item);
            }
        } else {
            arr1_[insert_index] = arr1_[i].board; // insert_index may be eq i
            insert_index++;
        }
    }
    // cut only used part of arr1_
    arr1_.resize(insert_index);
    // reallocate all vectors
    Arr1(arr1_).swap(arr1_);
    Arr2(arr2_).swap(arr2_);
    Arr3(arr3_).swap(arr3_);
    Arr4(arr4_).swap(arr4_);
    Arr5(arr5_).swap(arr5_);
    ArrN(arrN_).swap(arrN_);
}

void BoardIndex::find_games(const Board& board, std::vector<int>& games) {
    int32_t board_hash = board.to_int();
    const Item1& item1 = std::lower_bound(arr1_.begin(), arr1_.end(),
                                          board_hash, ItemCompare<Item1>());
    if (item1.board == board_hash) {
        games.push_back(item1.game0);
        return;
    }
    const Item2& item2 = std::lower_bound(arr2_.begin(), arr2_.end(),
                                          board_hash, ItemCompare<Item2>());
    if (item2.board == board_hash) {
        games.push_back(item2.game0);
        games.push_back(item2.game1);
        return;
    }
    const Item3& item3 = std::lower_bound(arr3_.begin(), arr3_.end(),
                                          board_hash, ItemCompare<Item3>());
    if (item3.board == board_hash) {
        games.push_back(item3.game0);
        games.push_back(item3.game1);
        games.push_back(item3.game2);
        return;
    }
    const Item4& item4 = std::lower_bound(arr4_.begin(), arr4_.end(),
                                          board_hash, ItemCompare<Item4>());
    if (item4.board == board_hash) {
        games.push_back(item4.game0);
        games.push_back(item4.game1);
        games.push_back(item4.game2);
        games.push_back(item4.game3);
        return;
    }
    const Item5& item5 = std::lower_bound(arr5_.begin(), arr5_.end(),
                                          board_hash, ItemCompare<Item5>());
    if (item5.board == board_hash) {
        games.push_back(item5.game0);
        games.push_back(item5.game1);
        games.push_back(item5.game2);
        games.push_back(item5.game3);
        games.push_back(item5.game4);
        return;
    }
    const ItemN& itemN = std::lower_bound(arrN_.begin(), arrN_.end(),
                                          board_hash, ItemCompare<ItemN>());
    if (itemN.board == board_hash) {
        games.insert(games.end(), itemN.games, itemN.games + itemN.size);
        return;
    }
}

void BoardIndex::find_games(const Moves& moves, std::vector<int>& games,
                            int max_moves) {
}

}

