#include "collision.hpp"
#include <algorithm>

#include "tiled.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include <iostream>

namespace Collision
{
    std::optional<glm::vec2> linecastAABB(const glm::vec2& start,
                                          const glm::vec2& end,
                                          const glm::vec2& position,
                                          const glm::vec2& size);
    std::optional<glm::vec2> linecastEllipse(const glm::vec2& start,
                                             const glm::vec2& end,
                                             glm::vec2 center,
                                             glm::vec2& radii);
    std::optional<glm::vec2> linecastCircle(const glm::vec2& start,
                                            const glm::vec2& end,
                                            glm::vec2 center,
                                            float radius);
    std::optional<glm::vec2> linecastTriangle(const glm::vec2& start,
                                              const glm::vec2& end,
                                              glm::vec2 a,
                                              glm::vec2 b,
                                              glm::vec2 c);
    
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

    std::optional<glm::vec2>
    Linecast(
        Resources::LevelPtr level,
        glm::vec2 cameraCenter,
        glm::vec2 viewport,
        glm::vec2 start, glm::vec2 end)
    {
        auto windowsize = glm::ivec2(3, 3);
        std::optional<glm::vec2> ret = std::nullopt;

        auto setret = [&](std::optional<glm::vec2> v) -> void {
            if(!ret) ret = v;
            else if(ret && v) {
                // Check for highest
                if((*v).y < (*ret).y)
                    ret = v;
            }
        };
        
        for(auto layer : level->map->layers) {
            // ignore non-background layers.
            // TODO: ok, but WHAT background layer should we collide with?
            if(layer.name.rfind("bg") != 0)
                continue;

            auto tilewindow = layer.getTileWindow(
                cameraCenter,
                viewport,
                level->tiledata->tilesize,
                windowsize);
            glm::vec2 cameraDiff = glm::mod(cameraCenter, level->tiledata->tilesize);

            for(unsigned x = 0; x < 3; x++)
                for(unsigned y = 0; y < 3; y++) {
                    glm::vec2 windowPos = glm::vec2(x, y) * level->tiledata->tilesize;

                    // compensate all non-line positions with this
                    //glm::vec2 comp = windowPos + cameraDiff;
                    glm::vec2 comp = windowPos - glm::mod(cameraCenter, level->tiledata->tilesize);
                    
                    unsigned i = (y * 3) + x;
                    if(tilewindow[i] == 0) continue;
                    //std::cout << "tile = " << tilewindow[i]-1 << std::endl;
                    std::optional<Collision::Array> opt = level->tiledata->collisionarrays[tilewindow[i] - 1]; //--
                    if(opt == std::nullopt) continue;
                    std::vector<ShapePtr> collisions = *opt;

                    for(auto shape : collisions) {
                        if(auto aabb = std::dynamic_pointer_cast<AABB>(shape)) {
                            auto intersection = linecastAABB(
                                start, end, aabb->position + comp, aabb->size);
                            if(intersection) {
                                std::cout << "aabb intersection" << std::endl;
                                setret(intersection);
                            }
                            continue;
                        }

                        if(auto ellipse = std::dynamic_pointer_cast<Ellipse>(shape)) {
                            std::optional<glm::vec2> intersection = std::nullopt;
                            if(glm::epsilonEqual(ellipse->radii.x, ellipse->radii.y, glm::epsilon<float>()))
                                intersection = linecastCircle(
                                    start, end, ellipse->position + comp, ellipse->radii.x);
                            else intersection = linecastEllipse(
                                start, end, ellipse->position + comp, ellipse->radii);
                            if(intersection) {
                                std::cout << "ellipse intersection" << std::endl;
                                setret(intersection);
                            }
                            continue;
                        }

                        if(auto triangle = std::dynamic_pointer_cast<Triangle>(shape)) {
                            auto intersection = linecastTriangle(
                                start, end, triangle->a + comp, triangle->b + comp, triangle->c + comp);
                            if(intersection) {
                                std::cout << "triangle intersection" << std::endl;
                                setret(intersection);
                            }
                            continue;
                        }
                    }
                }
        }

        if(ret) {
            std::cout << "Collision at " << (*ret).x << ", " << (*ret).y << std::endl;
        }
        return ret;
    }

    
    std::optional<glm::vec2>
    linecastAABB(
        const glm::vec2& start,
        const glm::vec2& end,
        const glm::vec2& position,
        const glm::vec2& size)
    {
        float tmin = 0.0f, tmax = 1.0f;

        auto boxmin = position;
        auto boxmax = position + size;
        
        for(int i = 0; i < 2; i++) {
            if(glm::epsilonEqual(end[i], start[i], glm::epsilon<float>())) {
                if(start[i] < boxmin[i] || start[i] > boxmax[i])
                    return std::nullopt; // no intersection
            }

            float ood = 1.0f / (start[i] - end[i]);
            float t1 = (boxmin[i] - start[i]) * ood;
            float t2 = (boxmax[i] - start[i]) * ood;

            tmin = glm::max(tmin, glm::min(t1, t2));
            tmax = glm::min(tmax, glm::max(t1, t2));

            if(tmax < tmin)
                return std::nullopt; // no intersection
        }

        return (start + tmin * (end - start));
    }
    
    std::optional<glm::vec2>
    linecastEllipse(
        const glm::vec2& start,
        const glm::vec2& end,
        glm::vec2 center,
        glm::vec2& radii)
    {
        // vector on line direction
        glm::vec2 dir = end - start;

        // vector between line start and ellipse center
        glm::vec2 diff = start - center;

        // quadratic equation coefficients
        auto sqradius = radii * radii;
        float a = dir.x * dir.x / sqradius.x + dir.y * dir.y / sqradius.y;
        float b = 2.0f * (diff.x * dir.x / sqradius.x + diff.y * dir.y / sqradius.y);
        float c = diff.x * diff.x / sqradius.x + diff.y * diff.y / sqradius.y - 1.0f;

        // discriminant of the quadratic equation
        float discriminant = (b * b) - 4.0f * a * c;

        if(discriminant < 0.0f) return std::nullopt;

        // calculate intersection points
        float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
        float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

        // since the line is segmented, verify for intervals [0, 1]
        if(t1 >= 0.0f && t1 <= 1.0f)
            return start + t1 * dir;
        if(t2 >= 0.0f && t2 <= 1.0f)
            return start + t2 * dir;

        return std::nullopt;
    }

    std::optional<glm::vec2>
    linecastCircle(
        const glm::vec2& start,
        const glm::vec2& end,
        glm::vec2 center,
        float radius)
    {
        glm::vec2 dir = end - start;
        glm::vec2 diff = start - center;

        float a = glm::dot(dir, dir);
        float b = 2.0f * glm::dot(diff, dir);
        float c = glm::dot(diff, diff) - radius * radius;

        float discriminant = (b * b) - 4.0f * a * c;

        if(discriminant < 0.0f) return std::nullopt;

        float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
        float t2 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

        if(t1 >= 0.0f && t1 <= 1.0f)
            return start + t1 * dir;
        if(t2 >= 0.0f && t2 <= 1.0f)
            return start + t2 * dir;

        return std::nullopt;
    }

    std::optional<glm::vec2>
    linecastTriangle(const glm::vec2& start,
                     const glm::vec2& end,
                     glm::vec2 a,
                     glm::vec2 b,
                     glm::vec2 c)
    {
        // build inverse transform matrix to convert triangle coordinates
        // into object space
        glm::mat3 invTransform = glm::inverse(
            glm::mat3(glm::vec3(a, 0.0f), glm::vec3(b, 0.0f), glm::vec3(c, 0.0f)));

        // transform line into triangle object space
        glm::vec3 startObj = invTransform * glm::vec3(start, 1.0f);
        glm::vec3 endObj = invTransform * glm::vec3(end, 1.0f);
        glm::vec3 dirObj = endObj - startObj;

        // parameters for line's parametric equation
        float t = -startObj.z / dirObj.z;

        // calculate barycentric coordinates
        glm::vec2 intersection = glm::vec2(startObj) + t * glm::vec2(dirObj);

        // verify if barycentric coordinates are within triangle
        if(intersection.x >= 0.0f
           && intersection.y >= 0.0f
           && (intersection.x + intersection.y) <= 1.0f) {
            // transform intersection point back to original space
            glm::vec3 origIntersection = glm::vec3(intersection, 1.0f);
            glm::vec3 worldIntersection =
                glm::mat3(glm::vec3(a, 0.0f),
                          glm::vec3(b, 0.0f),
                          glm::vec3(c, 0.0f)) * origIntersection;
            return glm::vec2(worldIntersection.x, worldIntersection.y);
        }

        return std::nullopt;
    }
}
