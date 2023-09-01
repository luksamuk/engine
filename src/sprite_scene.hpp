#ifndef SPRITE_SCENE_HPP_INCLUDED
#define SPRITE_SCENE_HPP_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "scene.hpp"
#include "render.hpp"
#include "sprite.hpp"
#include "resources.hpp"

class SpriteScene : public Scene
{
private:

    glm::mat4 model, view, projection;
    Animator *animator;
    SpriteFont *font;
    LevelData lvl;
    unsigned  act;
public:
    SpriteScene(LevelData l, unsigned act);
    virtual ~SpriteScene();
    virtual void load() override;
    virtual void update() override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
