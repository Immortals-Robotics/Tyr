#pragma once

#include "Robot.h"

namespace Tyr::Strategy
{
class Strategys
{
public:
    Strategys();
    void    SetName(QString str);
    QString getName();
    QString name;
    Robot   robot[8];
    int     weight;
    int     minX;
    int     maxX;
    int     minY;
    int     maxY;
};
} // namespace Tyr::Strategy
