#include "linear.h"

namespace Tyr::Common
{
void Linear::calculate(int n, float *x, float *y)
{
    // calculate the averages of arrays x and y
    double xa = 0, ya = 0;
    for (int i = 0; i < n; i++)
    {
        xa += x[i];
        ya += y[i];
    }
    xa /= n;
    ya /= n;

    // calculate auxiliary sums
    double xx = 0, yy = 0, xy = 0;
    for (int i = 0; i < n; i++)
    {
        double tmpx = x[i] - xa, tmpy = y[i] - ya;
        xx += tmpx * tmpx;
        yy += tmpy * tmpy;
        xy += tmpx * tmpy;
    }

    // calculate regression line parameters
    amoodi  = false;
    xinter  = 0;
    m_a     = 0;
    m_b     = 0;
    m_coeff = 0;
    // make sure slope is not infinite
    if (xx < 0.01)
    {
        amoodi = true;
        xinter = xa;
        return;
    }

    m_b = xy / xx;
    if (std::fabs(m_b) > 50)
    {
        amoodi = true;
        xinter = xa;
        return;
    }
    m_a     = ya - m_b * xa;
    m_coeff = (std::fabs(yy) == 0) ? 1 : xy / sqrt(xx * yy);
}
} // namespace Tyr::Common
