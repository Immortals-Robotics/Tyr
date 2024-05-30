#pragma once

namespace Tyr::Gui
{
class StatusBar
{
public:
    void draw(const Common::Debug::Wrapper &t_wrapper);

private:
    using Filter = Common::MedianFilter<Common::Duration, 30>;
    std::unordered_map<std::string, Filter> m_interval_filters;
    std::unordered_map<std::string, Filter> m_duration_filters;
};
} // namespace Tyr::Gui
