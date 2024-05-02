#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <deque>
#include <filesystem>
#include <memory>
#include <numbers>
#include <optional>
#include <ostream>
#include <random>
#include <source_location>
#include <span>
#include <string_view>
#include <vector>
#include <fstream>

#include <fmt/format.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <toml++/toml.hpp>

#if defined(_WIN32)
#define NOGDI  // All GDI defines and routines
#define NOUSER // All USER defines and routines
#endif

#include <asio.hpp>

#if defined(_WIN32) // raylib uses these names as function parameters
#undef near
#undef far
#endif

#include <google/protobuf/message_lite.h>

#include "math/angle.h"
#include "math/geom/circle.h"
#include "math/geom/line.h"
#include "math/geom/line_segment.h"
#include "math/geom/rect.h"
#include "math/helpers.h"
#include "math/vector.h"
#include "services.h"
#