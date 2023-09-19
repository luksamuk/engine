#ifndef ENTITY_TEST_HPP_INCLUDED
#define ENTITY_TEST_HPP_INCLUDED

#include "scene.hpp"
#include "resources.hpp"
#include <memory>
#include <flecs.h>

class EntityTest : public Scenes::Scene
{
private:
    flecs::world ecs;
public:
    EntityTest();
    virtual ~EntityTest();
    virtual void load() override;
    virtual void unload() override;
    virtual void update(double dt) override;
    virtual void draw() override;
};

#endif
