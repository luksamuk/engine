#ifndef LEVEL_SCENE_HPP_INCLUDED
#define LEVEL_SCENE_HPP_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <flecs.h>

#include "scene.hpp"
#include "tiled.hpp"

class LevelScene : public Scenes::Scene
{
private:
    flecs::world ecs;
public:
    LevelScene(Tiled::LevelData l, unsigned act);
    virtual ~LevelScene();
    virtual void load() override;
    virtual void update(double dt) override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
