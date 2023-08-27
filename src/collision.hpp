#ifndef COLLISION_HPP_INCLUDED
#define COLLISION_HPP_INCLUDED

#include <glm/glm.hpp>
#include <vector>

// Collision shapes

struct CollisionShape
{
    virtual ~CollisionShape() = 0;
};

inline CollisionShape::~CollisionShape() {}



typedef std::vector<CollisionShape*> CollisionArray;



struct AABB : public CollisionShape
{
    glm::vec2 position, size;

    AABB(glm::vec2 position, glm::vec2 size) : position(position), size(size) {}
    virtual ~AABB() {}
};



struct EllipseCollision : public CollisionShape
{
    glm::vec2 position, radii;

    EllipseCollision(glm::vec2 position, glm::vec2 radii)
        : position(position), radii(radii) {}
    virtual ~EllipseCollision() {}
};



struct PointCollision : public CollisionShape
{
    glm::vec2 position;

    PointCollision(glm::vec2 position) : position(position) {}
    virtual ~PointCollision() {}
};



struct LineCollision : public CollisionShape
{
    glm::vec2 start, end;

    LineCollision(glm::vec2 start, glm::vec2 end) : start(start), end(end) {}
    virtual ~LineCollision() {}
};



struct TriangleCollision : public CollisionShape
{
    glm::vec2 a, b, c;
    TriangleCollision(glm::vec2 a, glm::vec2 b, glm::vec2 c)
        : a(a), b(b), c(c) {}
    virtual ~TriangleCollision() {}
};



std::vector<TriangleCollision*> TriangleCollisionFromPolygon(std::vector<glm::vec2>& points);

#endif
