#include "title_screen.hpp"
#include <glm/ext.hpp>

#include "controls.hpp"
#include "level_select.hpp"
#include "level_scene.hpp"
#include "core.hpp"

//const glm::vec2 viewportSize(1280.0f, 896.0f);
//const glm::vec2 viewportSize(1920.0f, 1344.0f);
//const glm::vec2 viewportSize(480.0f, 336.0f);
//const glm::vec2 viewportSize(600.0f, 420.0f);
const glm::vec2 viewportSize(960.0f, 672.0f);

const char *texts[] = {
    "Press Start Button",
    "  New Game >",
    "< Level Select >",
    "< Options >",
    "< Quit  ",
};

static int currentText = 0;
const bool debugmode = false;

TitleScreen::TitleScreen() {
    currentText = 0;
}

TitleScreen::~TitleScreen() {}

void TitleScreen::load() {
    // Resources::Manager::loadAtlas("resources/sprites/title.png",
    //                               glm::vec2(390.0f, 258.0f));
    Resources::Manager::loadAtlas("resources/sprites/title.png",
                                  glm::vec2(256.0f, 175.0f));
    
    atlas = Resources::Manager::getAtlas("resources/sprites/title.png");
    atlas->setFrame(0);

    Resources::Manager::loadFont("resources/sprites/fonts/hud.png",
                                 glm::vec2(10.0f, 18.0f));
    font  = Resources::Manager::getFont("resources/sprites/fonts/hud.png");

    // Resources::Manager::loadFont("resources/sprites/fonts/levelselect.png",
    //                              glm::vec2(10.0f, 10.0f));
    // font = Resources::Manager::getFont("resources/sprites/fonts/levelselect.png");

    vp = glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, 1.0f, -1.0f);


    // Start music
    Resources::Manager::loadBGMTable("resources/audiodata.toml");
    auto bgmtable = Resources::Manager::getBGMTable("resources/audiodata.toml");
    bgm = bgmtable->load("01");
    channel = Sound::getChannel();
    if(bgm != nullptr)
        Sound::channelOf(channel)->play(bgm);
}

void TitleScreen::unload() {
    Sound::releaseChannel(channel);
}

void TitleScreen::update(double) {
    if(Controls::pressed(BTN_DIGITAL_START) || Controls::pressed(BTN_DIGITAL_ACTIONDOWN)) {
        if(debugmode && Controls::pressing(BTN_DIGITAL_ACTIONDOWN)) {
            Scenes::Manager::add(new LevelSelect());
            setShouldUnload(true);
        } else {
            switch(currentText) {
            default: break;
            case 1:
                {
                    Resources::Manager::loadLevelDataManager("resources/leveldata.toml");
                    auto manager =
                        Resources::Manager::getLevelDataManager("resources/leveldata.toml");
                    if(manager->data.size() > 0) {
                        Scenes::Manager::add(new LevelScene(manager->data[0], 0, {
                                    Player::Character::Sonic,
                                    Player::Character::Tails
                                }));
                        setShouldUnload(true);
                    }
                }
                break;
            case 2:
                Scenes::Manager::add(new LevelSelect());
                setShouldUnload(true);
                break;
            case 3: break;
            case 4:
                Core::queryClose();
                break;
            }                
        }
    }

    if(currentText == 0) {
        if(Controls::pressed(BTN_DIGITAL_START)) {
            currentText = 1;
        }
    } else {
        // Handle controls here
        if(Controls::pressed(BTN_DIGITAL_LEFT))  currentText--;
        if(Controls::pressed(BTN_DIGITAL_RIGHT)) currentText++;
        // 1 <= x <= 4
        if(currentText < 1) currentText = 1;
        if(currentText > 4) currentText = 4;
    }
}

void TitleScreen::draw() {
    glm::mat4 mvp;

    const char *text = texts[currentText];
    auto textsize = font->measure(text);

    //glm::vec2 txtVwp(320.0f, 224.0f);
    glm::vec2 txtVwp(480.0f, 336.0f);
    //glm::vec2 txtVwp(640.0f, 448.0f);
    
    glm::vec3 txtPos((txtVwp / 2.0f)
                     - (textsize / 2.0f)
                     - (font->getGlyphsize() / 2.0f)
                     ,
                     0.0f);
    txtPos.y += txtVwp.y / 16.0f * 6.0f;

    mvp = glm::ortho(0.0f, txtVwp.x, txtVwp.y, 0.0f, 1.0f, -1.0f) *
        glm::translate(glm::mat4(1.0f), txtPos);
    font->draw(mvp, text);

    glm::vec2 titlepos(viewportSize / 2.0f);
    titlepos.y -= 20.0f;

    mvp = vp *
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(titlepos, 0.0f)) *
        glm::scale(
            glm::mat4(1.0f),
            glm::vec3(atlas->getFramesize(), 1.0f));
    atlas->draw(mvp);
}
