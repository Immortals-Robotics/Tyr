#include "services.h"

#include "debugging/debug.h"
#include "logging/logging.h"
#include "setting.h"
#include "state/referee.h"
#include "state/world.h"
#include "storage/storage.h"
#include "timer/timer.h"

namespace Tyr::Common
{
void Services::initialize()
{
    s_debug = new Debug();

    s_logger = new Logger();

    s_configReader = new ConfigReader("config.toml");

    s_setting = new Setting();
    s_setting->load(s_configReader->getRoot());

    Storage::init(std::filesystem::path(LOG_DIR) / "db");

    s_debug->initStorage("debug");

    s_global_timer = new Timer();
    s_global_timer->start();

    s_world_state     = new WorldState();
    s_raw_world_state = new RawWorldState();
    s_field_state     = new FieldState();
    s_referee_state   = new RefereeState();
}

void Services::saveConfig()
{
    s_configReader->save(s_setting->getConfigTable());
}

void Services::shutdown()
{
    delete s_world_state;
    delete s_raw_world_state;
    delete s_field_state;
    delete s_referee_state;

    Storage::shutdown();

    delete s_global_timer;
    delete s_setting;
    delete s_logger;
    delete s_debug;
}
} // namespace Tyr::Common