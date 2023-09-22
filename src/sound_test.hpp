#ifndef SOUND_TEST_HPP_INCLUDED
#define SOUND_TEST_HPP_INCLUDED

#include "scene.hpp"
#include "resources.hpp"
#include "sprite.hpp"
#include "sound.hpp"
#include <glm/glm.hpp>

class SoundTest : public Scenes::Scene
{
private:
    Resources::BGMTablePtr   table;
    Sound::AudioSourceIndex  channel;
    Resources::AudioPtr      bgm;
    Resources::FontPtr       font;
    glm::mat4                vp;
    std::string              txt;
    int selection;
    Resources::AtlasPtr      atlas;
public:
    SoundTest();
    virtual ~SoundTest();
    virtual void load() override;
    virtual void unload() override;
    virtual void update(double dt) override;
    virtual void draw() override;
};

#endif
