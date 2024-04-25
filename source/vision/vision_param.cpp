#include "Vision.h"

namespace Loki::Vision
{
void VisionModule::ProcessParam(Common::WorldState *state)
{
    double avg   = 0;
    int    count = 0;
    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        if (Common::setting().use_camera[i])
        {
            avg += frame[i].t_capture();
            count++;
        }
    }

    avg /= count;
    state->delta_t_capture = avg - state->t_capture;
    state->t_capture       = avg;
}
} // namespace Loki::Vision