#pragma once
namespace Tyr::Gui
{
const std::map<Tyr::Common::Debug::Log::Level, std::string> log_level_map{
    {Tyr::Common::Debug::Log::Level::Critical, "critical"}, {Tyr::Common::Debug::Log::Level::Trace, "trace"},
    {Tyr::Common::Debug::Log::Level::Debug, "debug"},       {Tyr::Common::Debug::Log::Level::Info, "info"},
    {Tyr::Common::Debug::Log::Level::Warning, "warning"},   {Tyr::Common::Debug::Log::Level::Error, "error"},
};
class LogMenu
{
private:
    int m_filter_level = 0;

    void drawTerminal(const Common::Debug::Wrapper &t_wrapper);
    bool applyFilter(const Tyr::Common::Debug::Log::Level t_level);

public:
    void draw(const Common::Debug::Wrapper &t_wrapper);
    LogMenu()  = default;
    ~LogMenu() = default;
};
} // namespace Tyr::Gui