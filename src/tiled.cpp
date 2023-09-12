#include "tiled.hpp"
#include "io.hpp"
#include "sprite.hpp"
#include <rapidxml/rapidxml.hpp>
#include <cstring>
#include <sstream>
#include <glm/ext.hpp>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <glm/gtx/string_cast.hpp>

namespace Tiled
{
    auto getattr = [](auto node, const char *name) -> std::string {
        auto attr = node->first_attribute(name);
        if(!attr) return "";
        return attr->value();
    };

    auto getfattr = [](auto node, const char *name) -> float {
        auto val = getattr(node, name);
        if(val == "") return 0.0f;
        return std::stof(val);
    };

    TileData::TileData(const char *path)
    {
        std::string contents = IO::slurp_file(path);
        rapidxml::xml_document<> doc;
        doc.parse<0>(const_cast<char*>(contents.c_str()));

        auto tileset = doc.first_node("tileset");

        version = getattr(tileset, "version");
        tiledversion = getattr(tileset, "tiledversion");
        name = getattr(tileset, "name");
        tilewidth = std::stoi(getattr(tileset, "tilewidth"));
        tileheight = std::stoi(getattr(tileset, "tileheight"));
        tilesize = glm::vec2(tilewidth, tileheight);
        tilecount = std::stoi(getattr(tileset, "tilecount"));
        columns = std::stoi(getattr(tileset, "columns"));

        // Each tile has a collision array
        collisionarrays.reserve(tilecount);
        for(auto i = 0; i < tilecount; i++)
            collisionarrays.push_back(std::nullopt);

        for(auto tile = tileset->first_node();
            tile != nullptr;
            tile = tile->next_sibling()) {
            if(!strcmp(tile->name(), "tile")) {
                unsigned id = std::stoi(getattr(tile, "id"));
                auto group = tile->first_node("objectgroup");
                if(group == nullptr) {
                    continue;
                }

                collisionarrays[id] = Collision::Array();
                Collision::Array *c = &*collisionarrays[id];

                for(auto obj = group->first_node();
                    obj != nullptr;
                    obj = obj->next_sibling()) {
                    glm::vec2 position, size;

                    position.x = getfattr(obj, "x");
                    position.y = getfattr(obj, "y");
                    size.x = getfattr(obj, "width");
                    size.y = getfattr(obj, "height");

                    auto child = obj->first_node();
                    if(child == nullptr) {
                        c->push_back(std::make_unique<Collision::AABB>(position, size));
                    } else {
                        std::string name = child->name();
                        if(name == "ellipse") {
                            c->push_back(std::make_unique<Collision::Ellipse>(position, size));
                        } else if(name == "point") {
                            c->push_back(std::make_unique<Collision::Point>(position));
                        } else if(name == "polygon") {
                            std::vector<glm::vec2> acc;
                            std::istringstream pairs(getattr(child, "points"));
                            while(!pairs.eof()) {
                                std::string coords;
                                pairs >> coords;
                                glm::vec2 p;

                                std::string number;
                                std::istringstream ss(coords);
                                std::getline(ss, number, ',');
                                p.x = std::stof(number);
                                std::getline(ss, number, ',');
                                p.y = std::stof(number);

                                p += position;
                                acc.push_back(p);
                            }
                            auto triangles = Collision::TrianglesFromPolygon(acc);
                            while(triangles.size() > 0) {
                                const auto it = triangles.begin();
                                c->push_back(std::move(*it));
                                triangles.erase(it);
                            }
                        }
                    }
                }
            }
        }
    }

    TileData::~TileData()
    {
        collisionarrays.clear();
    }

    TileMap::TileMap(const char *path)
    {
        std::string contents = IO::slurp_file(path);
        rapidxml::xml_document<> doc;
        doc.parse<0>(const_cast<char*>(contents.c_str()));

        auto map = doc.first_node("map");

        version = getattr(map, "version");
        tiledversion = getattr(map, "tiledversion");
        orientation = getattr(map, "orientation");
        renderorder = getattr(map, "renderorder");
        width = std::stoi(getattr(map, "width"));
        height = std::stoi(getattr(map, "height"));
        tilewidth = std::stoi(getattr(map, "tilewidth"));
        infinite = (std::stoi(getattr(map, "infinite")) == 0 ? false : true);
        nextlayerid = std::stoi(getattr(map, "nextlayerid"));
        nextobjectid = std::stoi(getattr(map, "nextobjectid"));

        // ignore "tileset" child. We'll always use a .tsx file
        // programatically associated with this map

        for(auto child = map->first_node();
            child != nullptr;
            child = child->next_sibling()) {
            if(!strcmp(child->name(), "layer")) {
                LayerData layer;
                layer.id = std::stoi(getattr(child, "id"));
                layer.name = getattr(child, "name");
                layer.width = std::stoi(getattr(child, "width"));
                layer.height = std::stoi(getattr(child, "height"));

                // Read CSV data sequentially. We can always do the math
                // and map these tiles according to layer width.
                layer.data.reserve(layer.width * layer.height);
                auto csv_child = child->first_node("data");
                std::istringstream csv(csv_child->value());
                std::string line;
                while(std::getline(csv, line)) {
                    //std::cout << line << std::endl;
                    char comma;
                    int tile;
                    std::istringstream ss(line);
                    while(!ss.eof()) {
                        ss >> tile;
                        if(!ss.eof()) {
                            // each element must always end with a comma
                            ss >> comma;
                            layer.data.push_back(tile);
                        }
                    }
                }
                layers.push_back(layer);
            } else if(!strcmp(child->name(), "objectgroup")) {
                // Any object groups are treated equally.
                for(auto object = child->first_node();
                    object != nullptr;
                    object = object->next_sibling()) {
                    MapObject o;
                    std::string name = getattr(object, "name");
                    o.type = getattr(object, "type");
                    o.position = glm::vec2(
                        getfattr(object, "x"),
                        getfattr(object, "y"));
                    objects[name] = o;
                }
            }
        }
    }

    std::vector<int>
    LayerData::getTileWindow(
        glm::vec2 cameraCenter,
        glm::vec2 viewportSize,
        glm::vec2 tilesize,
        glm::ivec2& windowSize)
    {
        std::vector<int> window;

        // Adjust camera center as if it were at a precise middle tile position
        cameraCenter -= glm::mod(cameraCenter, tilesize);

        auto xpostile = [&](float x) -> int {
            return glm::trunc(x / tilesize.x);
        };

        auto ypostile = [&](float y) -> int {
            return glm::trunc(y / tilesize.y);
        };

        // Determine tile window size
        windowSize = glm::trunc(viewportSize / tilesize);
        windowSize += glm::ivec2(2, 1);

        // Determine map coordinates of tiles according to adjusted camera
        float left = cameraCenter.x - (tilesize.x * (windowSize.x / 2));
        float right = (cameraCenter.x + tilesize.x) + (tilesize.x * (windowSize.x / 2));
        float top = cameraCenter.y - (tilesize.y * (windowSize.y / 2));
        float bottom = (cameraCenter.y + tilesize.y) + (tilesize.y * (windowSize.y / 2));    

        // Determine actual tile indexes on map grid
        int tileminx = xpostile(left);
        int tileminy = ypostile(top);
        int tilemaxx = xpostile(right);
        int tilemaxy = ypostile(bottom);


        // Recalculate actual window size
        windowSize.x = tilemaxx - tileminx + 1;
        windowSize.y = tilemaxy - tileminy + 1;

        window.reserve(windowSize.x * windowSize.y); 

        auto posToCoord = [&](int x, int y) -> int {
            return (y * this->width) + x;
        };
    
        for(int j = tileminy; j <= tilemaxy + 1; j++)
            for(int i = tileminx; i < tilemaxx + 1; i++) {
                if((i < 0) ||
                   (j < 0) ||
                   (i >= this->width) ||
                   (j >= this->height))
                    window.push_back(0);
                else
                    window.push_back(this->data[posToCoord(i, j)]);
            }

        return window;
    }

    std::optional<MapObject>
    TileMap::getObject(std::string name)
    {
        auto it = this->objects.find(name);
        if(it == this->objects.end())
            return std::nullopt;
        return it->second;
    }

    const LevelData *
    LevelDataManager::getLevel(std::string name) const
    {
        for(unsigned i = 0; i < data.size(); i++)
        {
            if(data[i].name == name) {
                return &data[i];
            }
        }
        return nullptr;
    }

    Resources::LevelPtr
    LevelData::loadLevel(unsigned zone)
    {
        if(zone >= maps_path.size())
            return nullptr;

        Resources::Manager::loadTileData(tiles_path.c_str());
        Resources::Manager::loadTileMap(maps_path[zone].c_str());

        auto tiles = Resources::Manager::getTileData(tiles_path.c_str());
        auto map   = Resources::Manager::getTileMap(maps_path[zone].c_str());

        Resources::Manager::loadAtlas(atlas_path.c_str(), tiles->tilesize);
        auto atlas = Resources::Manager::getAtlas(atlas_path.c_str());

        return std::make_shared<Level>(atlas, tiles, map);
    }

    void
    Level::drawFrontLayers(glm::vec2 cameraCenter,
                           glm::vec2 viewportSize,
                           glm::mat4& vp)
    {
        this->drawLayers(
            [](LayerData& layer) -> bool {
                return layer.name.rfind("fg") == 0; 
            },
            cameraCenter,
            viewportSize,
            vp);
    }

    void
    Level::drawBackLayers(glm::vec2 cameraCenter,
                          glm::vec2 viewportSize,
                          glm::mat4& vp)
    {
        this->drawLayers(
            [](LayerData& layer) -> bool {
                return layer.name.rfind("bg") == 0; 
            },
            cameraCenter,
            viewportSize,
            vp);
    }

    void
    Level::drawLayers(std::function<bool(LayerData&)> picker,
                      glm::vec2 cameraCenter,
                      glm::vec2 viewportSize,
                      glm::mat4& vp)
    {
        std::vector<LayerData> layers;
        std::copy_if(
            map->layers.begin(),
            map->layers.end(),
            std::back_inserter(layers),
            picker);

        for(auto it = layers.rbegin(); it != layers.rend(); it++) {
            this->drawLayer(*it, cameraCenter, viewportSize, vp);
        }
    }
    
    void
    Level::drawLayer(LayerData& layer,
                     glm::vec2 cameraCenter,
                     glm::vec2 viewportSize,
                     glm::mat4& vp)
    {
        glm::ivec2 windowSize;
        std::vector<int> window = layer.getTileWindow(
            cameraCenter,
            viewportSize,
            tiledata->tilesize,
            windowSize);

        // Render each tile on window
        int x = 0;
        int y = 0;
        for(unsigned i = 0; i < window.size(); i++) {
            if(window[i] != 0) {
                glm::vec2 cameraDiff;
                cameraDiff = glm::mod(cameraCenter, tiledata->tilesize);
                glm::mat4 level_model = glm::translate(
                    glm::mat4(1.0),
                    glm::vec3(
                        (tiledata->tilesize.x / 2) +
                        ((x - 1) * tiledata->tilesize.x) - (cameraDiff.x),
                        (tiledata->tilesize.y / 2) +
                        (y * tiledata->tilesize.y) - (cameraDiff.y),
                        0.0f)
                    );
                level_model = glm::scale(
                    level_model,
                    glm::vec3(tiledata->tilesize.x / 2.0f,
                              tiledata->tilesize.y / 2.0f,
                              1.0f));
                atlas->setFrame(window[i] - 1);
                glm::mat4 levelmvp = vp * level_model;
                atlas->draw(levelmvp);
            }
            if(x++ >= windowSize.x - 1) {
                x = 0;
                y++;
            }
        }
    }

    inline void
    print_window(std::vector<int>& window, glm::ivec2 windowSize)
    {
        int x = 0;
        for(int value : window) {
            x++;
            if(value > 0)
                std::cout << std::setw(3) << value;
            else
                std::cout << "___";
            std::cout << ' ';
            if(x >= windowSize.x) {
                std::cout << std::endl;
                x = 0;
            }
        }
        std::cout << std::endl;
    }

    // void
    // Level::drawLayer(LayerData& layer,
    //                  glm::vec2 cameraCenter,
    //                  glm::vec2 viewportSize,
    //                  glm::mat4& vp)
    // {        
    //     glm::ivec2 windowSize;
    //     std::vector<int> window = layer.getTileWindow(
    //         cameraCenter,
    //         viewportSize,
    //         tiledata->tilesize,
    //         windowSize);

    //     print_window(window, windowSize);

    //     // Calculate starting rendering point
    //     glm::vec2 startingPoint = glm::vec2(0.0f, 0.0f);
    //     startingPoint = tiledata->tilesize / 2.0f;
    //     startingPoint.y += tiledata->tilesize.y;
    //     //startingPoint = (glm::vec2(windowSize) * tiledata->tilesize) / 2.0f;
    //     // startingPoint /= 2.0f;
    //     // startingPoint -= viewportSize / 2.0f;
    //     // //startingPoint *= -1.0f;
    //     // startingPoint += tiledata->tilesize / 2.0f; // Compensate for center
        
    //     // startingPoint += tiledata->tilesize / 4.0f; // Compensate for center
    //     // startingPoint.y += tiledata->tilesize.y / 4.0f;
    //     // startingPoint.y += tiledata->tilesize.y / 8.0f;
        
    //     // Camera difference to emulate perfect camera scrolling
    //     glm::vec2 cameraDiff = glm::vec2(0.0f, 0.0f);
    //     cameraDiff = glm::mod(cameraCenter, tiledata->tilesize);

    //     std::cout << "startingpoint: " << glm::to_string(startingPoint) << std::endl;
        
    //     int x = 0, y = 0;
    //     for(unsigned i = 0; i < window.size(); i++) {
    //         if(window[i] != 0) {
    //             glm::mat4 model = glm::mat4(1.0f);
    //             //model = glm::translate(model, glm::vec3(-viewportSize / 2.0f, 0.0f));
    //             model = glm::translate(
    //                 model,
    //                 glm::vec3(
    //                     startingPoint.x - cameraDiff.x + (x * tiledata->tilesize.x),
    //                     startingPoint.y - cameraDiff.y + (y * tiledata->tilesize.y),
    //                     0.0f));
    //             model = glm::scale(
    //                 model,
    //                 glm::vec3(
    //                     tiledata->tilesize.x / 2.0f,
    //                     tiledata->tilesize.y / 2.0f,
    //                     1.0f));
    //             atlas->setFrame(window[i] - 1);
    //             glm::mat4 mvp = vp * model;
    //             atlas->draw(mvp);
    //         }

    //         if(x++ >= windowSize.x - 1) {
    //             x = 0; y++;
    //         }
    //     }
    // }
}
