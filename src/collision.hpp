#ifndef COLLISION_HPP_INCLUDED
#define COLLISION_HPP_INCLUDED

#include <glm/glm.hpp>
#include <vector>

// Collision shapes

namespace Collision
{
    struct Shape
    {
        virtual ~Shape() = 0;
    };

    inline Shape::~Shape() {}

    
    struct AABB : public Shape
    {
        glm::vec2 position, size;

        AABB(glm::vec2 position, glm::vec2 size) : position(position), size(size) {}
        virtual ~AABB() {}
    };

    
    struct Ellipse : public Shape
    {
        glm::vec2 position, radii;

        Ellipse(glm::vec2 position, glm::vec2 radii)
            : position(position), radii(radii) {}
        virtual ~Ellipse() {}
    };


    struct Point : public Shape
    {
        glm::vec2 position;

        Point(glm::vec2 position) : position(position) {}
        virtual ~Point() {}
    };


    struct Line : public Shape
    {
        glm::vec2 start, end;

        Line(glm::vec2 start, glm::vec2 end) : start(start), end(end) {}
        virtual ~Line() {}
    };


    struct Triangle : public Shape
    {
        glm::vec2 a, b, c;
        Triangle(glm::vec2 a, glm::vec2 b, glm::vec2 c)
            : a(a), b(b), c(c) {}
        virtual ~Triangle() {}
    };


    std::vector<Triangle*> TrianglesFromPolygon(std::vector<glm::vec2>& points);

    typedef std::vector<Shape*> Array;

}
    
#endif
