
#pragma once

#include "../network/nng_client.h"
#include "storage.h"

namespace Immortals::Common
{
class Dumper
{
public:
    Dumper() = default;

    void addEntry(std::string_view t_url, std::string_view t_db)
    {
        m_entries.emplace_back(t_url, t_db);
    }

    bool process();

private:
    struct Entry
    {
        NngClient client;
        Storage   storage;

        Entry(const std::string_view t_url, const std::string_view t_db) : client(t_url)
        {
            storage.open(t_db);
        }
    };

    std::vector<Entry> m_entries;
};
} // namespace Immortals::Common
