#pragma once

namespace Tyr::Common::Config
{
class File
{
public:
    explicit File(const std::filesystem::path &t_path);

    void load();
    void save() const;

    [[nodiscard]] const toml::table &root() const
    {
        return m_table;
    }

    [[nodiscard]] toml::table &root()
    {
        return m_table;
    }

private:
    std::filesystem::path m_file_path;

    toml::table m_table;
};
} // namespace Tyr::Common::Config
