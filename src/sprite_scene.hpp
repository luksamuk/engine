#ifndef SPRITE_SCENE_HPP_INCLUDED
#define SPRITE_SCENE_HPP_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "scene.hpp"
#include "render.hpp"
#include "sprite.hpp"
#include "resources.hpp"
#include "tiled.hpp"

class SpriteScene : public Scenes::Scene
{
private:

    glm::mat4 model, view, projection;
    Resources::AnimatorPtr animator;
    Resources::FontPtr     font;
    Tiled::LevelData       lvldata;
    Resources::LevelPtr    lvl;
    unsigned  act;

    Resources::AnimatorPtr sonicAnim;
    Resources::AnimatorPtr knucklesAnim;
    Resources::AnimatorPtr sonicManiaAnim;

    void changeCharacter(unsigned);
public:
    SpriteScene(Tiled::LevelData l, unsigned act);
    virtual ~SpriteScene();
    virtual void load() override;
    virtual void update() override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
