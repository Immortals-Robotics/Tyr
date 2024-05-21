#pragma once

namespace Tyr::Common
{
class ConfigReader
{
public:
    explicit ConfigReader(std::string_view t_file_path);

    void load();

    [[nodiscard]] auto getRoot() const
    {
        return m_table;
    }

    void save(toml::table t_table);

private:
    std::filesystem::path m_file_path;

    toml::table m_table;
};

struct IConfig
{
protected:
    virtual ~IConfig() = default;

public:
    virtual void load(toml::table t_table) = 0;
};
} // namespace Tyr::Common
