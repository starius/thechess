/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_ELOPLAYER_HPP_
#define THECHESS_MODEL_ELOPLAYER_HPP_

#include <vector>

namespace thechess {

class EloPlayer;

/** Vector of game statictics */
typedef std::vector<EloPlayer*> EloPlayers;
}

namespace thechess {

/** Games statictics.
Number of played, won and failed games, and Elo rating are tracked.

\section Elo rating calculation
\f[ E_A = \frac{1}{1+10^\frac{R_B - R_A}{400}} = \frac{Q_A}{Q_A + Q_B} \f]
\f[ R'_A = R_A + K \cdot (S_A - E_A) \f]
Where:
 - \f$ E_A \f$ -- mean of A's rating increment.
 - \f$ R_A, R_B \f$ -- rating of player A, rating of player B.
 - \f$ R'_A \f$ -- new rating of A.

\f[ Q_A = 10^{R_A/400} \f]
\f[ Q_B = 10^{R_B/400} \f]
\f[
    K = \left\{
    \begin{array}{l l}
    10, & R \ge 2400 \\
    15, & all \ge 30 \\
    25
    \end{array} \right.
\f]
\f[
    S_A = \left\{
    \begin{array}{l l}
    1, & win \\
    0.5, & draw \\
    0, & fail \\
    \end{array} \right.
\f]

This scheme was extended for multiple player games
(e.g. \ref TEAM "team competitions").
In this case \f$E_A = \frac{Q_A}{Q_A + Q_B + Q_C + \dots} \f$.

\ingroup model
*/
class EloPlayer {
public:

#ifndef DOXYGEN_ONLY
    EloPlayer();
#endif

    /** Constructor */
    EloPlayer(bool);

    /** Get Elo rating */
    const int& elo() const {
        return elo_;
    }

    /** Get the number of all played games */
    const int& all() const {
        return all_;
    }

    /** Get the number of wins */
    const int& wins() const {
        return wins_;
    }

    /** Get the number of fails */
    const int& fails() const {
        return fails_;
    }

    /** Get the number of draws */
    int draws() const;

    /** Get value of K */
    float K() const;

    /** Get value of Q */
    float Q() const;

    /** Get E of this player in game with other player */
    float E(const EloPlayer* other) const;

    /** Get E of this player in game with multiple players.
    \param q_sum Sum of Q of all members (including this player)
    */
    float E(float q_sum) const;

    /** Change stats if this player wins other player */
    void win(EloPlayer* loser);

    /** Change stats if this player plays draw with other player */
    void draw(EloPlayer* other);

    /** Change stats after multiple players competition.
    \param winners List of winners.
    \param losers List of losers.
    \note If multiple user competition ended with draw,
        all members are considered winners.
    */
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

