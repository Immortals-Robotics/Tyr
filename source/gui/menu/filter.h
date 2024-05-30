#pragma once

namespace Tyr::Gui
{
class FilterMenu
{
public:
     FilterMenu() = default;
    ~FilterMenu() = default;

    void feedDebug(const Common::Debug::Wrapper &t_wrapper);

    void draw();

    struct Node
    {
        explicit Node(const std::string &t_name) : name(t_name)
        {}

        void addChild(const std::string &t_child);

        std::string name;
        bool        active = true;

        std::vector<std::unique_ptr<Node>> children;
    };

    struct Map
    {
        bool apply(const Common::Debug::SourceLocation &t_source) const
        {
            const std::filesystem::path file_path{t_source.file};
            const std::string           file_name = file_path.filename().string();
            if (m_map.contains(file_name) && m_map.at(file_name)->active)
            {
                for (const auto &function : m_map.at(file_name)->children)
                {
                    if (extractFunctionName(t_source.function) == function->name && function->active)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

    private:
        friend class FilterMenu;
        std::unordered_map<std::string, Node *> m_map;
    };

    const Map &map() const
    {
        return m_map;
    }

private:
    void pushToFilters(const Common::Debug::SourceLocation &t_source);

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

    bool m_all_filter = true;
    bool m_all_change = false;

    std::vector<std::unique_ptr<Node>> m_nodes;
    Map                                m_map;
};
} // namespace Tyr::Gui
