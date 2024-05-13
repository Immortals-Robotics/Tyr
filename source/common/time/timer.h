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

#pragma once

namespace Tyr::Common
{
class Timer
{
public:
    void start();
    void end();

    float time();
    float interval();

    float intervalSmooth();

private:
    std::chrono::high_resolution_clock::time_point tv1, tv2;
    float m_interval = 0.0f;
};
} // namespace Tyr::Common
