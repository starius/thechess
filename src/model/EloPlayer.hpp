/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_ELOPLAYER_HPP_
#define THECHESS_MODEL_ELOPLAYER_HPP_

#include <vector>

namespace thechess {
class EloPlayer;
typedef std::vector<EloPlayer*> EloPlayers;
}

namespace thechess {

class EloPlayer {
public:
    EloPlayer();
    EloPlayer(bool);

    const int& elo() const {
        return elo_;
    }
    const int& all() const {
        return all_;
    }
    const int& wins() const {
        return wins_;
    }
    const int& fails() const {
        return fails_;
    }
    int draws() const;

    float K() const;
    float Q() const;
    float E(const EloPlayer* other) const;
    float E(float q_sum) const;

    void win(EloPlayer* loser);
    void draw(EloPlayer* other);
    static void multiple(const EloPlayers& winners, const EloPlayers& losers);

private:
    int elo_;
    int all_;
    int wins_;
    int fails_;

    void apply_result_(float q_sum, float S);
};

}

#endif

