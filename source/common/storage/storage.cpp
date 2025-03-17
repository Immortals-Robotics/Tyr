#include "storage.h"

namespace Immortals::Common
{
static std::string getErrorString(const int t_error)
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
        std::string buff(80, '\0');
#ifdef _MSC_VER
        if (strerror_s(buff.data(), buff.size(), t_error) != 0)
        {
            buff = "Unknown error";
        }
#elif defined(__APPLE__)
        if (strerror_r(t_error, buff.data(), buff.size()) != 0)
        {
            buff = "Unknown error";
        }
#else
        auto        p = strerror_r(t_error, buff.data(), buff.size());
        std::string tmp(p, std::strlen(p));
        std::swap(buff, tmp);
#endif
        buff.resize(buff.find('\0'));
        return buff;
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

        mdb_txn_abort(transaction);
        return false;
    }

    // lmdb expects null-terminated strings
    const std::string name_str{t_name};

    result = mdb_dbi_open(transaction, name_str.c_str(), MDB_INTEGERKEY | MDB_CREATE, &m_dbi);
    if (result != MDB_SUCCESS)
    {
        logCritical("lmdb db \"{}\" open failed with: {}", t_name, getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    // this is needed to keep db handles
    result = mdb_txn_commit(transaction);
    if (result != MDB_SUCCESS)
    {
        logCritical("lmdb transaction commit failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
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

    MDB_cursor *mdb_cursor;

    result = mdb_cursor_open(transaction, m_dbi, &mdb_cursor);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb cursor open failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    result = mdb_cursor_get(mdb_cursor, &mdb_key, &mdb_data, MDB_SET_RANGE);
    if (result == MDB_NOTFOUND)
    {
        Common::logWarning("lmdb cursor get failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }
    else if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb get failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    bool pb_result = t_message->ParseFromArray(mdb_data.mv_data, mdb_data.mv_size);

    mdb_txn_abort(transaction);

    if (!pb_result)
    {
        logError("Failed to parse protobuf message with size {} from db", mdb_data.mv_size);
        return false;
    }

    return true;
}

bool Storage::closest(Key t_key, Key *t_closest) const
{
    int result;

    MDB_txn *transaction;
    result = mdb_txn_begin(s_env, nullptr, MDB_RDONLY, &transaction);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb readonly transaction begin failed with: {}", getErrorString(result));
        return false;
    }

    MDB_val mdb_key{
        .mv_size = sizeof(Key),
        .mv_data = &t_key,
    };
    MDB_val mdb_data;

    MDB_cursor *mdb_cursor;

    result = mdb_cursor_open(transaction, m_dbi, &mdb_cursor);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb cursor open failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    result = mdb_cursor_get(mdb_cursor, &mdb_key, &mdb_data, MDB_SET_RANGE);
    if (result == MDB_NOTFOUND)
    {
        Common::logWarning("lmdb cursor get failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }
    else if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb cursor get failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    *t_closest = *static_cast<Key *>(mdb_key.mv_data);

    mdb_txn_abort(transaction);

    return true;
}

bool Storage::next(Storage::Key t_key, Storage::Key *t_next) const
{
    int result;

    MDB_txn *transaction;
    result = mdb_txn_begin(s_env, nullptr, MDB_RDONLY, &transaction);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb readonly transaction begin failed with: {}", getErrorString(result));
        return false;
    }

    MDB_cursor *mdb_cursor;
    result = mdb_cursor_open(transaction, m_dbi, &mdb_cursor);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb cursor open failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    MDB_val mdb_key{
        .mv_size = sizeof(Key),
        .mv_data = &t_key,
    };
    MDB_val mdb_data;

    result = mdb_cursor_get(mdb_cursor, &mdb_key, &mdb_data, MDB_SET_RANGE);
    if (result == MDB_NOTFOUND)
    {
        Common::logWarning("lmdb cursor get failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }
    else if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb cursor get failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    result = mdb_cursor_get(mdb_cursor, &mdb_key, &mdb_data, MDB_NEXT);
    if (result == MDB_NOTFOUND)
    {
        Common::logWarning("lmdb cursor get failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }
    else if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb cursor get failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    *t_next = *static_cast<Key *>(mdb_key.mv_data);

    mdb_txn_abort(transaction);

    return true;
}

unsigned long Storage::getBoundary(Key *t_first, Key *t_last) const
{
    int result;

    MDB_txn *transaction;
    result = mdb_txn_begin(s_env, nullptr, 0, &transaction);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb read/write transaction begin failed with: {}", getErrorString(result));
        return 0;
    }

    MDB_val     mdb_key, mdb_data;
    MDB_cursor *mdb_cursor;

    result = mdb_cursor_open(transaction, m_dbi, &mdb_cursor);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb cursor open failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return 0;
    }

    result = mdb_cursor_get(mdb_cursor, &mdb_key, &mdb_data, MDB_FIRST);
    if (result == MDB_NOTFOUND)
    {
        mdb_txn_abort(transaction);
        return 0;
    }
    else if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb get failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return 0;
    }

    *t_first = *static_cast<Key *>(mdb_key.mv_data);

    result = mdb_cursor_get(mdb_cursor, &mdb_key, &mdb_data, MDB_LAST);
    if (result == MDB_NOTFOUND)
    {
        mdb_txn_abort(transaction);
        return 0;
    }
    else if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb get failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return 0;
    }
    *t_last = *static_cast<Key *>(mdb_key.mv_data);

    MDB_stat stat;
    result = mdb_stat(transaction, m_dbi, &stat);
    if (result != MDB_SUCCESS)
    {
        mdb_txn_abort(transaction);
        Common::logError("Could not get stats: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return 0;
    }

    result = mdb_txn_commit(transaction);
    if (result != MDB_SUCCESS)
    {
        Common::logError("lmdb transaction commit failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    return stat.ms_entries;
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

        mdb_txn_abort(transaction);
        return false;
    }

    const bool pb_result = t_message.SerializeToArray(mdb_data.mv_data, mdb_data.mv_size);
    if (!pb_result)
    {
        logError("Failed to serialize protobuf message for db storage");

        mdb_txn_abort(transaction);
        return false;
    }

    result = mdb_put(transaction, m_dbi, &mdb_key, &mdb_data, 0);
    if (result != MDB_SUCCESS)
    {
        logError("lmdb put for key [{}] with size {} failed with: {}", t_key, mdb_data.mv_size, getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    result = mdb_txn_commit(transaction);
    if (result != MDB_SUCCESS)
    {
        logError("lmdb transaction commit failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
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

        mdb_txn_abort(transaction);
        return false;
    }

    result = mdb_txn_commit(transaction);
    if (result != MDB_SUCCESS)
    {
        logError("lmdb transaction commit failed with: {}", getErrorString(result));

        mdb_txn_abort(transaction);
        return false;
    }

    return true;
}
} // namespace Immortals::Common
