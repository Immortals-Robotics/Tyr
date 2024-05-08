
#pragma once

namespace Tyr::Common
{
class Storage
{
public:
    using Key = size_t;

    Storage() = default;

    bool open(std::string_view t_name);
    bool close();

    bool get(Key t_key, google::protobuf::MessageLite *t_message) const;
    bool store(Key t_key, const google::protobuf::MessageLite &t_message);

protected:
    static bool init(const std::filesystem::path &t_path);
    static void shutdown();

    friend struct Services;

private:
    static constexpr size_t kMaxDbCount = 10;
    static constexpr size_t kMapSize    = 1 * 1024 * 1024 * 1024; // 1 GB

    inline static MDB_env *s_env = nullptr;

    MDB_dbi m_dbi = 0;
};
} // namespace Tyr::Common
