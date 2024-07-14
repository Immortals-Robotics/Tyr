#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::markManager()
{
    if (!m_is_defending)
    {
        for (auto &pair : m_mark_map)
        {
            pair.second = -1;
        }
        return;
    }

    const Common::Duration start_t = m_timer.time();

    std::vector<std::pair<int, float>> crunchingOpps;

    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        float threat = calculateOppThreat(i, m_ref_state.theirRestart());
        if (threat < 0)
            continue;
        crunchingOpps.push_back(std::make_pair(i, threat));
    }

    std::sort(crunchingOpps.begin(), crunchingOpps.end(),
              [](const std::pair<int, float> &a, const std::pair<int, float> &b) -> bool
              { return a.second > b.second; });

    Common::logDebug("Opps: {}", crunchingOpps.size());
    for (auto it = crunchingOpps.begin(); it != crunchingOpps.end(); ++it)
    {
        Common::logDebug("-- {} : {}", it->first, it->second);
    }

    struct MarkPair
    {
        int   own;
        int   opp;
        float cost;
    };
    std::vector<MarkPair> mark_pairs;
    int                   mpi = 0;
    for (auto it = crunchingOpps.begin(); it != crunchingOpps.end(); ++it)
    {
        for (auto it2 = m_mark_map.begin(); it2 != m_mark_map.end(); ++it2)
        {
            const int   own  = *it2->first;
            const int   opp  = it->first;
            const float cost = calculateMarkCost(own, opp);
            if (cost < 0)
                continue;
            mark_pairs.push_back(MarkPair{own, opp, cost});
        }
        if (++mpi >= m_mark_map.size())
            break;
    }

    const int def_count =
        std::count_if(m_mark_map.begin(), m_mark_map.end(), [this](const auto &pair)
                      { return m_own_robot[*pair.first].state().seen_state != Common::SeenState::CompletelyOut; });

    int markings = std::min(def_count, static_cast<int>(crunchingOpps.size()));

    struct MarkFormation
    {
        std::vector<std::pair<int, int>> pairs;
        float                            TotalCost;

        MarkFormation()
        {
            pairs.reserve(5);
            TotalCost = 0.0f;
        }
    };
    std::vector<MarkFormation> valid_formations;
    valid_formations.reserve(120);

    for (auto it1 = mark_pairs.begin(); it1 != mark_pairs.end(); ++it1)
    {
        if (markings == 1)
        {
            MarkFormation new_formation1;
            new_formation1.pairs.push_back(std::make_pair(it1->own, it1->opp));
            new_formation1.TotalCost += it1->cost;
            valid_formations.push_back(new_formation1);
        }
        for (auto it2 = it1; it2 != mark_pairs.end(); ++it2)
        {
            if (it1->own == it2->own || it1->opp == it2->opp)
                continue;
            if (markings == 2)
            {
                MarkFormation new_formation2;
                new_formation2.pairs.push_back(std::make_pair(it1->own, it1->opp));
                new_formation2.TotalCost += it1->cost;
                new_formation2.pairs.push_back(std::make_pair(it2->own, it2->opp));
                new_formation2.TotalCost += it2->cost;
                valid_formations.push_back(new_formation2);
            }
            for (auto it3 = it2; it3 != mark_pairs.end(); ++it3)
            {
                if (it2->own == it3->own || it2->opp == it3->opp || it1->own == it3->own || it1->opp == it3->opp)
                    continue;
                if (markings == 3)
                {
                    MarkFormation new_formation3;
                    new_formation3.pairs.push_back(std::make_pair(it1->own, it1->opp));
                    new_formation3.TotalCost += it1->cost;
                    new_formation3.pairs.push_back(std::make_pair(it2->own, it2->opp));
                    new_formation3.TotalCost += it2->cost;
                    new_formation3.pairs.push_back(std::make_pair(it3->own, it3->opp));
                    new_formation3.TotalCost += it3->cost;
                    valid_formations.push_back(new_formation3);
                }
                for (auto it4 = it3; it4 != mark_pairs.end(); ++it4)
                {
                    if (it3->own == it4->own || it3->opp == it4->opp || it2->own == it4->own || it2->opp == it4->opp ||
                        it1->own == it4->own || it1->opp == it4->opp)
                        continue;
                    if (markings == 4)
                    {
                        MarkFormation new_formation4;
                        new_formation4.pairs.push_back(std::make_pair(it1->own, it1->opp));
                        new_formation4.TotalCost += it1->cost;
                        new_formation4.pairs.push_back(std::make_pair(it2->own, it2->opp));
                        new_formation4.TotalCost += it2->cost;
                        new_formation4.pairs.push_back(std::make_pair(it3->own, it3->opp));
                        new_formation4.TotalCost += it3->cost;
                        new_formation4.pairs.push_back(std::make_pair(it4->own, it4->opp));
                        new_formation4.TotalCost += it4->cost;
                        valid_formations.push_back(new_formation4);
                    }
                    for (auto it5 = it4; it5 != mark_pairs.end(); ++it5)
                    {
                        if (it4->own == it5->own || it4->opp == it5->opp || it3->own == it5->own ||
                            it3->opp == it5->opp || it2->own == it5->own || it2->opp == it5->opp ||
                            it1->own == it5->own || it1->opp == it5->opp)
                            continue;
                        if (markings == 5)
                        {
                            MarkFormation new_formation5;
                            new_formation5.pairs.push_back(std::make_pair(it1->own, it1->opp));
                            new_formation5.TotalCost += it1->cost;
                            new_formation5.pairs.push_back(std::make_pair(it2->own, it2->opp));
                            new_formation5.TotalCost += it2->cost;
                            new_formation5.pairs.push_back(std::make_pair(it3->own, it3->opp));
                            new_formation5.TotalCost += it3->cost;
                            new_formation5.pairs.push_back(std::make_pair(it4->own, it4->opp));
                            new_formation5.TotalCost += it4->cost;
                            new_formation5.pairs.push_back(std::make_pair(it5->own, it5->opp));
                            new_formation5.TotalCost += it5->cost;
                            valid_formations.push_back(new_formation5);
                        }
                    }
                }
            }
        }
    }

    for (auto it = m_mark_map.begin(); it != m_mark_map.end(); ++it)
        m_mark_map[it->first] = -1;

    const auto best_formation = std::min_element(
        valid_formations.begin(), valid_formations.end(), [](const MarkFormation &a, const MarkFormation &b) -> bool
        { return a.pairs.size() == b.pairs.size() ? a.TotalCost < b.TotalCost : a.pairs.size() > b.pairs.size(); });

    if (best_formation != valid_formations.end())
    {
        Common::logDebug("mark formation with {} pairs and cost of {}", best_formation->pairs.size(),
                         best_formation->TotalCost);

        for (const auto &pair : best_formation->pairs)
        {
            Common::logDebug(" mark pair {} : {}", pair.first, pair.second);

            for (auto it1 = m_mark_map.begin(); it1 != m_mark_map.end(); ++it1)
            {
                if (*it1->first == pair.first)
                {
                    it1->second = pair.second;
                    break;
                }
            }
        }
    }

    const Common::Duration end_t = m_timer.time();

    Common::logTrace("markManager execution time: {}", (end_t - start_t));
}
} // namespace Tyr::Soccer
