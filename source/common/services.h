#pragma once

namespace Immortals::Common
{
#if FEATURE_DEBUG
namespace Debug
{
class Hub;
}
#endif

namespace Config
{
class Config;
} // namespace Config

#if FEATURE_LOGGING
class Logger;
#endif

class Timer;

struct FieldState;

struct Services
{
    struct Params
    {
#if FEATURE_CONFIG_FILE
        std::filesystem::path t_config_path;
#endif
#if FEATURE_STORAGE
        std::filesystem::path t_db_path;
#endif
    };

    static bool initialize(const Params &t_params);
    static void shutdown();

    static Config::Config &config()
    {
        return *s_config;
    }

#if FEATURE_DEBUG
    static Debug::Hub &debug()
    {
        return *s_debug;
    }
#endif

#if FEATURE_LOGGING
    static Logger &logger()
    {
        return *s_logger;
    }
#endif

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

#if FEATURE_DEBUG
    static inline Debug::Hub *s_debug;
#endif

#if FEATURE_LOGGING
    static inline Logger *s_logger;
#endif

    static inline Timer *s_global_timer;

    static inline FieldState *s_field_state;
};

inline static Config::Config &config()
{
    return Services::config();
}

#if FEATURE_DEBUG
inline static Debug::Hub &debug()
{
    return Services::debug();
}
#endif

#if FEATURE_LOGGING
inline static Logger &logger()
{
    return Services::logger();
}
#endif

inline static Timer &global_timer()
{
    return Services::global_timer();
}

inline static FieldState &field()
{
    return Services::field();
}
} // namespace Immortals::Common

#include "logging/macros.h"
