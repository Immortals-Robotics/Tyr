#include "config.h"
#include <fstream>
namespace Tyr::Common
{
ConfigReader::ConfigReader(const std::string_view t_file_path)
{
    const std::filesystem::path data_dir(DATA_DIR);
    const std::filesystem::path file(t_file_path);
    m_file_path = data_dir / file;

    load();
}

void ConfigReader::load()
{
    toml::parse_result config = toml::parse_file(m_file_path.string());

    m_table = std::move(config);
}

void ConfigReader::save(toml::table t_table)
{
    std::ofstream ofs(m_file_path.string());
    ofs << t_table;
    ofs.close();
}

} // namespace Tyr::Common
