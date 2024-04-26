#pragma once
#include "setting.h"
#include "logging/logging.h"
#include "timer/timer.h"

namespace Tyr::Common
{
struct Services
{
    static void initialize()
    {
        const ConfigReader config("config.toml");
        s_setting = new Setting();
        s_setting->load(config.getRoot());

        s_logger = new Logger();

        s_global_timer = new Timer();
        s_global_timer->start();
    }

    static void shutdown()
    {
        delete s_setting;
        delete s_logger;
        delete s_global_timer;
    }

    static const Setting &setting()
    {
        return *s_setting;
    }

    static Timer &global_timer()
    {
        return *s_global_timer;
    }

private:
    static inline Setting* s_setting;
    static inline Logger*  s_logger;
    static inline Timer*   s_global_timer;
};

static const Setting &setting()
{
    return Services::setting();
}

static Timer &global_timer()
{
    return Services::global_timer();
}
} // namespace Tyr::Common
