#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-identifier-naming"
#include "config_menu.h"
namespace Tyr::Gui
{
void ConfigMenu::drawFilterTab()
{
    if (ImGui::Checkbox("All", &m_all_filter))
    {
        m_all_change = true;
        if (!m_all_filter)
        {
            m_filter_tree.clear();
            m_node_map.clear();
        }
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    for (const auto &node : m_filter_tree.nodes)
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
        for (const auto &child : node->children)
        {
            ImGui::SetCursorPosX(30);
            if (force_active)
            {
                child->active = node->active;
            }
            if (ImGui::Checkbox(child->name.c_str(), &child->active) && child->active)
            {
                node->active = true;
            }
        }
        ImGui::Separator();
        ImGui::Spacing();
    }
    if (m_filter_tree.nodes.size())
    {
        m_all_change = false;
    }
}

template <typename T>
void ConfigMenu::pushToFilters(const std::vector<T> &t_input)
{
    for (const auto &in : t_input)
    {
        const std::filesystem::path file_path{in.source.file};
        const std::string           filename = file_path.filename().string();

        auto it = m_node_map.find(filename);
        if (it == m_node_map.end())
        {
            auto node = std::make_unique<FilterNode>(filename);

            FilterNode *node_ptr = node.get();
            m_filter_tree.addNode(std::move(node));
            m_node_map[filename] = node_ptr;
        }
        auto function_name = ConfigMenu::extractFunctionName(in.source.function);
        m_node_map[filename]->addChild(function_name);
    }
}

void ConfigMenu::feedDebug(const Common::Debug::Wrapper &t_wrapper)
{
    pushToFilters(t_wrapper.logs);
    pushToFilters(t_wrapper.draws);
}

ConfigMenu::FilterNode::FilterNode(std::string t_name) : name(t_name)
{}

void ConfigMenu::FilterNode::addChild(const std::string t_child)
{
    auto it = std::find_if(children.begin(), children.end(),
                           [&t_child](const std::unique_ptr<FilterNode> &child) { return child->name == t_child; });
    if (it == children.end())
    {
        children.push_back(std::make_unique<FilterNode>(t_child));
    }
}

void ConfigMenu::FilterTree::addNode(std::unique_ptr<FilterNode> t_node)
{
    nodes.push_back(std::move(t_node));
}

void ConfigMenu::FilterTree::clear()
{
    nodes.clear();
}

} // namespace Tyr::Gui
#pragma clang diagnostic pop