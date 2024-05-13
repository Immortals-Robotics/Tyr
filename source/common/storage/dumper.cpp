#include "dumper.h"

namespace Tyr::Common
{
void Dumper::process()
{
    for (Entry &entry : m_entries)
    {
        const NngMessage message = entry.client.receiveRaw();
        if (message.size() == 0)
            continue;

        entry.storage.storeRaw(message.time().timestamp(), {message.data(), message.size()});
    }
}
} // namespace Tyr::Common
