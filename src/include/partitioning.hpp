#ifndef PARTITIONING_HPP_INCLUDED
#define PARTITIONING_HPP_INCLUDED

#include <glm/glm.hpp>
#include <set>
#include <vector>
#include <flecs.h>

typedef std::set<flecs::entity> EntitySet;

class Grid
{
private:
    std::vector<EntitySet> cells;
    glm::ivec2 size;
    glm::vec2  cellsize;

    inline glm::ivec2 asCellCoord(glm::vec2);
    inline int        asArrayCoord(glm::ivec2);
    inline int        coordToArray(glm::vec2);

    std::vector<int>  getOverlappingCells(glm::vec2 center, float radius);
public:
    Grid(glm::vec2 playarea, glm::vec2 cellsize);

    void insert(flecs::entity obj);
    void move(flecs::entity obj);
    void testAll(CollisionTestFn fn);
};

#endif
