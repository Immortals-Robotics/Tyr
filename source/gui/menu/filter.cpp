#include "filter.h"

namespace Tyr::Gui
{
void FilterMenu::draw()
{
    if (ImGui::Checkbox("All", &m_all_filter))
    {
        m_all_change = true;
        if (!m_all_filter)
        {
            m_nodes.clear();
            m_map.m_map.clear();
        }
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    for (const auto &node : m_nodes)
    {
        auto force_active = false;
        if (m_all_change)
        {
            force_active = true;
            node->active = m_all_filter;
        }
        if (ImGui::Checkbox(node->name.c_str(), &node->active))
        {
            force_active = true;
        }
        ImGui::Indent();
        for (const auto &child : node->children)
        {
            if (force_active)
            {
                child->active = node->active;
            }
            if (ImGui::Checkbox(child->name.c_str(), &child->active) && child->active)
            {
                node->active = true;
            }
        }
        ImGui::Unindent();
        ImGui::Separator();
        ImGui::Spacing();
    }

    if (m_nodes.size())
    {
        m_all_change = false;
    }
}

void FilterMenu::pushToFilters(const Common::Debug::SourceLocation &t_source)
{
    const std::filesystem::path file_path{t_source.file};
    const std::string           filename = file_path.filename().string();

    if (!m_map.m_map.contains(filename))
    {
        auto node = std::make_unique<Node>(filename);

        Node *node_ptr = node.get();
        m_nodes.emplace_back(std::move(node));
        m_map.m_map[filename] = node_ptr;
    }
    auto function_name = FilterMenu::extractFunctionName(t_source.function);
    m_map.m_map[filename]->addChild(function_name);
}

void FilterMenu::feedDebug(const Common::Debug::Wrapper &t_wrapper)
{
    for (const auto &log : t_wrapper.logs)
        pushToFilters(log.source);
    for (const auto &draw : t_wrapper.draws)
        pushToFilters(draw.source);
}

void FilterMenu::Node::addChild(const std::string &t_child)
{
    const auto it = std::find_if(children.begin(), children.end(),
                                 [&t_child](const std::unique_ptr<Node> &child) { return child->name == t_child; });
    if (it == children.end())
    {
        children.push_back(std::make_unique<Node>(t_child));
    }
}
} // namespace Tyr::Gui
