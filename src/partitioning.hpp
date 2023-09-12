#ifndef PARTITIONING_HPP_INCLUDED
#define PARTITIONING_HPP_INCLUDED

#include <glm/glm.hpp>
#include <set>

#include "game_object.hpp"

typedef std::set<ObjPtr> ObjSet;

class Grid
{
private:
    std::vector<ObjSet> cells;
    glm::ivec2 size;
    glm::vec2  cellsize;

    inline glm::ivec2 asCellCoord(glm::vec2);
    inline int        asArrayCoord(glm::ivec2);
    inline int        coordToArray(glm::vec2);

    std::vector<int>  getOverlappingCells(glm::vec2 center, float radius);
public:
    Grid(glm::vec2 playarea, glm::vec2 cellsize);

    void insert(ObjPtr obj);
    void move(ObjPtr obj);
    void testAll(CollisionTestFn fn);
};

#endif
