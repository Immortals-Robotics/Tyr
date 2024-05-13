#include "storage.h"

#include "../services.h"

namespace Tyr::Common
{
static constexpr std::string_view getErrorString(const int t_error)
{
    switch (t_error)
    {
    case MDB_SUCCESS:
        return "MDB_SUCCESS";
    case MDB_KEYEXIST:
        return "MDB_KEYEXIST";
    case MDB_NOTFOUND:
        return "MDB_NOTFOUND";
    case MDB_PAGE_NOTFOUND:
        return "MDB_PAGE_NOTFOUND";
    case MDB_CORRUPTED:
        return "MDB_CORRUPTED";
    case MDB_PANIC:
        return "MDB_PANIC";
    case MDB_VERSION_MISMATCH:
        return "MDB_VERSION_MISMATCH";
    case MDB_INVALID:
        return "MDB_INVALID";
    case MDB_MAP_FULL:
        return "MDB_MAP_FULL";
    case MDB_DBS_FULL:
        return "MDB_DBS_FULL";
    case MDB_READERS_FULL:
        return "MDB_READERS_FULL";
    case MDB_TLS_FULL:
        return "MDB_TLS_FULL";
    case MDB_TXN_FULL:
        return "MDB_TXN_FULL";
    case MDB_CURSOR_FULL:
        return "MDB_CURSOR_FULL";
    case MDB_PAGE_FULL:
        return "MDB_PAGE_FULL";
    case MDB_MAP_RESIZED:
        return "MDB_MAP_RESIZED";
    case MDB_INCOMPATIBLE:
        return "MDB_INCOMPATIBLE";
    case MDB_BAD_RSLOT:
        return "MDB_BAD_RSLOT";
    case MDB_BAD_TXN:
        return "MDB_BAD_TXN";
    case MDB_BAD_VALSIZE:
        return "MDB_BAD_VALSIZE";
    case MDB_BAD_DBI:
        return "MDB_BAD_DBI";
    default:
        return std::strerror(t_error);
    }
}

bool Storage::init(const std::filesystem::path &t_path)
{
    // create output directory if not exists
    if (!std::filesystem::exists(t_path))
    {
        const bool fs_result = std::filesystem::create_directories(t_path);
        if (!fs_result)
        {
            logCritical("Failed to create db directory at {}", t_path);
            return false;
        }
    }

    int result;

    result = mdb_env_create(&s_env);
    if (result != MDB_SUCCESS)
    {
        logCritical("lmdb env creation failed with: {}", getErrorString(result));
        return false;
    }

    result = mdb_env_set_maxdbs(s_env, kMaxDbCount);
    if (result != MDB_SUCCESS)
    {
        logCritical("lmdb env maxdb setter failed with: {}", getErrorString(result));
        return false;
    }

    result = mdb_env_set_mapsize(s_env, kMapSize);
    if (result != MDB_SUCCESS)
    {
        logCritical("lmdb env map size setter failed with: {}", getErrorString(result));
        return false;
    }

    result = mdb_env_open(s_env, t_path.string().c_str(), 0, 0664);
    if (result != MDB_SUCCESS)
    {
        logCritical("lmdb env open in \"{}\" failed with: {}", t_path.string(), getErrorString(result));
        return false;
    }

    logInfo("Initialized storage in \"{}\"", t_path.string());

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
        logCritical("lmdb transaction begin failed with: {}", getErrorString(result));
        return false;
    }

    // lmdb expects null-terminated strings
    const std::string name_str{t_name};

    result = mdb_dbi_open(transaction, name_str.c_str(), MDB_INTEGERKEY | MDB_CREATE, &m_dbi);
    if (result != MDB_SUCCESS)
    {
        logCritical("lmdb db \"{}\" open failed with: {}", t_name, getErrorString(result));
        return false;
    }

    // this is needed to keep db handles
    result = mdb_txn_commit(transaction);
    if (result != MDB_SUCCESS)
    {
        logCritical("lmdb transaction commit failed with: {}", getErrorString(result));
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
        logError("lmdb readonly transaction begin failed with: {}", getErrorString(result));
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
        logError("lmdb get [{}] failed with: {}", t_key, getErrorString(result));
        return false;
    }

    const bool pb_result = t_message->ParseFromArray(mdb_data.mv_data, mdb_data.mv_size);
    mdb_txn_abort(transaction);

    if (!pb_result)
    {
        logError("Failed to parse protobuf message with size {} from db", mdb_data.mv_size);
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
        logError("lmdb read/write transaction begin failed with: {}", getErrorString(result));
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
        logError("lmdb dry put for key [{}] with size {} failed with: {}", t_key, message_size, getErrorString(result));
        return false;
    }

    const bool pb_result = t_message.SerializeToArray(mdb_data.mv_data, mdb_data.mv_size);
    if (!pb_result)
    {
        logError("Failed to serialize protobuf message for db storage");
        return false;
    }

    result = mdb_put(transaction, m_dbi, &mdb_key, &mdb_data, 0);
    if (result != MDB_SUCCESS)
    {
        logError("lmdb put for key [{}] with size {} failed with: {}", t_key, mdb_data.mv_size, getErrorString(result));
        return false;
    }

    result = mdb_txn_commit(transaction);
    if (result != MDB_SUCCESS)
    {
        logError("lmdb transaction commit failed with: {}", getErrorString(result));
        return false;
    }

    return true;
}
bool Storage::storeRaw(Key t_key, std::span<char> t_data)
{
    int result;

    MDB_txn *transaction;
    result = mdb_txn_begin(s_env, nullptr, 0, &transaction);
    if (result != MDB_SUCCESS)
    {
        logError("lmdb read/write transaction begin failed with: {}", getErrorString(result));
        return false;
    }

    MDB_val mdb_key{
        .mv_size = sizeof(Key),
        .mv_data = &t_key,
    };
    MDB_val mdb_data{
        .mv_size = t_data.size(),
        .mv_data = t_data.data(),
    };

    result = mdb_put(transaction, m_dbi, &mdb_key, &mdb_data, 0);
    if (result != MDB_SUCCESS)
    {
        logError("lmdb put for key [{}] with size {} failed with: {}", t_key, mdb_data.mv_size, getErrorString(result));
        return false;
    }

    result = mdb_txn_commit(transaction);
    if (result != MDB_SUCCESS)
    {
        logError("lmdb transaction commit failed with: {}", getErrorString(result));
        return false;
    }

    return true;
}
} // namespace Tyr::Common
