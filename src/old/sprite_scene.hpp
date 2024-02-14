#ifndef SPRITE_SCENE_HPP_INCLUDED
#define SPRITE_SCENE_HPP_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "engine/scene.hpp"
#include "engine/render.hpp"
#include "engine/sprite.hpp"
#include "engine/resources.hpp"
#include "engine/tiled.hpp"
#include "game_object.hpp"
#include "partitioning.hpp"
#include "engine/sound.hpp"

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

    std::unique_ptr<Grid>  grid;
    std::vector<ObjPtr>    objects;

    Sound::AudioSourceIndex channel;
    Resources::AudioPtr     bgm;

    void changeCharacter(unsigned);
public:
    SpriteScene(Tiled::LevelData l, unsigned act);
    virtual ~SpriteScene();
    virtual void load() override;
    virtual void update(double dt) override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
