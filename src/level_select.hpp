#ifndef LEVEL_SELECT_HPP_INCLUDED
#define LEVEL_SELECT_HPP_INCLUDED

#include "scene.hpp"
#include "resources.hpp"
#include "sprite.hpp"
#include <glm/glm.hpp>

class LevelSelect : public Scene
{
private:
    LevelDataManager  manager;
    SpriteFont       *font;
    SpriteAtlas      *bg;
    glm::mat4         vp, text_mvp, title_mvp;
    std::string       txt;
    int selection = 0;

    std::pair<LevelData, unsigned> fromSelection();
public:
    LevelSelect();
    virtual ~LevelSelect();
    virtual void load() override;
    virtual void update() override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
