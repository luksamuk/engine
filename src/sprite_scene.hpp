#ifndef SPRITE_SCENE_HPP_INCLUDED
#define SPRITE_SCENE_HPP_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "scene.hpp"
#include "render.hpp"
#include "sprite.hpp"
#include "resources.hpp"

class SpriteScene : public Scenes::Scene
{
private:

    glm::mat4 model, view, projection;
    std::shared_ptr<Sprite::Animator> animator;
    Sprite::Font *font;
    Resources::LevelData lvl;
    unsigned  act;

    void changeCharacter(unsigned);
public:
    SpriteScene(Resources::LevelData l, unsigned act);
    virtual ~SpriteScene();
    virtual void load() override;
    virtual void update() override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
