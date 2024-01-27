#ifndef ENTITY_TEST_HPP_INCLUDED
#define ENTITY_TEST_HPP_INCLUDED

#include "engine/scene.hpp"
#include "engine/resources.hpp"
#include <memory>
#include <flecs.h>

struct Transform {
    glm::vec2 pos;
};

// typedef glm::vec2 Transform;
// typedef glm::vec2 Speed;

struct Speed {
    glm::vec2 speed;
};

struct PlayerControl{};

struct AutoControl{
    float step;
};

struct SphereRender{};

class EntityTest : public Scenes::Scene
{
private:
    glm::mat4            fontmvp;
    Resources::FontPtr   font;
    Resources::AtlasPtr  atlas;
    
    flecs::world       ecs;
public:
    EntityTest();
    virtual ~EntityTest();
    virtual void load() override;
    virtual void unload() override;
    virtual void update(double dt) override;
    virtual void draw() override;
};

#endif
