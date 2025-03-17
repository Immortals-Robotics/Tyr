#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <map>
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

#if defined(_WIN32)
#define NOGDI  // All GDI defines and routines
#define NOUSER // All USER defines and routines
#define WIN32_LEAN_AND_MEAN
// needed for thread name setting
#include <Windows.h>
#include <processthreadsapi.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#endif

#include <asio.hpp>
#include <google/protobuf/message_lite.h>

#if defined(_WIN32)
#undef min
#undef max
// raylib uses these names as function parameters
#undef near
#undef far
#endif

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>

#include <google/protobuf/message_lite.h>

#include <lmdb.h>

#include <raylib.h>

// These are defined by raylib and will conflict with proto definitions
#undef YELLOW
#undef BLUE

#include <imgui.h>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/std.h>
#include <spdlog/async.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <toml++/toml.hpp>

#include <xxhash.h>

#include <protos/ssl-game-controller/ssl_gc_referee_message.pb.h>
#include <protos/ssl-vision/messages_robocup_ssl_wrapper.pb.h>

#include <protos/immortals/debug.pb.h>
#include <protos/immortals/referee_state.pb.h>
#include <protos/immortals/world_state.pb.h>
#include <protos/immortals/soccer_state.pb.h>

#include "time/duration.h"
#include "time/time_point.h"
#include "time/timer.h"

#include "services.h"

#include "math/angle.h"
#include "math/geom/circle.h"
#include "math/geom/line.h"
#include "math/geom/line_segment.h"
#include "math/geom/rect.h"
#include "math/geom/robot.h"
#include "math/geom/triangle.h"
#include "math/helpers.h"
#include "math/linear.h"
#include "math/median_filter.h"
#include "math/random.h"
#include "math/vec2.h"
#include "math/vec3.h"

#include "config/base.h"
#include "config/common.h"
#include "config/config.h"
#include "config/network.h"
#include "config/soccer.h"
#include "config/vision.h"

#include "config/file.h"

#include "debugging/thread_name.h"

#include "debugging/color.h"
#include "debugging/draw.h"
#include "debugging/execution_time.h"
#include "debugging/hub.h"

#include "debugging/log.h"

#include "debugging/source_location.h"
#include "debugging/wrapper.h"

#include "logging/debug_sink.h"

#include "logging/logging.h"

#include "network/address.h"
#include "network/udp_client.h"
#include "network/udp_server.h"

#include "network/nng_client.h"
#include "network/nng_message.h"
#include "network/nng_server.h"

#include "state/field/ball_model.h"
#include "state/field/camera_calibration.h"

#include "state/field/field.h"

#include "state/raw/frame.h"

#include "state/raw/ball.h"
#include "state/raw/robot.h"

#include "state/raw/world.h"

#include "state/world/seen_state.h"

#include "state/world/ball.h"
#include "state/world/robot.h"

#include "state/world/world.h"

#include "state/referee/state.h"

#include "state/soccer/robot.h"
#include "state/soccer/state.h"

#include "storage/dumper.h"
#include "storage/storage.h"
