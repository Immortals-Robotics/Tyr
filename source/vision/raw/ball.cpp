#include "raw.h"

namespace Tyr::Vision
{
void Raw::processBalls()
{
    int balls_num = 0;

    auto &balls = m_state.balls;

    int num = balls.size();

    for (int i = 0; i < num; i++)
    {
        for (int j = i + 1; j < num; j++)
        {
            if (balls[i].position.distanceTo(balls[j].position) < Common::setting().merge_distance)
            {
                balls[i].position = (balls[i].position + balls[j].position) / 2.0f;

                balls[j] = balls[num - 1];
                num--;

                j--;
            }
        }
        balls_num++;
    }

    balls.resize(balls_num);
}
} // namespace Tyr::Vision
