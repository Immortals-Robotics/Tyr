#include "dumper.h"

namespace Immortals::Common
{
bool Dumper::process()
{
    bool any = false;

    for (Entry &entry : m_entries)
    {
        const NngMessage message = entry.client.receiveRaw();
        if (message.size() == 0)
            continue;

        any = true;
        entry.storage.storeRaw(message.time().microseconds(), {message.data(), message.size()});
    }

    return any;
}
} // namespace Immortals::Common
