#include "sprite_scene.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "resources.hpp"

#include <glm/ext.hpp>

#include "tiled.hpp"
#include "level_select.hpp"

static std::shared_ptr<Animator> movie;

static SpriteAtlas *chunks;
static TileData    *tiles;
static TileMap     *map;
static glm::vec2 cameraCenter;

const glm::vec2 viewportSize(320.0f, 224.0f);
//const glm::vec2 viewportSize(640.0f, 360.0f);

static unsigned ANIM_LOOKUP,
    ANIM_CROUCHDOWN,
    ANIM_RUNNING,
    ANIM_IDLE,
    ANIM_ROLLING;
    

SpriteScene::SpriteScene(LevelData l, unsigned act)
{
    lvl = l;
    this->act = act;
    animator = nullptr;
}

SpriteScene::~SpriteScene() {}
#include <iostream>
#include <typeinfo>

void
SpriteScene::changeCharacter(unsigned chara)
{
    // if(animator) {
    //     delete animator;
    //     animator = nullptr;
    // }
    
    switch(chara) {
    default:
        animator = resourcesLoadAnimator("resources/animation/sonic.toml");
        break;
    case 1:
        animator = resourcesLoadAnimator("resources/animation/knuckles.toml");
        break;
    case 2:
        animator = resourcesLoadAnimator("resources/animation/sonic1mania.toml");
        break;
    }
    animator->setAnimation(0);

    ANIM_LOOKUP     = animator->getAnimationByName("Looking Up");
    ANIM_CROUCHDOWN = animator->getAnimationByName("Crouching Down");
    ANIM_RUNNING    = animator->getAnimationByName("Running");
    ANIM_IDLE       = animator->getAnimationByName("Idle");
    ANIM_ROLLING    = animator->getAnimationByName("Rolling");
}

void
SpriteScene::load()
{
    changeCharacter(0);

    movie = resourcesLoadAnimator("resources/animation/movie.toml");
    movie->setAnimation(0);

    font = new SpriteFont("resources/sprites/fonts/levelselect.png",
                          glm::vec2(10.0f, 10.0f));

    cameraCenter = viewportSize / 2.0f;

    tiles = new TileData(lvl.tiles_path.c_str());
    chunks = new SpriteAtlas(lvl.atlas_path.c_str(),
                             tiles->tilesize);
    map = new TileMap(lvl.maps_path[act].c_str());

    auto spawnpoint = map->getObject("spawn_sonic");
    if(spawnpoint) {
        cameraCenter = spawnpoint->position;
    }

    std::cout << "TILE DATA" << std::endl
              << "version: " << tiles->version << std::endl
              << "tiledversion: " << tiles->tiledversion << std::endl
              << "tilewidth: " << tiles->tilewidth << std::endl
              << "tileheight: " << tiles->tileheight << std::endl
              << "tilecount: " << tiles->tilecount << std::endl
              << "columns: " << tiles->columns << std::endl
              << std::endl;
    // for(unsigned i = 0; i < tiles->collisionarrays.size(); i++) {
    //     std::cout << "Tile #" << i;
    //     if(tiles->collisionarrays[i] == std::nullopt) {
    //         std::cout << " no collision" << std::endl;
    //     } else {
    //         std::cout << " collision:" << std::endl;
    //         for(auto shapeptr : *tiles->collisionarrays[i]) {
    //             std::cout << "  " << typeid(*shapeptr).name() << std::endl;
    //         }
    //     }
    // }

    std::cout << "MAP DATA" << std::endl
              << "version: " << map->version << std::endl
              << "tiledversion: " << map->tiledversion << std::endl
              << "orientation: " << map->orientation << std::endl
              << "renderorder: " << map->renderorder << std::endl
              << "width: " << map->width << std::endl
              << "height: " << map->height << std::endl
              << "tilewidth: " << map->tilewidth << std::endl
              << "infinite: " << map->infinite << std::endl
              << "nextlayerid: " << map->nextlayerid << std::endl
              << "nextobjectid: " << map->nextobjectid << std::endl
              << "# layers: " << map->layers.size() << std::endl;
    // for(auto& layer : map->layers) {
    //     std::cout << "  Layer #" << layer.id
    //               << ' ' << '(' << layer.name << ')' << std::endl
    //               << "  Width: " << layer.width << " Height: " << layer.height
    //               << " #tiles: " << layer.data.size()
    //               << std::endl;
    //     int x = 0;
    //     for(auto tile : layer.data) {
    //         if(tile == 0)
    //             std::cout << ' ';
    //         else std::cout << (tile - 1);

    //         if(x++ >= layer.width) {
    //             std::cout << std::endl;
    //             x = 0;
    //         }
    //     }
    //     std::cout << std::endl;
    // }
}

void SpriteScene::unload()
{
    delete font;
    //delete animator;
    //delete movie;
    delete chunks;
    delete tiles;
    delete map;
}

static float direction = 1.0f;

void SpriteScene::update()
{
    static glm::vec3 position = glm::vec3(viewportSize / 2.0f, 0.0f);

    if(controlsPressed(BTN_DIGITAL_OPTION)) {
        SceneManager::add(new LevelSelect());
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

     if(controlsPressing(BTN_DIGITAL_ACTIONDOWN)) {
        animator->setAnimation(ANIM_ROLLING);
    } else {
         if(controlsPressing(BTN_DIGITAL_UP)) {
             cameraCenter.y -= 8.0f;
             animator->setAnimation(ANIM_LOOKUP);
         }
         if(controlsPressing(BTN_DIGITAL_DOWN)) {
             cameraCenter.y += 8.0f;
             animator->setAnimation(ANIM_CROUCHDOWN);
         }
         if(controlsPressing(BTN_DIGITAL_LEFT)) {
             cameraCenter.x -= 8.0f;
             animator->setAnimation(ANIM_RUNNING);
             direction = -1.0f;
         }
         if(controlsPressing(BTN_DIGITAL_RIGHT)) {
             cameraCenter.x += 8.0f;
             animator->setAnimation(ANIM_RUNNING);
             direction = 1.0f;
         }

         if(!controlsPressing(BTN_DIGITAL_UP) &&
            !controlsPressing(BTN_DIGITAL_DOWN) &&
            !controlsPressing(BTN_DIGITAL_LEFT) &&
            !controlsPressing(BTN_DIGITAL_RIGHT))
             animator->setAnimation(ANIM_IDLE);
     }

    
    if(controlsPressed(BTN_DIGITAL_ACTIONUP)) {
        changeCharacter(1);
    } else if(controlsPressed(BTN_DIGITAL_ACTIONLEFT)) {
        changeCharacter(0);
    } else if(controlsPressed(BTN_DIGITAL_ACTIONRIGHT)) {
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
    font->draw(font_mvp, "Hello world!\n\rThis is a test.");
    
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
