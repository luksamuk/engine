#include "partitioning.hpp"
#include <algorithm>
#include <iostream>

inline int
Grid::coordToArray(glm::vec2 pos)
{
    return Grid::asArrayCoord(Grid::asCellCoord(pos));
}

inline int
Grid::asArrayCoord(glm::ivec2 cellCoord)
{
    return (cellCoord.y * this->size.x) + cellCoord.x;
}

inline glm::ivec2
Grid::asCellCoord(glm::vec2 pos)
{
    pos -= glm::mod(pos, this->cellsize);
    return glm::ivec2(glm::trunc(pos / this->cellsize));
}

Grid::Grid(glm::vec2 playarea, glm::vec2 cellsize)
{
    this->cellsize = cellsize;
    size = this->asCellCoord(playarea) + 1;
    cells.reserve(size.x * size.y);
    for(int i = 0; i < size.x * size.y; i++) {
        cells.push_back({});
    }

    std::cout << "Created grid " << size.x << "x" << size.y
              << " with a cell size of " << cellsize.x << "x" << cellsize.y
              << "." << std::endl;
}

std::vector<int>
Grid::getOverlappingCells(glm::vec2 center, float radius)
{
    std::vector<int> cellidx;

    auto pushvalid = [&](int idx) {
        if((idx < 0) || ((unsigned)idx >= this->cells.size()))
            return;
        cellidx.push_back(idx);
    };

    auto relativepos = glm::mod(center, cellsize);

    bool left   = (relativepos.x - radius) < 0.0f;
    bool right  = (relativepos.x + radius) > cellsize.x;
    bool top    = (relativepos.y - radius) < 0.0f;
    bool bottom = (relativepos.y + radius) > cellsize.y;

    pushvalid(coordToArray(center));
    
    if(left)
        pushvalid(coordToArray(glm::vec2(center.x - radius, center.y)));
    if(right)
        pushvalid(coordToArray(glm::vec2(center.x + radius, center.y)));
    if(top)
        pushvalid(coordToArray(glm::vec2(center.x, center.y - radius)));
    if(bottom)
        pushvalid(coordToArray(glm::vec2(center.x, center.y + radius)));

    if(left && top)
        pushvalid(coordToArray(glm::vec2(center.x - radius, center.y - radius)));
    if(left && bottom)
        pushvalid(coordToArray(glm::vec2(center.x - radius, center.y + radius)));

    if(right && top)
        pushvalid(coordToArray(glm::vec2(center.x + radius, center.y - radius)));
    if(right && bottom)
        pushvalid(coordToArray(glm::vec2(center.x + radius, center.y + radius)));
    
    return cellidx;
}

void
Grid::insert(ObjPtr obj)
{
    if((obj->getRadius() >= cellsize.x) || (obj->getRadius() >= cellsize.y)) {
        // Don't even try handling objects greater than the grid!
        return;
    }

    auto v = this->getOverlappingCells(obj->getCenter(), obj->getRadius());
    for(auto cell_id : v) {
        // std::cout << "Cell id: "
        //           << (cell_id + 1) << "/"
        //           << this->cells.size() << std::endl;
        this->cells[cell_id].insert(obj);
    }
}

void
Grid::move(ObjPtr obj)
{
    if((obj->getRadius() >= cellsize.x) || (obj->getRadius() >= cellsize.y)) {
        // Don't even try handling objects greater than the grid!
        return;
    }
    
    auto oldCells = this->getOverlappingCells(
        obj->getOldCenter(),
        obj->getRadius());
    auto newCells = this->getOverlappingCells(
        obj->getCenter(),
        obj->getRadius());

    std::vector<int> removedCells;
    std::vector<int> addedCells;

    std::set_difference(oldCells.begin(), oldCells.end(),
                        newCells.begin(), newCells.end(),
                        std::inserter(removedCells, removedCells.begin()));

    std::set_difference(newCells.begin(), newCells.end(),
                        oldCells.begin(), oldCells.end(),
                        std::inserter(addedCells, addedCells.begin()));

    for(auto idx : removedCells) {
        cells[idx].erase(obj);
    }

    for(auto idx : addedCells) {
        cells[idx].insert(obj);
    }
}

void
Grid::testAll(CollisionTestFn fn)
{
    for(auto set : cells) {
        for(auto itr = set.begin(); itr != set.end(); itr++) {
            auto itr_next = itr; itr_next++;
            if(itr_next != set.end()) {
                auto pA = *itr;
                auto pB = *itr_next;
                // TODO: Report collision result to these objects
                fn(pA, pB);
            }
        }
    }
}
