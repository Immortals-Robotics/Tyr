#include "storage.h"

#include "../services.h"

namespace Tyr::Common
{
bool Storage::init(const std::filesystem::path &t_path)
{
    // create output directory if not exists
    if (!std::filesystem::exists(t_path))
    {
        const bool fs_result = std::filesystem::create_directories(t_path);
        if (!fs_result)
        {
            Common::logCritical("Failed to create db directory at {}", t_path.string());
            return false;
        }
    }

    int result;

    result = mdb_env_create(&s_env);
    if (result != MDB_SUCCESS)
    {
        Common::logCritical("lmdb env creation failed with: {}", result);
        return false;
    }

    result = mdb_env_set_maxdbs(s_env, kMaxDbCount);
    if (result != MDB_SUCCESS)
    {
        Common::logCritical("lmdb env maxdb setter failed with: {}", result);
        return false;
    }

    result = mdb_env_set_mapsize(s_env, kMapSize);
    if (result != MDB_SUCCESS)
    {
        Common::logCritical("lmdb env map size setter failed with: {}", result);
        return false;
    }

    result = mdb_env_open(s_env, t_path.string().c_str(), 0, 0664);
    if (result != MDB_SUCCESS)
    {
        Common::logCritical("lmdb env open in \"{}\" failed with: {}", t_path.string(), result);
        return false;
    }

    Common::logInfo("Initialized storage in \"{}\"", t_path.string());

    return true;
}

void Storage::shutdown()
{
    mdb_env_close(s_env);
    s_env = nullptr;
}

bool Storage::open(std::string_view t_name)
{
    int result;

    MDB_txn *transaction;
    result = mdb_txn_begin(s_env, nullptr, 0, &transaction);
    if (result != MDB_SUCCESS)
    {
        Common::logCritical("lmdb transaction begin failed with: {}", result);
        return false;
    }

    // lmdb expects null-terminated strings
    const std::string name_str{t_name};

    result = mdb_dbi_open(transaction, name_str.c_str(), MDB_INTEGERKEY | MDB_CREATE, &m_dbi);
    if (result != MDB_SUCCESS)
    {
        Common::logCritical("lmdb db \"{}\" open failed with: {}", t_name, result);
        return false;
    }

    // this is needed to keep db handles
    result = mdb_txn_commit(transaction);
    if (result != MDB_SUCCESS)
    {
        Common::logCritical("lmdb transaction commit failed with: {}", result);
        return false;
    }

    return true;
}

bool Storage::close()
{
    return true;
}

bool Storage::get(Key t_key, google::protobuf::MessageLite *t_message) const
{
    int result;

    MDB_txn *transaction;
    result = mdb_txn_begin(s_env, nullptr, MDB_RDONLY, &transaction);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb readonly transaction begin failed with: {}", result);
        return false;
    }

    MDB_val mdb_key{
        .mv_size = sizeof(Key),
        .mv_data = &t_key,
    };
    MDB_val mdb_data;
    result = mdb_get(transaction, m_dbi, &mdb_key, &mdb_data);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb get [{}] failed with: {}", t_key, result);
        return false;
    }

    const bool pb_result = t_message->ParseFromArray(mdb_data.mv_data, mdb_data.mv_size);
    mdb_txn_abort(transaction);

    if (!pb_result)
    {
        Common::logError("Failed to parse protobuf message with size {} from db", mdb_data.mv_size);
        return false;
    }

    return true;
}

bool Storage::store(Key t_key, const google::protobuf::MessageLite &t_message)
{
    int result;

    MDB_txn *transaction;
    result = mdb_txn_begin(s_env, nullptr, 0, &transaction);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb read/write transaction begin failed with: {}", result);
        return false;
    }

    const size_t message_size = t_message.ByteSizeLong();

    MDB_val mdb_key{
        .mv_size = sizeof(Key),
        .mv_data = &t_key,
    };
    MDB_val mdb_data{
        .mv_size = message_size,
        .mv_data = nullptr,
    };

    // dry put to ask lmdb to reserve the destination buffer
    result = mdb_put(transaction, m_dbi, &mdb_key, &mdb_data, MDB_RESERVE);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb dry put for key [{}] with size {} failed with: {}", t_key, message_size, result);
        return false;
    }

    const bool pb_result = t_message.SerializeToArray(mdb_data.mv_data, mdb_data.mv_size);
    if (!pb_result)
    {
        Common::logError("Failed to serialize protobuf message for db storage");
        return false;
    }

    result = mdb_put(transaction, m_dbi, &mdb_key, &mdb_data, 0);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb put for key [{}] with size {} failed with: {}", t_key, message_size, result);
        return false;
    }

    result = mdb_txn_commit(transaction);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb transaction commit failed with: {}", result);
        return false;
    }

    return true;
}
} // namespace Tyr::Common
