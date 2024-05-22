#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <numbers>
#include <optional>
#include <ostream>
#include <random>
#include <source_location>
#include <span>
#include <string.h>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/std.h>
#include <imgui.h>
#include <lmdb.h>
#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>
#include <raylib.h>
#include <spdlog/async.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <toml++/toml.hpp>
#include <xxhash.h>

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
#include "math/geom/triangle.h"
#include "math/helpers.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "services.h"
