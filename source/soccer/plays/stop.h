#include "play.h"

namespace Tyr::Soccer
{

class Stop final : public IPlay
{
public:
    void execute() override;
};

} // namespace Tyr::Soccer
