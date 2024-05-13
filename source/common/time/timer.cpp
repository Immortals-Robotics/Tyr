/*========================================================================
 timer.h : Simple C++ wrapper for making interval timers
 ------------------------------------------------------------------------
 Copyright (C) 1999-2002  James R. Bruce
 School of Computer Science, Carnegie Mellon University
 ------------------------------------------------------------------------
 This software is distributed under the GNU General Public License,
 version 2.  If you do not have a copy of this licence, visit
 www.gnu.org, or write: Free Software Foundation, 59 Temple Place,
 Suite 330 Boston, MA 02111-1307 USA.  This program is distributed
 in the hope that it will be useful, but WITHOUT ANY WARRANTY,
 including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 ========================================================================*/

#include "timer.h"

namespace Tyr::Common
{
void Timer::start()
{
    tv1 = std::chrono::high_resolution_clock::now();
}

void Timer::end()
{
    tv2 = std::chrono::high_resolution_clock::now();
}

float Timer::time()
{
    end();
    const auto time_span = std::chrono::duration_cast<std::chrono::duration<float>>(tv2 - tv1);
    return time_span.count();
}

float Timer::interval()
{
    float t;
    //	tv2 = high_resolution_clock::now();
    t   = time();
    tv1 = tv2;
    return (t);
}
float Timer::intervalSmooth()
{
    static constexpr float kAlpha = 0.1f;

    m_interval = (kAlpha * interval()) + (1.0f - kAlpha) * m_interval;
    return m_interval;
}
} // namespace Tyr::Common
