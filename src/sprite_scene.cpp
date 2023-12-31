#include "sprite_scene.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "resources.hpp"

#include <glm/ext.hpp>

#include "tiled.hpp"
#include "level_select.hpp"
#include "core.hpp"

#include <iostream>
#include <typeinfo>
#include <sstream>

static glm::vec2            cameraCenter;
static glm::vec2            charPos;
static float                direction = 1.0f;

const glm::vec2 viewportSize(320.0f, 224.0f);
//const glm::vec2 viewportSize(640.0f, 360.0f);

static unsigned ANIM_LOOKUP,
    ANIM_CROUCHDOWN,
    ANIM_RUNNING,
    ANIM_IDLE,
    ANIM_ROLLING;
    

SpriteScene::SpriteScene(Tiled::LevelData l, unsigned act)
{
    lvldata = l;
    this->act = act;
    animator = nullptr;
    direction = 1.0f;
}

SpriteScene::~SpriteScene() {}

void
SpriteScene::changeCharacter(unsigned chara)
{
    switch(chara) {
    default:
        animator = sonicAnim;
        break;
    case 1:
        animator = knucklesAnim;
        break;
    case 2:
        animator = sonicManiaAnim;
        break;
    }
    animator->setAnimation(0);

    ANIM_LOOKUP     = animator->getAnimationByName("Looking Up");
    ANIM_CROUCHDOWN = animator->getAnimationByName("Crouching Down");
    ANIM_RUNNING    = animator->getAnimationByName("Walking");
    ANIM_IDLE       = animator->getAnimationByName("Idle");
    ANIM_ROLLING    = animator->getAnimationByName("Rolling");
}

void
SpriteScene::load()
{
    Resources::Manager::loadAnimator("resources/animation/tails.toml");
    Resources::Manager::loadAnimator("resources/animation/knuckles.toml");
    Resources::Manager::loadAnimator("resources/animation/sonic1mania.toml");
    Resources::Manager::loadFont("resources/sprites/fonts/levelselect.png",
                                 glm::vec2(10.0f, 10.0f));
    Resources::Manager::loadBGMTable("resources/audiodata.toml");
    
    auto bgmtable = Resources::Manager::getBGMTable("resources/audiodata.toml");
    bgm = bgmtable->load(lvldata.bgm);
    channel = Sound::getChannel();

    sonicAnim    = Resources::Manager::getAnimator("resources/animation/tails.toml");
    knucklesAnim = Resources::Manager::getAnimator("resources/animation/knuckles.toml");
    sonicManiaAnim = Resources::Manager::getAnimator("resources/animation/sonic1mania.toml");
    
    changeCharacter(2);

    font = Resources::Manager::getFont("resources/sprites/fonts/levelselect.png");

    cameraCenter = viewportSize / 2.0f;

    // Loading level data
    lvl = lvldata.loadLevel(act);
    grid = std::make_unique<Grid>(
        glm::vec2(lvl->map->width * lvl->atlas->getFramesize().x,
                  lvl->map->height * lvl->atlas->getFramesize().y),
        lvl->atlas->getFramesize());

    // TODO: load objects

    auto spawnpoint = lvl->map->getObject("spawn_sonic");
    if(spawnpoint) {
        cameraCenter = spawnpoint->position;
    }

    Render::setClearColor(glm::vec4(0.392f, 0.584f, 0.929f, 1.0f));
    if(bgm != nullptr)
        Sound::channelOf(channel)->play(bgm);
}

void SpriteScene::unload() {
    Sound::releaseChannel(channel);
}

void SpriteScene::update(double dt)
{
    static double oldReportTime = 0.0;
    double currentReportTime = glfwGetTime();

    if(currentReportTime - oldReportTime > 2.0) {
        if(dt > 0.0f) {
            std::ostringstream oss;
            oss.clear();
            oss << "FPS: " << 1.0 / dt;
            Core::setWindowCaptionAppendix(oss.str());
        }
        oldReportTime = currentReportTime;
    }

    // Update objects
    for(auto objptr : objects) {
        objptr->update(dt);
        grid->move(objptr);
    }

    // Perform collision detection
    grid->testAll(
        [](ObjPtr pA, ObjPtr pB) -> std::optional<glm::vec2> {
            float dist = glm::distance(pA->getCenter(), pB->getCenter());
            if(dist < (pA->getRadius() + pB->getRadius())) {
                // TODO: Perform finer test
            }

            return std::nullopt;
        });


    static glm::vec3 position = glm::vec3(viewportSize / 2.0f, 0.0f);

    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }

    animator->update();
    
    //position.x = 0.9 * glm::cos(1 * (float)glfwGetTime());
    //position.y = 0.9 * glm::sin(2 * (float)glfwGetTime());

    // auto mouse = controlsMousePos();
    // position.x = mouse.x;
    // position.y = mouse.y;

    // Model matrix should reflect the sprite size
    // through scaling, and then apply a translation to the center
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(direction * 30.0f, 30.0f, 1.0f));

    float moveSpeed = 480.0f * dt;

    if(Controls::pressing(BTN_DIGITAL_ACTIONDOWN)) {
        animator->setAnimation(ANIM_ROLLING);
    } else {
         if(Controls::pressing(BTN_DIGITAL_UP)) {
             cameraCenter.y -= moveSpeed;
             if(!Controls::pressing(BTN_DIGITAL_LEFT)
                && !Controls::pressing(BTN_DIGITAL_RIGHT))
                 animator->setAnimation(ANIM_LOOKUP);
         }
         if(Controls::pressing(BTN_DIGITAL_DOWN)) {
             cameraCenter.y += moveSpeed;
             if(!Controls::pressing(BTN_DIGITAL_LEFT)
                && !Controls::pressing(BTN_DIGITAL_RIGHT))
                 animator->setAnimation(ANIM_CROUCHDOWN);
         }
         if(Controls::pressing(BTN_DIGITAL_LEFT)) {
             cameraCenter.x -= moveSpeed;
             animator->setAnimation(ANIM_RUNNING);
             direction = -1.0f;
         }
         if(Controls::pressing(BTN_DIGITAL_RIGHT)) {
             cameraCenter.x += moveSpeed;
             animator->setAnimation(ANIM_RUNNING);
             direction = 1.0f;
         }

         if(!Controls::pressing(BTN_DIGITAL_UP) &&
            !Controls::pressing(BTN_DIGITAL_DOWN) &&
            !Controls::pressing(BTN_DIGITAL_LEFT) &&
            !Controls::pressing(BTN_DIGITAL_RIGHT))
             animator->setAnimation(ANIM_IDLE);
     }

    // Camera minimum limits
    //cameraCenter = charPos;
    //auto cameraPadding = viewportSize / 8.0f;
    auto cameraPadding = glm::vec2(64.0f, 64.0f);
    auto levelSize = glm::vec2(
        (lvl->map->width - 1) * lvl->tiledata->tilewidth,
        (lvl->map->height - 1) * lvl->tiledata->tileheight);
    //cameraCenter = glm::max(cameraPadding, cameraCenter);
    //cameraCenter = glm::min(levelSize - cameraPadding, cameraCenter);

    
    if(Controls::pressed(BTN_DIGITAL_ACTIONUP)) {
        changeCharacter(1);
    } else if(Controls::pressed(BTN_DIGITAL_ACTIONLEFT)) {
        changeCharacter(0);
    } else if(Controls::pressed(BTN_DIGITAL_ACTIONRIGHT)) {
        changeCharacter(2);
    }

    
    view = glm::mat4(1.0f);
    //projection = glm::ortho(0.0f, 640.0f, 0.0f, 360.0f, 1.0f, -1.0f);
    //projection = glm::ortho(0.0f, 640.0f, 360.0f, 0.0f, 1.0f, -1.0f);
    //projection = glm::ortho(0.0f, 320.0f, 224.0f, 0.0f, 1.0f, -1.0f);
    //projection = glm::ortho(0.0f, 320.0f, 180.0f, 0.0f, 1.0f, -1.0f);
    projection = glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, 1.0f, -1.0f);
}
//#include<iomanip>
void SpriteScene::draw()
{
    glm::mat4 font_mvp;

    glm::mat4 char_model;
    char_model = glm::mat4(1.0f);
    char_model = glm::translate(char_model, glm::vec3(charPos, 0.0f));
    char_model = glm::scale(char_model, glm::vec3(direction * 30.0f, 30.0f, 1.0f));
    
    // Sprite MVP
    glm::mat4 char_mvp =
        projection *
        view *
        char_model;

    // Camera MVP
    //glm::mat4 mvp = projection * view * model;

    font_mvp =
        projection *
        view *
        glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 0.0f));

    // Text
    {
        std::ostringstream oss;
        oss.clear();
        oss << "X:         " << cameraCenter.x << std::endl
            << "Y:         " << cameraCenter.y << std::endl
            // << "Direction: " << (direction < 0.0f ? "Left" : "Right") << std::endl
            // << "Animation: " << animator->getAnimation() << std::endl
            << "Gamepad:   " << (Controls::isGamepad() ? "Yes" : "No");
        font->draw(font_mvp, oss.str().c_str());
    }

    glm::mat4 vp = projection * view;
    lvl->drawFrontLayers(cameraCenter, viewportSize, vp);
    //animator->draw(mvp);
    for(auto obj : objects) {
        // TODO: Only draw stuff within the screen!
        obj->draw(vp);
    }
    lvl->drawBackLayers(cameraCenter, viewportSize, vp);
}
