#include "services.h"

namespace Tyr::Common
{
bool Services::initialize([[maybe_unused]] const Params &t_params)
{
    s_logger = new Logger();

    s_config = new Config::Config(t_params.t_config_path);
    s_config->load();

    s_debug = new Debug::Hub();

    if (!Storage::init(t_params.t_db_path))
    {
        return false;
    }

    s_logger->addDebugSink();

    s_global_timer = new Timer();
    s_global_timer->start();

    s_field_state = new FieldState();

    return true;
}

void Services::shutdown()
{
    delete s_field_state;
    Storage::shutdown();

    delete s_global_timer;

    delete s_config;

    delete s_logger;

    delete s_debug;
}
} // namespace Tyr::Common
