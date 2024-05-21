#pragma once

namespace Tyr::Common
{
namespace Debug
{
class Hub;
}

class Setting;
class Logger;
class Timer;
class ConfigReader;

struct FieldState;

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

    static Debug::Hub &debug()
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

    static FieldState &field()
    {
        return *s_field_state;
    }

private:
    static inline Setting      *s_setting;
    static inline Debug::Hub   *s_debug;
    static inline Logger       *s_logger;
    static inline Timer        *s_global_timer;
    static inline ConfigReader *s_configReader;

    static inline FieldState *s_field_state;
};

inline static ConfigReader &configReader()
{
    return Services::configReader();
}

inline static Setting &setting()
{
    return Services::setting();
}

inline static Debug::Hub &debug()
{
    return Services::debug();
}

inline static Logger &logger()
{
    return Services::logger();
}

inline static Timer &global_timer()
{
    return Services::global_timer();
}

inline static FieldState &field()
{
    return Services::field();
}
} // namespace Tyr::Common

#include "logging/macros.h"
