
#include <algorithm>
#include <boost/assert.hpp>
#include <boost/foreach.hpp>

#include "rand.hpp"
#include "model/StagedCompetition.hpp"
#include "model/Object.hpp"

namespace thechess {
namespace model {

UserPair::UserPair()
{
}

UserPair::UserPair(UserPtr first, UserPtr second)
{
    BOOST_ASSERT(first != second);
    if (first < second)
    {
        first_ = first;
        second_ = second;
    }
    else
    {
        first_ = second;
        second_ = first;
    }
}

bool UserPair::operator<(const UserPair& other) const
{
    return first_ < other.first() || (first_ == other.first() && second_ < other.second());
}

bool UserPair::operator==(const UserPair& other) const
{
    return first_ == other.first() && second_ == other.second();
}

bool UserPair::operator!=(const UserPair& other) const
{
    return !(*this == other);
}

StagedCompetition::StagedCompetition(const Competition* competition):
competition_(competition)
{
    BOOST_ASSERT(competition->type() == STAGED);
    winner_stage_ = ceil(log2(double(competition->members().size()))) + 0.5;
    read_games_();
    read_paires_();
}

void StagedCompetition::process(Competition* competition, Objects& objects)
{
    BOOST_ASSERT(competition->id() == competition_->id());
    if (games_.empty() && competition->state() == Competition::ACTIVE)
        start_competition_();
    join_users_();
    create_games_(competition, objects);
}

void StagedCompetition::read_games_()
{
    BOOST_FOREACH(GamePtr game, competition_->games_vector())
    {
        UserPair pair(game->white(), game->black());
        int stage = game->competition_stage();
        paires_.insert(std::make_pair(stage, pair));
        games_[pair].push_back(game);
    }
}

void StagedCompetition::read_paires_()
{
    BOOST_FOREACH(UserPtr user, competition_->members_vector())
    {
        states_[user] = UNPAIRED;
        stages_[user] = 1; // if he was paired in 0 stage, it will be overwritten
    }
    BOOST_FOREACH(Paires::value_type& stage_and_pair, paires_)
    {
        int stage = stage_and_pair.first;
        UserPair pair = stage_and_pair.second;
        read_pair_(stage, pair);
    }
}

void StagedCompetition::read_pair_(int stage, const UserPair& pair)
{
    states_[pair.first()] = PAIRED;
    states_[pair.second()] = PAIRED;
    stages_[pair.first()] = stage;
    stages_[pair.second()] = stage;
    if (Competition::all_ended(games_[pair]))
    {
        std::vector<UserPtr> winners =
            Competition::winners_of_games(games_[pair]);
        if (winners.size() == 1)
        {
            states_[pair.first()] = LOSER;
            states_[pair.second()] = LOSER;
            UserPtr winner = winners[0];
            winners_[pair] = winner;
            states_[winner] = UNPAIRED;
            stages_[winner] = stage + 1;
            if (stage + 1 == winner_stage_)
            {
                BOOST_ASSERT(!winner_);
                states_[winner] = WINNER;
                winner_ = winner;
            }
        }
    }
}

void StagedCompetition::start_competition_()
{
    std::vector<UserPtr> members = competition_->members_vector();
    std::random_shuffle(members.begin(), members.end(), random::rand_for_shuffle);
    int members_size = members.size();
    int max_pow2 = pow(2, floor(log2(members_size))) + 0.5;
    int pairs = members_size - max_pow2 || members_size / 2;
    for (int i = 0; i < pairs; i++)
    {
        UserPtr first = members[2*i];
        UserPtr second = members[2*i+1];
        states_[first] = PAIRED;
        states_[second] = PAIRED;
        stages_[first] = 0;
        stages_[second] = 0;
        paires_.insert(std::make_pair(0, UserPair(first, second)));
    }
}


void StagedCompetition::join_users_()
{
    typedef std::vector<UserPtr> UserVector;
    typedef std::map<int, UserVector> Unpaired;
    Unpaired unpaired;
    BOOST_FOREACH(States::value_type& user_and_state, states_)
        if (user_and_state.second == UNPAIRED)
            unpaired[stages_[user_and_state.first]].push_back(user_and_state.first);
    BOOST_FOREACH(Unpaired::value_type& stage_and_users, unpaired)
    {
        int stage = stage_and_users.first;
        UserVector& users = stage_and_users.second;
        std::random_shuffle(users.begin(), users.end(), random::rand_for_shuffle);
        while (users.size() >= 2)
        {
            UserPtr first = users.back();
            users.pop_back();
            UserPtr second = users.back();
            users.pop_back();
            states_[first] = PAIRED;
            states_[second] = PAIRED;
            paires_.insert(std::make_pair(stage, UserPair(first, second)));
        }
    }
}

void StagedCompetition::create_games_(Competition* competition, Objects& objects)
{
    BOOST_FOREACH(Paires::value_type& stage_and_pair, paires_)
    {
        int stage = stage_and_pair.first;
        UserPair pair = stage_and_pair.second;
        if (!winners_[pair] && Competition::all_ended(games_[pair]))
        {
            bool no_draw = static_cast<int>(games_[pair].size()) ==
                competition->min_substages() + stage*competition->increment_substages();
            int n = no_draw ? 1 : 2;
            for (int i = 0; i < n; i++)
            {
                UserPtr white = i ? pair.first() : pair.second();
                UserPtr black = i ? pair.second() : pair.first();
                GamePtr game = competition->create_game_(white, black, stage, no_draw);
                games_[pair].push_back(game);
                objects.push_back(Object(GameObject, game.id()));
            }
            objects.push_back(Object(UserObject, pair.first().id()));
            objects.push_back(Object(UserObject, pair.second().id()));
        }
    }
}

}
}


