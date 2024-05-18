#include "linear.h"

namespace Tyr::Common
{
Linear Linear::calculate(int n, float *x, float *y)
{
    // calculate the averages of arrays x and y
    float xa = 0;
    float ya = 0;
    for (int i = 0; i < n; i++)
    {
        xa += x[i];
        ya += y[i];
    }
    xa /= n;
    ya /= n;

    // calculate auxiliary sums
    float xx = 0;
    float yy = 0;
    float xy = 0;
    for (int i = 0; i < n; i++)
    {
        float tmp_x = x[i] - xa;
        float tmpy  = y[i] - ya;

        xx += tmp_x * tmp_x;
        yy += tmpy * tmpy;
        xy += tmp_x * tmpy;
    }

    Linear result;

    // make sure slope is not infinite
    if (xx < 0.01)
    {
        result.amoodi = true;
        result.xinter = xa;
        return result;
    }

    result.m_b = xy / xx;
    if (std::fabs(result.m_b) > 50)
    {
        result.amoodi = true;
        result.xinter = xa;
        return result;
    }

    result.m_a     = ya - result.m_b * xa;
    result.m_coeff = (std::fabs(yy) == 0) ? 1 : xy / sqrt(xx * yy);

    return result;
}
} // namespace Tyr::Common
