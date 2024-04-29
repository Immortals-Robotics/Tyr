#pragma once

#include <atomic>
#include <cmath>
#include <list>
#include <mutex>
#include <regex>
#include <span>
#include <string>
#include <thread>

#include <raylib.h>
#include <raymath.h>

#include <imgui.h>
#include <rlimgui/rlImGui.h>

#include <immortals/Imm_wrapper.pb.h>
#include <ssl-vision/messages_robocup_ssl_geometry_legacy.pb.h>
#include <ssl-vision/messages_robocup_ssl_wrapper.pb.h>

#include <services.h>
#include <network/udp_client.h>
#include <math/geom/circle.h>
#include <math/geom/line_segment.h>
#include <math/geom/line.h>
#include <math/geom/rect.h>
