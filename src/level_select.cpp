#include "level_select.hpp"
#include <sstream>
#include <iomanip>

#include "controls.hpp"

#include <glm/ext.hpp>

#include "sprite_scene.hpp"

//const glm::vec2 viewportSize(320.0f, 224.0f);
const glm::vec2 viewportSize(480.0f, 336.0f);

LevelSelect::LevelSelect()
{
}

LevelSelect::~LevelSelect()
{
}

void LevelSelect::load()
{
    manager = resourcesLoadLevelDataManager("resources/leveldata.toml");
    font = new SpriteFont("resources/sprites/fonts/levelselect.png",
                          glm::vec2(10.0f, 10.0f));
    bg = new SpriteAtlas("resources/background/levelselect.png",
                         glm::vec2(96.0f, 64.0f));
    bg->setFrame(0);

    vp = glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, 1.0f, -1.0f);

    text_mvp = vp *
        glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 50.0f, 0.0f));

    title_mvp = vp *
        glm::translate(glm::mat4(1.0f), glm::vec3(140.0f, 20.0f, 0.0f));
}

std::pair<LevelData, unsigned>
LevelSelect::fromSelection()
{
    int currlvl = 0;
    for(auto& level : manager.data) {
        unsigned maxlvl = level.maps_path.size();
        maxlvl = maxlvl == 0 ? 1 : maxlvl;
        for(unsigned i = 0; i < maxlvl; i++) {
            if(currlvl == selection) {
                return std::pair<LevelData, unsigned>(level, i);
            }
            currlvl++;
        }
    }

    LevelData l;
    return std::pair<LevelData, unsigned>(l, 0);
}

void LevelSelect::update()
{
    const int linesize = 18;
    
    std::ostringstream oss;
    oss.clear();

    int currlvl = 0;
    for(auto& level : manager.data) {
        unsigned maxlvl = level.maps_path.size();
        maxlvl = maxlvl == 0 ? 1 : maxlvl;
        for(unsigned i = 0; i < maxlvl; i++) {
            if(selection == currlvl) {
                oss << "* ";
            } else {
                oss << "  ";
            }
            oss << std::left
                << std::setw(linesize)
                << level.name
                << ' ';
            if(maxlvl > 1)
                oss << (i + 1);
            oss << std::endl;
            currlvl++;
        }
        oss << std::endl;
    }
    txt = oss.str();

    if(controlsPressed(BTN_DIGITAL_UP))
        selection--;
    if(controlsPressed(BTN_DIGITAL_DOWN))
        selection++;

    if(selection >= currlvl) selection = 0;
    if(selection < 0) selection = currlvl - 1;

    if(controlsPressed(BTN_DIGITAL_START)) {
        auto lvldata = fromSelection();
        if(lvldata.first.maps_path.size() > 0) {
            SceneManager::add(new SpriteScene(lvldata.first, lvldata.second));
            setShouldUnload(true);
        }
    }
}

void LevelSelect::draw()
{
    // Draw title
    font->draw(title_mvp, "*** Level Select ***");
    
    // Draw selection text
    font->draw(text_mvp, txt.c_str());

    glm::mat4 bg_scale = glm::scale(glm::mat4(1.0f),
                                    glm::vec3(bg->getFramesize() / 2.0f, 1.0f));
    for(float x = 0.0f;
        x < viewportSize.x + bg->getFramesize().x;
        x += bg->getFramesize().x)
    {
        for(float y = 0.0f;
            y < viewportSize.y + bg->getFramesize().y;
            y += bg->getFramesize().y)
        {
            glm::mat4 bg_model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
            glm::mat4 bg_mvp = vp * bg_model * bg_scale;
            bg->draw(bg_mvp);
        }
    }
}

void LevelSelect::unload()
{
    delete font;
    delete bg;
}
