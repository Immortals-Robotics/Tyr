#pragma once

namespace Tyr::Strategy
{
class Application : public QApplication
{
public:
    Application(int &argc, char **argv) : QApplication(argc, argv)
    {
        Common::Services::initialize();
    }

    ~Application() override
    {
        Common::Services::shutdown();
    }
};
} // namespace Tyr::Strategy
