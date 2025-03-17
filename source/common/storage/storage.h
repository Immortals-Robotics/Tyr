
#pragma once

namespace Immortals::Common
{
class Storage
{
public:
    using Key = size_t;

    Storage() = default;

    bool open(std::string_view t_name);
    bool close();

    bool get(Key t_key, google::protobuf::MessageLite *t_message) const;

    bool closest(Key t_key, Key *t_closest) const;
    bool next(Key t_key, Key *t_next) const;

    unsigned long getBoundary(Key *t_first, Key *t_last) const;
    bool          store(Key t_key, const google::protobuf::MessageLite &t_message);

    bool storeRaw(Key t_key, std::span<char> t_data);

protected:
    static bool init(const std::filesystem::path &t_path);
    static void shutdown();

    friend struct Services;

private:
    static constexpr size_t kMaxDbCount = 10;
    static constexpr size_t kMapSize    = 100llu * 1024llu * 1024llu * 1024llu; // 1 GB

    inline static MDB_env *s_env = nullptr;

    MDB_dbi m_dbi = 0;
};
} // namespace Immortals::Common
