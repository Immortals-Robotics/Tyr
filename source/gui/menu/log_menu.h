#pragma once
namespace Tyr::Gui
{
class LogMenu
{
private:
    int m_filter_level = 0;

    void drawTerminal(const Common::Debug::Wrapper &t_wrapper);

public:
    void draw(const Common::Debug::Wrapper &t_wrapper);
    LogMenu()  = default;
    ~LogMenu() = default;
};
} // namespace Tyr::Gui