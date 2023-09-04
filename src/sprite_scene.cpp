#include "sprite_scene.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "resources.hpp"

#include <glm/ext.hpp>

#include "tiled.hpp"
#include "level_select.hpp"

#include <iostream>
#include <typeinfo>
#include <sstream>

static Resources::AnimatorPtr movie;

static Resources::AtlasPtr  chunks;
static Tiled::TileData     *tiles;
static Tiled::TileMap      *map;
static glm::vec2            cameraCenter;
static float                direction = 1.0f;

const glm::vec2 viewportSize(320.0f, 224.0f);
//const glm::vec2 viewportSize(640.0f, 360.0f);

static unsigned ANIM_LOOKUP,
    ANIM_CROUCHDOWN,
    ANIM_RUNNING,
    ANIM_IDLE,
    ANIM_ROLLING;
    

SpriteScene::SpriteScene(Resources::LevelData l, unsigned act)
{
    lvl = l;
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
    Resources::Manager::loadAnimator("resources/animation/movie.toml");
    Resources::Manager::loadAnimator("resources/animation/sonic.toml");
    Resources::Manager::loadAnimator("resources/animation/knuckles.toml");
    Resources::Manager::loadAnimator("resources/animation/sonic1mania.toml");
    Resources::Manager::loadFont("resources/sprites/fonts/levelselect.png",
                                 glm::vec2(10.0f, 10.0f));

    sonicAnim    = Resources::Manager::getAnimator("resources/animation/sonic.toml");
    knucklesAnim = Resources::Manager::getAnimator("resources/animation/knuckles.toml");
    sonicManiaAnim = Resources::Manager::getAnimator("resources/animation/sonic1mania.toml");
    
    changeCharacter(2);

    movie = Resources::Manager::getAnimator("resources/animation/movie.toml");
    movie->setAnimation(0);

    font = Resources::Manager::getFont("resources/sprites/fonts/levelselect.png");

    cameraCenter = viewportSize / 2.0f;

    // Loading level data
    tiles = new Tiled::TileData(lvl.tiles_path.c_str());
    Resources::Manager::loadAtlas(lvl.atlas_path.c_str(),
                                  tiles->tilesize);
    chunks = Resources::Manager::getAtlas(lvl.atlas_path.c_str());
    map = new Tiled::TileMap(lvl.maps_path[act].c_str());

    auto spawnpoint = map->getObject("spawn_sonic");
    if(spawnpoint) {
        cameraCenter = spawnpoint->position;
    }

    Render::setClearColor(glm::vec4(0.392f, 0.584f, 0.929f, 1.0f));
}

void SpriteScene::unload()
{
    delete tiles;
    delete map;
}

void SpriteScene::update()
{
    static glm::vec3 position = glm::vec3(viewportSize / 2.0f, 0.0f);

    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }

    animator->update();
    
    //movie->update();
    
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

     if(Controls::pressing(BTN_DIGITAL_ACTIONDOWN)) {
        animator->setAnimation(ANIM_ROLLING);
    } else {
         if(Controls::pressing(BTN_DIGITAL_UP)) {
             cameraCenter.y -= 8.0f;
             if(!Controls::pressing(BTN_DIGITAL_LEFT)
                && !Controls::pressing(BTN_DIGITAL_RIGHT))
                 animator->setAnimation(ANIM_LOOKUP);
         }
         if(Controls::pressing(BTN_DIGITAL_DOWN)) {
             cameraCenter.y += 8.0f;
             if(!Controls::pressing(BTN_DIGITAL_LEFT)
                && !Controls::pressing(BTN_DIGITAL_RIGHT))
                 animator->setAnimation(ANIM_CROUCHDOWN);
         }
         if(Controls::pressing(BTN_DIGITAL_LEFT)) {
             cameraCenter.x -= 8.0f;
             animator->setAnimation(ANIM_RUNNING);
             direction = -1.0f;
         }
         if(Controls::pressing(BTN_DIGITAL_RIGHT)) {
             cameraCenter.x += 8.0f;
             animator->setAnimation(ANIM_RUNNING);
             direction = 1.0f;
         }

         if(!Controls::pressing(BTN_DIGITAL_UP) &&
            !Controls::pressing(BTN_DIGITAL_DOWN) &&
            !Controls::pressing(BTN_DIGITAL_LEFT) &&
            !Controls::pressing(BTN_DIGITAL_RIGHT))
             animator->setAnimation(ANIM_IDLE);
     }

    
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
    glm::mat4 mvp_movie, movie_model, level_model, font_mvp;
    
    // Sprite MVP
    glm::mat4 mvp = projection * view * model;

    // Movie MVP
    /*
    movie_model = glm::mat4(1.0f);
    movie_model = glm::translate(movie_model, glm::vec3(160.0f, 100.0f, 0.0f));
    movie_model = glm::scale(movie_model, glm::vec3(60.0f, 40.0f, 1.0f));
    */
    
    mvp_movie = projection * view * movie_model;

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
            << "Direction: " << (direction < 0.0f ? "Left" : "Right") << std::endl
            << "Animation: " << animator->getAnimation() << std::endl
            << "Gamepad:   " << (Controls::isGamepad() ? "Yes" : "No");
        font->draw(font_mvp, oss.str().c_str());
    }
    
    animator->draw(mvp);

    // Draw the level
    glm::ivec2 windowSize;
    glm::vec2 tileSize(tiles->tilewidth, tiles->tileheight);
    for(auto it = map->layers.rbegin(); it != map->layers.rend(); it++) {
        auto& layer = *it;
        std::vector<int> window = layer.getTileWindow(
            cameraCenter,
            viewportSize,
            tileSize,
            windowSize);

        // std::cout << "Window: " << std::endl;
        // int xx = 0;
        // for(int i = 0; i < window.size(); i++) {
        //     if(window[i] == 0)
        //         std::cout << "___";
        //     else
        //         std::cout << std::setw(3) << window[i] - 1;
        //     std::cout << ' ';
        //     if(xx++ >= windowSize.x - 1) {
        //         xx = 0;
        //         std::cout << std::endl;
        //     }
        // }
        // std::cout << std::endl;
        
        int x = 0;
        int y = 0;
        for(unsigned i = 0; i < window.size(); i++) {
            if(window[i] != 0) {
                glm::vec2 cameraDiff;
                cameraDiff = glm::trunc(cameraCenter / tileSize) * tileSize;
                cameraDiff = glm::mod(cameraCenter, tileSize);
                level_model = glm::translate(
                    glm::mat4(1.0),
                    glm::vec3(
                        (tileSize.x / 2) + ((x - 1) * tileSize.x) - (cameraDiff.x),
                        (tileSize.y / 2) + (y * tileSize.y) - (cameraDiff.y),
                        0.0f)
                    );
                level_model = glm::scale(
                    level_model,
                    glm::vec3(tileSize.x / 2.0f, tileSize.y / 2.0f, 1.0f));
                chunks->setFrame(window[i] - 1);
                glm::mat4 levelmvp = projection * view * level_model;
                chunks->draw(levelmvp);
            }
            if(x++ >= windowSize.x - 1) {
                x = 0;
                y++;
            }
        }
    }

    //movie->draw(mvp_movie);
}
