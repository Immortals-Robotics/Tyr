#pragma once

#include "command.h"

namespace Tyr::Sender
{
class Base
{
public:
    virtual ~Base() = default;

    virtual bool send(const CommandsWrapper &t_wrapper) = 0;

private:
    std::unique_ptr<Common::NngClient> m_client;
};
} // namespace Tyr::Sender
