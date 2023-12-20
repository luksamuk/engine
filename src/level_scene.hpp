#ifndef LEVEL_SCENE_HPP_INCLUDED
#define LEVEL_SCENE_HPP_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <flecs.h>

#include "scene.hpp"
#include "tiled.hpp"
#include "player_constants.hpp"
#include "sound.hpp"

class LevelScene : public Scenes::Scene
{
private:
    flecs::world ecs;
    std::vector<Player::Character> chars;
    Sound::AudioSourceIndex channel;
    Resources::AudioPtr     bgm;

    flecs::entity makePlayer(const char *, Player::Character, flecs::entity*);
public:
    LevelScene(Tiled::LevelData l, unsigned act, std::vector<Player::Character>);
    virtual ~LevelScene();
    virtual void load() override;
    virtual void update(double dt) override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
