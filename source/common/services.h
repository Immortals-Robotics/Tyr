#pragma once

namespace Tyr::Common
{
namespace Debug
{
class Hub;
}

namespace Config
{
class Config;
} // namespace Config

class Logger;
class Timer;

struct FieldState;

struct Services
{
    static bool initialize();
    static void shutdown();

    static Config::Config &config()
    {
        return *s_config;
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
    static inline Config::Config *s_config;
    static inline Debug::Hub     *s_debug;
    static inline Logger         *s_logger;
    static inline Timer          *s_global_timer;

    static inline FieldState *s_field_state;
};

inline static Config::Config &config()
{
    return Services::config();
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
