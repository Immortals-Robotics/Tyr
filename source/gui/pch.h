#pragma once

#include <string>
#include <mutex>
#include <span>
#include <list>
#include <thread>
#include <atomic>

#include <raylib.h>
#include <raymath.h>

#include <imgui.h>
#include <rlimgui/rlImGui.h>

#include <ssl-vision/messages_robocup_ssl_geometry_legacy.pb.h>
#include <ssl-vision/messages_robocup_ssl_wrapper.pb.h>
#include <immortals/Imm_wrapper.pb.h>

#include <network/udp_client.h>
