#pragma once

namespace Immortals::Common::Debug
{
void setThreadName(const char *name)
{
#if defined(_WIN32)
    const std::wstring wideName{name, name + strlen(name)};

    const HRESULT hr = SetThreadDescription(GetCurrentThread(), wideName.c_str());
    if (FAILED(hr))
    {
        logError("Failed to set thread name \"{}\": {}", name, hr);
    }
#elif defined(__linux__) || defined(__APPLE__)
    pthread_setname_np(name);
#endif
}
} // namespace Immortals::Common::Debug
