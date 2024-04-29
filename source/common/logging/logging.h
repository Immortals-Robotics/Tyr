#pragma once

#include <quill/Quill.h>

namespace Tyr::Common
{
class Logger
{
protected:
    Logger();
    ~Logger() = default;

    friend struct Services;

private:
	quill::Logger* m_logger = nullptr;
};
} // namespace Tyr::Common
