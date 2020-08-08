#pragma once
#include "GameObject.hpp"

//struct used to sort objects by distance
struct DistObj
{
    GameObject* obj;
    float distance;

    bool operator < (DistObj other)
    {
        return distance < other.distance;
    }
};

//struct used to define a key in the quadrant system
struct QuadrantKey
{
    int x;
    int y;
};

//== operator for QuadrantKey defined
inline bool operator == (const QuadrantKey& lhs, const QuadrantKey& rhs)
{
    return (lhs.x == rhs.x && lhs.y == rhs.y);
}

//specialized hash function for QuadrantKeys
struct QuadrantKeyHash
{
    size_t operator() (const QuadrantKey& qKey) const
    {
        //basically just mashes both coordinates together, the second one essentially bit shifted 4 bytes to the right
        size_t h1 = qKey.x;
        size_t h2 = 31 * (long long)qKey.y;

        return h1 ^ h2;
    }
};