#include "engine/collision.hpp"
#include <algorithm>

namespace Collision
{
    Array
    TrianglesFromPolygon(std::vector<glm::vec2>& points)
    {
        Array triangles;
        if(points.size() < 3) return triangles;

        std::sort(points.begin(), points.end(),
                  [](glm::vec2 const& a, glm::vec2 const& b) {
                      return a.x < b.x;
                  });

        glm::vec2 leftmost = points[0];
        std::vector<glm::vec2> below;
        std::vector<glm::vec2> above;

        bool first = true;
        for(auto point : points) {
            if(!first) {
                if((point.y - leftmost.y > 0))
                    above.push_back(point);
                else
                    below.push_back(point);
            }
            first = false;
        }

        // Points are ordered from the leftmost, in a counterclockwise
        // manner, going through the bottom ones and then the above ones
        std::vector<glm::vec2> ordered;
        ordered.push_back(leftmost);
        ordered.insert(ordered.end(), below.begin(), below.end());
        ordered.insert(ordered.end(), above.rbegin(), above.rend());

        for(unsigned i = 0; i < ordered.size() - 2; i += 2) {
            triangles.push_back(
                std::make_unique<Triangle>(
                    ordered[i],
                    ordered[i + 1],
                    ordered[i + 2]));
        }

        return triangles;
    }
}
