#pragma once

#include "command.h"

namespace Tyr::Sender
{
class ISender
{
public:
    virtual ~ISender() = default;

    virtual void queueCommand(const Command &command) = 0;
    virtual bool flush()                              = 0;
};
} // namespace Tyr::Sender
