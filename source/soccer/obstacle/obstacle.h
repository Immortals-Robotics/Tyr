#pragma once

#include "obstacle_new.h"

namespace Tyr::Soccer
{
/*bool obs_lut[605][405];

void clear_map ( void )
{
    for ( int i = 0 ; i < 605 ; i ++ )
        for ( int j = 0 ; j < 405 ; j ++ )
        {
            obs_lut[i][j] = false;
        }
}

bool IsInObstacle ( Common::Vec2 p )
{
    return obs_lut[(int)p.x][(int)p.y];
}

bool collisionDetect ( Common::Vec2 p1 , Common::Vec2 p2 )
{
    float coss , sinn;
    coss = ( p2.x - p1.x ) / DIS ( p1 , p2 );
    sinn = ( p2.y - p1.y ) / DIS ( p1 , p2 );

    Common::Vec2 current = p1;

    while ( DIS ( current , p2 ) > 9 )
    {
        if ( IsInObstacle ( current ) )
            return true;

        current.x += coss * 9;
        current.y += sinn * 9;
    }

    return false;
}

void AddCircle ( int x , int y , int r )
{
    for ( int i = 0 ; i < 605 ; i ++ )
        for ( int j = 0 ; j < 405 ; j ++ )
        {
            //obs_lut[i][j] = false;
            if ( (i-x) * (i-x) + (j-y) * (j-y) < r * r )	obs_lut[i][j] = true;
        }
}*/

void clear_map(void);
bool IsInObstacle(Common::Vec2 p);
bool collisionDetect(Common::Vec2 p1, Common::Vec2 p2);
void AddCircle(float x, float y, float r);
void AddRectangle(float x, float y, float w, float h);
} // namespace Tyr::Soccer