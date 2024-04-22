#pragma once


namespace Loki::Common
{
class Logger
{
protected:
    Logger();
    ~Logger() = default;

    friend struct Services;

private:
    std::filesystem::path getNewLogFilePath() const;
};
} // namespace Loki::Common
