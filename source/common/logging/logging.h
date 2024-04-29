#pragma once

namespace Tyr::Common
{
class Logger
{
protected:
    Logger();
    ~Logger() = default;

    friend struct Services;
};
} // namespace Tyr::Common
