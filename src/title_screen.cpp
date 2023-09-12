#include "title_screen.hpp"
#include <glm/ext.hpp>

#include "controls.hpp"
#include "level_select.hpp"
#include "sprite_scene.hpp"

const glm::vec2 viewportSize(1280.0f, 896.0f);
//const glm::vec2 viewportSize(1920.0f, 1344.0f);

TitleScreen::TitleScreen() {}
TitleScreen::~TitleScreen() {}

void TitleScreen::load() {
    Resources::Manager::loadAtlas(
        "resources/sprites/title.png",
        glm::vec2(390.0f, 258.0f));
    atlas = Resources::Manager::getAtlas("resources/sprites/title.png");
    atlas->setFrame(0);

    vp = glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, 1.0f, -1.0f);
}

void TitleScreen::unload() {}

void TitleScreen::update(double dt) {
    if(Controls::pressed(BTN_DIGITAL_START)) {
        if(Controls::pressing(BTN_DIGITAL_ACTIONDOWN)) {
            Scenes::Manager::add(new LevelSelect());
            setShouldUnload(true);
        } else {
            Resources::Manager::loadLevelDataManager("resources/leveldata.toml");
            auto manager =
                Resources::Manager::getLevelDataManager("resources/leveldata.toml");
            if(manager->data.size() > 0) {
                Scenes::Manager::add(new SpriteScene(manager->data[0], 0));
                setShouldUnload(true);
            }
        }
    }
}

void TitleScreen::draw() {
    glm::mat4 mvp = vp *
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(viewportSize / 2.0f, 0.0f)) *
        glm::scale(
            glm::mat4(1.0f),
            glm::vec3(atlas->getFramesize(), 1.0f));
    atlas->draw(mvp);
}
