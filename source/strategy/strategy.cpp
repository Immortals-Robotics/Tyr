#include "strategy.h"

Strategys::Strategys()
{
    for(int i=0;i<8;i++)
    {
        robot[i].waypointCounter=0;
        robot[i].ID = i;
    }

    robot[0].Name="gk";
    robot[1].Name="def1";
    robot[2].Name="def2";
    robot[3].Name="midf";
    robot[4].Name="passgir";
    robot[5].Name="atack";
    robot[4].Name="rw";
    robot[5].Name="lw";

    weight = 0;

}

void Strategys::SetName(QString str)
{
    name = str;
}

QString Strategys::getName()
{
    return name;
}

