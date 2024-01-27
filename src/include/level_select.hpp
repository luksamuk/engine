#ifndef LEVEL_SELECT_HPP_INCLUDED
#define LEVEL_SELECT_HPP_INCLUDED

#include "engine/scene.hpp"
#include "engine/sound.hpp"
#include "engine/resources.hpp"
#include "engine/sprite.hpp"
#include <glm/glm.hpp>

class LevelSelect : public Scenes::Scene
{
private:
    Resources::LevelDataManagerPtr manager;
    Resources::FontPtr             font;
    Resources::AnimatorPtr         bganim;
    
    glm::mat4         vp, text_mvp, title_mvp;
    std::string       txt;
    int selection = 0;

    Sound::AudioSourceIndex channel;
    Resources::AudioPtr     bgm;

    std::pair<Tiled::LevelData, unsigned> fromSelection();
public:
    LevelSelect();
    virtual ~LevelSelect();
    virtual void load() override;
    virtual void update(double dt) override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
