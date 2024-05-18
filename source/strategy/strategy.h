#pragma once

#include "Robot.h"

namespace Tyr::Strategy
{
class Strategy
{
public:
    Strategy();

    void    setName(QString str);
    QString getName() const;

    Robot robot[8];
    int   weight;

    int min_x;
    int max_x;
    int min_y;
    int max_y;

private:
    QString name;
};
} // namespace Tyr::Strategy
