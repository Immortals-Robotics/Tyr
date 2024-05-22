#include "file.h"
namespace Tyr::Common::Config
{
File::File(const std::filesystem::path &t_file_name)
{
    const std::filesystem::path data_dir(DATA_DIR);
    m_file_path = data_dir / t_file_name;
}

void File::load()
{
    toml::parse_result config = toml::parse_file(m_file_path.string());

    m_table = std::move(config);
}

void File::save() const
{
    std::ofstream ofs(m_file_path);
    ofs << m_table;
    ofs.close();
}

} // namespace Tyr::Common::Config
