#include "linear.h"

namespace Tyr::Common
{
Linear Linear::calculate(const int t_n, const float *t_x, const float *t_y)
{
    // calculate the averages of arrays x and y
    float xa = 0;
    float ya = 0;
    for (int i = 0; i < t_n; i++)
    {
        xa += t_x[i];
        ya += t_y[i];
    }
    xa /= t_n;
    ya /= t_n;

    // calculate auxiliary sums
    float xx = 0;
    float yy = 0;
    float xy = 0;
    for (int i = 0; i < t_n; i++)
    {
        const float tmp_x = t_x[i] - xa;
        const float tmpy  = t_y[i] - ya;

        xx += tmp_x * tmp_x;
        yy += tmpy * tmpy;
        xy += tmp_x * tmpy;
    }

    Linear result;

    // make sure slope is not infinite
    if (xx < 0.01)
    {
        result.m_amoodi = true;
        result.m_xinter = xa;
        return result;
    }

    result.m_b = xy / xx;
    if (std::fabs(result.m_b) > 50)
    {
        result.m_amoodi = true;
        result.m_xinter = xa;
        return result;
    }

    result.m_a     = ya - result.m_b * xa;
    result.m_coeff = (std::fabs(yy) == 0) ? 1 : xy / sqrt(xx * yy);

    return result;
}
} // namespace Tyr::Common
