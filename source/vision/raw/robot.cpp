#include "raw.h"

namespace Tyr::Vision
{
void Raw::processRobots()
{
    mergeRobots(Common::TeamColor::Yellow);
    mergeRobots(Common::TeamColor::Blue);
}

void Raw::mergeRobots(const Common::TeamColor t_color)
{
    auto &robots = t_color == Common::TeamColor::Yellow ? m_state.yellow_robots : m_state.blue_robots;
    int   num    = robots.size();

    int robots_num = 0;
    for (int i = 0; i < num; i++)
    {
        for (int j = i + 1; j < num; j++)
        {
            if (robots[i].vision_id == robots[j].vision_id &&
                robots[i].position.distanceTo(robots[j].position) < Common::setting().merge_distance)
            {
                robots[i].position = (robots[i].position + robots[j].position) / 2.0f;
                robots[i].angle    = Common::Angle::average(robots[i].angle, robots[j].angle);

                std::swap(robots[j], robots[num - 1]);
                num--;

                j--;
            }
        }
        robots_num++;
    }

    robots.resize(robots_num);
}
} // namespace Tyr::Vision