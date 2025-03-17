#include "services.h"

namespace Immortals::Common
{
bool Services::initialize([[maybe_unused]] const Params &t_params)
{
#if FEATURE_LOGGING
    s_logger = new Logger();
#endif

#if FEATURE_CONFIG_FILE
    s_config = new Config::Config(t_params.t_config_path);
    s_config->load();
#else
    s_config = new Config::Config();
#endif

#if FEATURE_DEBUG
    s_debug = new Debug::Hub();
#endif

#if FEATURE_STORAGE
    if (!Storage::init(t_params.t_db_path))
    {
        return false;
    }
#endif

#if FEATURE_LOGGING && FEATURE_DEBUG
    s_logger->addDebugSink();
#endif

    s_global_timer = new Timer();
    s_global_timer->start();

    s_field_state = new FieldState();

    return true;
}

void Services::shutdown()
{
    delete s_field_state;
#if FEATURE_STORAGE
    Storage::shutdown();
#endif

    delete s_global_timer;

    delete s_config;

#if FEATURE_LOGGING
    delete s_logger;
#endif

#if FEATURE_DEBUG
    delete s_debug;
#endif
}
} // namespace Immortals::Common
