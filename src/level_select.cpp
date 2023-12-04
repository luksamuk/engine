#include "level_select.hpp"
#include <sstream>
#include <iomanip>
#include <functional>

#include "controls.hpp"
#include "core.hpp"

#include <glm/ext.hpp>

#include "level_scene.hpp"
#include "sprite_scene.hpp"
#include "test_scene.hpp"
#include "movie_scene.hpp"
#include "partition_test.hpp"
#include "title_screen.hpp"
#include "entity_test.hpp"
#include "sound_test.hpp"

//const glm::vec2 viewportSize(320.0f, 224.0f);
const glm::vec2 viewportSize(480.0f, 336.0f);
//const glm::vec2 viewportSize(400.0f, 280.0f);
//const glm::vec2 viewportSize(960.0f, 672.0f);

typedef std::function<Scenes::Scene*()> SceneCreatorFn;

struct SceneEntry {
    std::string name;
    SceneCreatorFn spawner;
};

const std::vector<SceneEntry> extra_scenes = {
    {"Sound Test",     []() { return new SoundTest();     }},
    {"Rendering Test", []() { return new TestScene();     }},
    {"Animation Test", []() { return new MovieScene();    }},
    {"Collision Test", []() { return new PartitionTest(); }},
    {"Entity Test",    []() { return new EntityTest();    }},
    {"Component Test", []() { return new LevelScene({},0); }},
    {"Title Screen",   []() { return new TitleScreen();   }},
    {"Exit",           []() {
        Core::queryClose();
        return nullptr;
    }},
};

LevelSelect::LevelSelect() {}

LevelSelect::~LevelSelect() {}

void LevelSelect::load()
{
    Resources::Manager::loadFont("resources/sprites/fonts/levelselect.png",
                                 glm::vec2(10.0f, 10.0f));
    Resources::Manager::loadLevelDataManager("resources/leveldata.toml");
    Resources::Manager::loadAnimator("resources/animation/levelselect.toml");
    
    bganim = Resources::Manager::getAnimator("resources/animation/levelselect.toml");
    manager = Resources::Manager::getLevelDataManager("resources/leveldata.toml");
    font = Resources::Manager::getFont("resources/sprites/fonts/levelselect.png");
    bganim->setAnimation(0);
    
    vp = glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, 1.0f, -1.0f);

    text_mvp = vp *
        glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 50.0f, 0.0f));

    title_mvp = vp *
        glm::translate(glm::mat4(1.0f), glm::vec3(140.0f, 20.0f, 0.0f));
    //glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 20.0f, 0.0f));
    
    Render::setClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    Core::setWindowCaptionAppendix("");

    // Start music
    Resources::Manager::loadBGMTable("resources/audiodata.toml");
    auto bgmtable = Resources::Manager::getBGMTable("resources/audiodata.toml");
    bgm = bgmtable->load("02");
    channel = Sound::getChannel();
    if(bgm != nullptr)
        Sound::channelOf(channel)->play(bgm);
}

std::pair<Tiled::LevelData, unsigned>
LevelSelect::fromSelection()
{
    int currlvl = 0;
    for(auto& level : manager->data) {
        unsigned maxlvl = level.maps_path.size();
        maxlvl = maxlvl == 0 ? 1 : maxlvl;
        for(unsigned i = 0; i < maxlvl; i++) {
            if(currlvl == selection) {
                return std::pair<Tiled::LevelData, unsigned>(level, i);
            }
            currlvl++;
        }
    }

    Tiled::LevelData l;
    return std::pair<Tiled::LevelData, unsigned>(l, 0);
}

void LevelSelect::update(double dt)
{
    //const int linesize = 23;
    const int linesize = 16;
    
    std::ostringstream oss;
    oss.clear();

    int currlvl = 0;
    int numlvls = 0;
    for(auto& level : manager->data) {
        unsigned maxlvl = level.maps_path.size();
        maxlvl = maxlvl == 0 ? 1 : maxlvl;
        for(unsigned i = 0; i < maxlvl; i++) {
            if(selection == currlvl) {
                oss << "* ";
            } else {
                oss << "  ";
            }
            oss << std::left
                << std::setw(linesize);
            oss << (i == 0 ? level.name : " ");
            //oss << " Zone";
            oss << ' ';
            if(maxlvl > 1)
                oss << (i + 1);
            oss << std::endl;
            currlvl++;
            numlvls++;
        }
        oss << std::endl;
    }
    for(auto& entry : extra_scenes) {
        if(selection == currlvl) {
            oss << "* ";
        } else {
            oss << "  ";
        }
        oss << std::left
            << std::setw(linesize)
            << entry.name
            << std::endl;
        currlvl++;
    }
    
    txt = oss.str();

    if(Controls::pressed(BTN_DIGITAL_UP))
        selection--;
    if(Controls::pressed(BTN_DIGITAL_DOWN))
        selection++;

    if(selection >= currlvl) selection = 0;
    if(selection < 0) selection = currlvl - 1;

    if(Controls::pressed(BTN_DIGITAL_START) || Controls::pressed(BTN_DIGITAL_ACTIONDOWN)) {
        if(selection < numlvls) {
            auto lvldata = fromSelection();
            if(lvldata.first.maps_path.size() > 0) {
                Scenes::Manager::add(new SpriteScene(lvldata.first, lvldata.second));
                setShouldUnload(true);
            }
        } else {
            int sel = selection - numlvls;
            auto scene = extra_scenes[sel].spawner();
            if(scene) {
                Scenes::Manager::add(scene);
                setShouldUnload(true);
            }
        }
    }

    bganim->update();
}

void LevelSelect::draw()
{
    // Draw title
    font->draw(title_mvp, "*** Level Select ***");
    
    // Draw selection text
    font->draw(text_mvp, txt.c_str());

    glm::mat4 bg_scale = glm::scale(glm::mat4(1.0f),
                                    glm::vec3(bganim->getFramesize() / 2.0f, 1.0f));
    for(float x = 0.0f;
        x < viewportSize.x + bganim->getFramesize().x;
        x += bganim->getFramesize().x)
    {
        for(float y = 0.0f;
            y < viewportSize.y + bganim->getFramesize().y;
            y += bganim->getFramesize().y)
        {
            glm::mat4 bg_model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
            glm::mat4 bg_mvp = vp * bg_model * bg_scale;
            bganim->draw(bg_mvp);
        }
    }
}

void LevelSelect::unload()
{
    Sound::releaseChannel(channel);
}
