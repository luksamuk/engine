#include "sound_test.hpp"
#include "controls.hpp"
#include "core.hpp"
#include "resources.hpp"
#include "sprite.hpp"
#include "sound.hpp"
#include "level_select.hpp"
#include <glm/ext.hpp>
#include <sstream>
#include <iomanip>
#include <iostream>

const glm::vec2 viewportSize(480.0f, 336.0f);

SoundTest::SoundTest() {}
SoundTest::~SoundTest() {}

void
SoundTest::load()
{
    Resources::Manager::loadBGMTable("resources/bgmdata.toml");
    Resources::Manager::loadFont("resources/sprites/fonts/hud.png",
                                 glm::vec2(10.0f, 18.0f));
    Resources::Manager::loadAtlas("resources/sprites/soundtest.png",
                                  glm::vec2(320.0f, 224.0f));
    
    table = Resources::Manager::getBGMTable("resources/bgmdata.toml");
    font  = Resources::Manager::getFont("resources/sprites/fonts/hud.png");
    atlas = Resources::Manager::getAtlas("resources/sprites/soundtest.png");
    atlas->setFrame(0);
    
    vp = glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, 1.0f, -1.0f);
    source = Sound::makeSource();
    bgm = nullptr;
    selection = 0;

    for(auto pair : table->entries) {
        std::cout << "\"" << pair.first << "\": " << pair.second.file << std::endl;
    }
}

void SoundTest::unload() {}

void
SoundTest::update(double)
{
    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }

    if(Controls::pressed(BTN_DIGITAL_LEFT)) {
        selection--;
    }

    if(Controls::pressed(BTN_DIGITAL_RIGHT)) {
        selection++;
    }

    if(selection < 0) selection = 255;
    if(selection >= 256) selection = 0;

    std::ostringstream oss;
    oss.str("");
    oss << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << selection;
    txt = oss.str();

    if(Controls::pressed(BTN_DIGITAL_START)) {
        source->stop();
        Resources::Manager::garbageCollect();

        if(selection > 0) {
            std::cout << "Loading song \"" << txt << "\"..." << std::endl;
            bgm = table->load(txt);
            if(bgm != nullptr)
                source->play(bgm);
        }
    }
}

void
SoundTest::draw()
{
    glm::mat4 mvp;
    
    auto textsize = font->measure("Sound Test");
    glm::vec3 txtPos((viewportSize / 2.0f)
                     - (textsize / 2.0f)
                     - (font->getGlyphsize() / 2.0f)
                     ,
                     0.0f);
    txtPos.y -= viewportSize.y / 16.0f * 1.0f;
    mvp = glm::translate(vp, txtPos);
    font->draw(mvp, "Sound Test");

    textsize = font->measure(txt.c_str());
    txtPos = glm::vec3((viewportSize / 2.0f)
                       - (textsize / 2.0f)
                       - (font->getGlyphsize() / 2.0f)
                       ,
                       0.0f);
    txtPos.y += viewportSize.y / 16.0f * 1.0f;
    mvp = glm::translate(vp, txtPos);
    font->draw(mvp, txt.c_str());

    // mvp = glm::ortho(0.0f, 320.0f, 224.0f, 0.0f, 1.0f, -1.0f);
    // mvp = glm::scale(mvp, glm::vec3(160.0f, 112.0f, 1.0f));
    mvp = glm::translate(vp, glm::vec3(viewportSize / 2.0f, 0.0f));
    mvp = glm::scale(mvp, glm::vec3(viewportSize / 2.0f, 1.0f));
    atlas->draw(mvp);
}
