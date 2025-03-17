#pragma once

#include "../common/source/pch.h"
using namespace Immortals;

#include <algorithm>
#include <queue>
#include <deque>
#include <float.h>
#include <fstream>
#include <limits.h>
#include <map>
#include <math.h>
#include <stdlib.h>
#include <Eigen/Dense>
#include <numeric>
#include <optional>

#ifdef PI
#undef PI
#endif
#include <lemon/smart_graph.h>
#include <lemon/network_simplex.h>

#include <protos/immortals/play_book.pb.h>

#include "../vision/kalman/ekf_3d.h"
