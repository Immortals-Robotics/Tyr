#include "vision.h"

namespace Tyr::Vision
{
void Vision::processParam()
{
    double avg   = 0;
    int    count = 0;
    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        if (Common::setting().use_camera[i])
        {
            avg += m_d_frame[i].t_capture();
            count++;
        }
    }

    avg /= count;
    m_state->delta_t_capture = avg - m_state->t_capture;
    m_state->t_capture       = avg;
}
} // namespace Tyr::Vision