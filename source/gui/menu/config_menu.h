#pragma once

namespace Tyr::Gui
{
class ConfigMenu
{
private:
    ImGuiWindowFlags m_window_flags;

    void drawTabBar();
    void drawConfigTab();
    void drawFilterTab();

    void drawConfigItem(const std::string &t_key, toml::node &t_value);
    void drawConfigArray(toml::array &t_array, int t_columns = 0);
    void drawConfigTable(toml::table &t_table);

    template <typename T>
    void pushToFilters(const std::vector<T> &t_input);

    bool m_configs_dirty = false;

public:
         ConfigMenu();
    ~    ConfigMenu() = default;
    void feedDebug(const Common::Debug::Wrapper &t_wrapper);

    void draw();

    struct FilterNode
    {
                                                 FilterNode(std::string t_name);
        std::string                              name;
        std::vector<std::unique_ptr<FilterNode>> children;
        bool                                     active = true;
        void                                     addChild(std::string t_child);
    };

    struct FilterTree
    {
                                                 FilterTree() = default;
        std::vector<std::unique_ptr<FilterNode>> nodes;
        void                                     clear();
        void                                     addNode(std::unique_ptr<FilterNode> t_node);
    };

    bool                                          m_all_filter = true;
    bool                                          m_all_change = false;
    FilterTree                                    m_filter_tree;
    std::unordered_map<std::string, FilterNode *> m_node_map;

    const std::unordered_map<std::string, FilterNode *> nodeMap()
    {
        return m_node_map;
    }

    static std::string extractFunctionName(const std::string_view t_function_name)
    {
        auto function_name = std::string(t_function_name);
        auto end_pos       = function_name.find_last_of('(');
        if (end_pos != std::string::npos)
        {
            auto start_pos = function_name.rfind(':', end_pos);
            if (start_pos != std::string::npos)
            {
                function_name = function_name.substr(start_pos + 1, end_pos - start_pos - 1);
            }
        }
        return function_name;
    }

    static bool applyFilter(Common::Debug::SourceLocation                        t_source,
                            const std::unordered_map<std::string, FilterNode *> &t_filter_node_map)
    {
        const std::filesystem::path file_path{t_source.file};
        const std::string           file_name = file_path.filename().string();
        auto                        draw      = false;
        if (t_filter_node_map.find(file_name) != t_filter_node_map.end() && t_filter_node_map.at(file_name)->active)
        {
            for (const auto &function : t_filter_node_map.at(file_name)->children)
            {
                if (ConfigMenu::extractFunctionName(t_source.function) == function->name && function->active)
                {
                    draw = true;
                }
            }
        }
        return draw;
    }
};

} // namespace Tyr::Gui
