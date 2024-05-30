#pragma once

namespace Tyr::Gui
{
class StatusBar
{
public:
    void draw(const Common::Debug::Wrapper &t_wrapper);

private:
    std::unordered_map<std::string, Common::MedianFilter<Common::Duration, 30>> m_interval_filters;
    std::unordered_map<std::string, Common::MedianFilter<Common::Duration, 30>> m_duration_filters;
};
} // namespace Tyr::Gui
