#include "obstacle.h"

namespace Tyr::Soccer
{
ObsMap map{};

void clear_map()
{
    map.resetMap();
}

bool IsInObstacle(const Common::Vec2 p)
{
    return map.isInObstacle(p);
}

bool collisionDetect(const Common::Vec2 p1, const Common::Vec2 p2)
{
    float coss, sinn;
    coss = (p2.x - p1.x) / p1.distanceTo(p2);
    sinn = (p2.y - p1.y) / p1.distanceTo(p2);

    Common::Vec2 current = p1;

    while (current.distanceTo(p2) > 10.0f)
    {
        if (IsInObstacle(current))
            return true;

        current.x += coss * 10.0f;
        current.y += sinn * 10.0f;
    }

    return false;
}

/*bool collisionDetect ( Common::Vec2 p1 , Common::Vec2 p2 )
{
        if ( map.getObsNum() == 0 )
            return false;
        float coss , sinn , dis;
        dis = Common::Vec2::distance ( p1 , p2 );

        if ( dis < 0.1 )
                return false;

        coss = ( p2.x - p1.x ) / dis;
        sinn = ( p2.y - p1.y ) / dis;

        Common::Vec2 current = p1;
        float advance_dis = 1;

        for ( int i = 0 ; Common::Vec2::distance ( current , p2 ) > 2 ; i ++ )
        {
                if ( i > 10 )
                    return true;

                if ( IsInObstacle ( current ) )
                        return true;
                advance_dis = std::min ( std::max ( 1 , map.NearestDistance(current.x,current.y) ) ,
Common::Vec2::distance ( current , p2 ) ); if ( advance_dis <= 0 ) return true;

                current.x += coss * advance_dis;
                current.y += sinn * advance_dis;
        }

        return false;
}*/

void AddCircle(const Common::Circle t_circle)
{
    map.addCircle(t_circle);
}

void AddRectangle(float x, float y, float w, float h)
{
    Common::Vec2 start{};
    Common::Vec2 size{};

    if (w < 0)
    {
        start.x = x + w;
        size.x  = -w;
    }
    else
    {
        start.x = x;
        size.x  = w;
    }

    if (h < 0)
    {
        start.y = y + h;
        // TODO: shouldn't this be -h?
        size.y = h;
    }
    else
    {
        start.y = y;
        size.y  = h;
    }

    map.addRectangle(Common::Rect{start, start + size});
}
} // namespace Tyr::Soccer
