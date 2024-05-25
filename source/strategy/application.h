#pragma once

namespace Tyr::Strategy
{
class Application : public QApplication
{
public:
    Application(int &argc, char **argv) : QApplication(argc, argv)
    {
        const Common::Services::Params params{.t_config_path = std::filesystem::path{DATA_DIR} / "config.toml",
                                              .t_db_path     = std::filesystem::path{LOG_DIR} / "db"};
        Common::Services::initialize(params);
    }

    ~Application() override
    {
        Common::Services::shutdown();
    }
};
} // namespace Tyr::Strategy
