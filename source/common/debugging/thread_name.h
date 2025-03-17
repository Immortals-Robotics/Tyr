#pragma once

namespace Immortals::Common::Debug
{
inline void setThreadName(const char *name)
{
#if defined(_WIN32)
    const std::wstring wide_name{name, name + strlen(name)};

    const HRESULT result = SetThreadDescription(GetCurrentThread(), wide_name.c_str());
    if (FAILED(result))
    {
        logError("Failed to set thread name \"{}\": {}", name, result);
    }
#elif defined(__linux__)
    pthread_setname_np(pthread_self(), name);
#elif defined(__APPLE__)
    pthread_setname_np(name);
#endif
}
} // namespace Immortals::Common::Debug
