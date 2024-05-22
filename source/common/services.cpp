#include "services.h"

#include "config/config.h"
#include "debugging/hub.h"
#include "logging/logging.h"
#include "state/world.h"
#include "storage/storage.h"
#include "time/timer.h"

namespace Tyr::Common
{
bool Services::initialize()
{
    s_logger = new Logger();

    s_config = new Config::Config("config.toml");
    s_config->load();

    s_debug = new Debug::Hub();

    if (!Storage::init(std::filesystem::path(LOG_DIR) / "db"))
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
