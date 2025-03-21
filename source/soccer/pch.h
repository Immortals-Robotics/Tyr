#pragma once

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
#include <concepts>

#ifdef PI
#undef PI
#endif
#include <lemon/smart_graph.h>
#include <lemon/network_simplex.h>

#include <protos/immortals/play_book.pb.h>

#include "globals/state.h"
#include "globals/field.h"
