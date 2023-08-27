#include "tiled.hpp"
#include "io.hpp"
#include <rapidxml/rapidxml.hpp>
#include <cstring>
#include <sstream>

TileData::TileData(const char *path)
{
    std::string contents = slurp_file(path);
    rapidxml::xml_document<> doc;
    doc.parse<0>(const_cast<char*>(contents.c_str()));

    auto tileset = doc.first_node("tileset");

    auto getattr = [&](auto node, const char *name) -> std::string {
        auto attr = node->first_attribute(name);
        if(!attr) return "";
        return attr->value();
    };

    auto getfattr = [&](auto node, const char *name) -> float {
        auto val = getattr(node, name);
        if(val == "") return 0.0f;
        return std::stof(val);
    };

    version = getattr(tileset, "version");
    tiledversion = getattr(tileset, "tiledversion");
    name = getattr(tileset, "name");
    tilewidth = std::stoi(getattr(tileset, "tilewidth"));
    tileheight = std::stoi(getattr(tileset, "tileheight"));
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

            collisionarrays[id] = CollisionArray();
            CollisionArray *c = &*collisionarrays[id];

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
                    c->push_back(new AABB(position, size));
                } else {
                    std::string name = child->name();
                    if(name == "ellipse") {
                        c->push_back(new EllipseCollision(position, size));
                    } else if(name == "point") {
                        c->push_back(new PointCollision(position));
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
                        auto triangles = TriangleCollisionFromPolygon(acc);
                        c->insert(c->end(), triangles.begin(), triangles.end());
                    }
                }
            }
        }
    }
}

TileData::~TileData()
{
    for(auto arr : collisionarrays) {
        if(arr != std::nullopt) {
            for(auto shape : *arr) {
                delete shape;
            }
        }
    }
}
