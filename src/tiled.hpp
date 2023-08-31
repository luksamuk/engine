#ifndef TILED_HPP_INCLUDED
#define TILED_HPP_INCLUDED

#include <string>
#include "collision.hpp"
#include <optional>

// Tools and structure to load Tiled file data

// Relates to .tsx files. Possible data:
// => tag: tileset
//    - version
//    - tiledversion
//    - name (tileset name)
//    - tilewidth
//    - tileheight
//    - tilecount
//    - columns
// => tag: image
//    - source (filename)
//    - trans (transparent color)
//    - width (image width)
//    - height (image height)
struct TileData
{
    std::string version;
    std::string tiledversion;
    std::string name;
    glm::vec2 tilesize;
    int tilewidth;
    int tileheight;
    int tilecount;
    int columns;
    
    std::vector<std::optional<CollisionArray>> collisionarrays;
    
    
    TileData(const char *path);
    ~TileData();
};

// Relates to a single layer in a .tmx file.
struct LayerData
{
    int id;
    std::string name;
    int width;
    int height;
    // REMEMBER: TILES START COUNTING AT 1. TILE #0, FROM A LEVEL LAYER
    // PERSPECTIVE, IS ACTUALLY AN EMPTY TILE. SO YOU SHOULD ALWAYS MAP A
    // LAYER'S TILE X TO (X - 1) ON TILE DATA, AND TREAT LAYER TILE 0 AS EMPTY
    std::vector<int> data;

    std::vector<int> getTileWindow(
        glm::vec2 cameraCenter,
        glm::vec2 viewportWidth,
        glm::vec2 tilesize,
        glm::ivec2& windowSize);
};

// Relates to .tmx files. Possible data:
// .TMX file:
// => tag: map
//    - version
//    - tiledversion
//    - orientation
//    - renderorder
//    - width
//    - height
//    - tilewidth
//    - infinite (int)
//    - nextlayerid (?)
//    - nextobjectid (?)
struct TileMap
{
    std::string version;
    std::string tiledversion;
    std::string orientation;
    std::string renderorder;
    int  width;
    int  height;
    int  tilewidth;
    bool infinite;
    int  nextlayerid;
    int  nextobjectid;
    std::vector<LayerData> layers;

    TileMap(const char *path);
};

#endif
