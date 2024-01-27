#ifndef TILED_HPP_INCLUDED
#define TILED_HPP_INCLUDED

#include <string>
#include "engine/collision.hpp"
#include <optional>
#include <map>
#include "engine/resources.hpp"
#include <functional>

// Tools and structure to load Tiled file data
namespace Tiled
{
    // Relates to .tsx files.
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
    
        std::vector<std::optional<Collision::Array>> collisionarrays;
    
    
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

    struct MapObject
    {
        std::string type;
        glm::vec2 position;
    };

    // Relates to .tmx files.
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
        std::map<std::string, MapObject> objects;

        TileMap(const char *path);

        std::optional<MapObject> getObject(std::string name);
    };

    struct Level
    {
        Resources::AtlasPtr    atlas;
        Resources::TileDataPtr tiledata;
        Resources::TileMapPtr  map;

        Level(Resources::AtlasPtr atlas,
              Resources::TileDataPtr tiledata,
              Resources::TileMapPtr map)
            : atlas(atlas), tiledata(tiledata), map(map) {}

        void drawFrontLayers(glm::vec2 cameraCenter, glm::vec2 viewportSize, glm::mat4& vp);
        void drawBackLayers(glm::vec2 cameraCenter, glm::vec2 viewportSize, glm::mat4& vp);
    private:
        void drawLayers(std::function<bool(LayerData&)> picker, glm::vec2 cameraCenter, glm::vec2 viewportSize, glm::mat4& vp);
        void drawLayer(LayerData& layer, glm::vec2 cameraCenter, glm::vec2 viewportSize, glm::mat4& vp);
    };

    struct LevelData
    {
        std::string name;
        std::string bgm;
        std::string atlas_path;
        std::string tiles_path;
        std::vector<std::string> maps_path;

        Resources::LevelPtr    loadLevel(unsigned);
    };

    struct LevelDataManager
    {
        std::vector<LevelData> data;

        const LevelData *getLevel(std::string name) const;
    };

}

#endif
