#ifndef TITLE_SCREEN_HPP_INCLUDED
#define TITLE_SCREEN_HPP_INCLUDED

#include "engine/scene.hpp"
#include "engine/sound.hpp"
#include "engine/resources.hpp"
#include "engine/sprite.hpp"
#include <glm/glm.hpp>

class TitleScreen : public Scenes::Scene
{
private:
    Resources::AtlasPtr     atlas;
    Resources::FontPtr      font;
    Resources::FontPtr      info_font;
    Sound::AudioSourceIndex channel;
    Resources::AudioPtr     bgm;
    glm::mat4 vp;
public:
    TitleScreen();
    ~TitleScreen();
    virtual void load() override;
    virtual void update(double dt) override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
