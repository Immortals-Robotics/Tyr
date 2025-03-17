#pragma once

#if FEATURE_LOGGING
#include "logging.h"
#endif

#if defined(__clang__) || defined(__GNUC__)
#define FORCEINLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE
#endif

namespace Immortals::Common
{
#if FEATURE_LOGGING
#define LOG_MACRO(fn, lvl)                                                                                             \
    template <typename... Args>                                                                                        \
    struct fn                                                                                                          \
    {                                                                                                                  \
        FORCEINLINE fn(spdlog::format_string_t<Args...> format, Args &&...args,                                        \
                       std::source_location             source = std::source_location::current())                      \
        {                                                                                                              \
            logger().log(source, spdlog::level::lvl, format, std::forward<Args>(args)...);                             \
        }                                                                                                              \
    };                                                                                                                 \
    template <typename... Args>                                                                                        \
    fn(spdlog::format_string_t<Args...> format, Args &&...args) -> fn<Args...>;
#else
#define LOG_MACRO(fn, lvl)                                                                                             \
    template <typename... Args>                                                                                        \
    FORCEINLINE void fn(const char *format, Args &&...args)                                                                 \
    {                                                                                                                  \
        std::printf(format, std::forward<Args>(args)...);                                                              \
    }
#endif

LOG_MACRO(logTrace, trace);
LOG_MACRO(logDebug, debug);
LOG_MACRO(logInfo, info);
LOG_MACRO(logWarning, warn);
LOG_MACRO(logError, err);
LOG_MACRO(logCritical, critical);

} // namespace Immortals::Common
