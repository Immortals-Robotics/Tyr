#pragma once

namespace Tyr::Common
{
class Setting;
class Debug;
class Logger;
class Timer;
class ConfigReader;

struct WorldState;
struct RawWorldState;
struct FieldState;
struct RefereeState;

struct Services
{
    static bool initialize();
    static void saveConfig();
    static void shutdown();

    static ConfigReader &configReader()
    {
        return *s_configReader;
    }

    static Setting &setting()
    {
        return *s_setting;
    }

    static Debug &debug()
    {
        return *s_debug;
    }

    static Logger &logger()
    {
        return *s_logger;
    }

    static Timer &global_timer()
    {
        return *s_global_timer;
    }

    static WorldState &worldState()
    {
        return *s_world_state;
    }

    static RawWorldState &rawWorldState()
    {
        return *s_raw_world_state;
    }

    static FieldState &field()
    {
        return *s_field_state;
    }

    static RefereeState &refereeState()
    {
        return *s_referee_state;
    }

private:
    static inline Setting      *s_setting;
    static inline Debug        *s_debug;
    static inline Logger       *s_logger;
    static inline Timer        *s_global_timer;
    static inline ConfigReader *s_configReader;

    static inline WorldState    *s_world_state;
    static inline RawWorldState *s_raw_world_state;
    static inline FieldState    *s_field_state;
    static inline RefereeState  *s_referee_state;
};

static ConfigReader &configReader()
{
    return Services::configReader();
}

static Setting &setting()
{
    return Services::setting();
}

static Debug &debug()
{
    return Services::debug();
}

static Logger &logger()
{
    return Services::logger();
}

static Timer &global_timer()
{
    return Services::global_timer();
}

static WorldState &worldState()
{
    return Services::worldState();
}

static RawWorldState &rawWorldState()
{
    return Services::rawWorldState();
}

static FieldState &field()
{
    return Services::field();
}

static RefereeState &refereeState()
{
    return Services::refereeState();
}
} // namespace Tyr::Common

#include "logging/macros.h"
