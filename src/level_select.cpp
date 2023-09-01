#include "level_select.hpp"
#include <sstream>
#include <iomanip>

#include "controls.hpp"

#include <glm/ext.hpp>

#include "sprite_scene.hpp"

const glm::vec2 viewportSize(320.0f, 224.0f);

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

    mvp =
        glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, 1.0f, -1.0f) *
        glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 0.0f));
}

std::pair<LevelData, unsigned>
LevelSelect::fromSelection()
{
    int currlvl = 0;
    for(auto& level : manager.data) {
        for(unsigned i = 0; i < level.maps_path.size(); i++) {
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
        for(unsigned i = 0; i < maxlvl; i++) {
            if(selection == currlvl) {
                oss << "> ";
            } else {
                oss << "  ";
            }
            oss << std::setw(linesize)
                << level.name
                << ' '
                << "zone ";
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
        if(lvldata.first.name != "") {
            SceneManager::add(new SpriteScene(lvldata.first, lvldata.second));
            setShouldUnload(true);
        }
    }
}

void LevelSelect::draw()
{
    font->draw(mvp, txt.c_str());
}

void LevelSelect::unload()
{
    delete font;
}
