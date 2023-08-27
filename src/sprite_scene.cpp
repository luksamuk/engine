#include "sprite_scene.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "resources.hpp"

#include <glm/ext.hpp>

#include "tiled.hpp"

static Animator *movie;
static TileData *tiles;
static TileMap  *map;

SpriteScene::SpriteScene() {}

SpriteScene::~SpriteScene() {}
#include <iostream>
#include <typeinfo>

void SpriteScene::load()
{
    animator = resourcesLoadAnimator("resources/animation/sonic.toml");
    animator->setAnimation(0);

    movie = resourcesLoadAnimator("resources/animation/movie.toml");
    movie->setAnimation(0);

    tiles = new TileData("resources/levels/R0/tiles.tsx");
    map = new TileMap("resources/levels/R0/zone0.tmx");

    std::cout << "version: " << tiles->version << std::endl
              << "tiledversion: " << tiles->tiledversion << std::endl
              << "tilewidth: " << tiles->tilewidth << std::endl
              << "tileheight: " << tiles->tileheight << std::endl
              << "tilecount: " << tiles->tilecount << std::endl
              << "columns: " << tiles->columns << std::endl;
    for(unsigned i = 0; i < tiles->collisionarrays.size(); i++) {
        std::cout << "Tile #" << i;
        if(tiles->collisionarrays[i] == std::nullopt) {
            std::cout << " no collision" << std::endl;
        } else {
            std::cout << " collision:" << std::endl;
            for(auto shapeptr : *tiles->collisionarrays[i]) {
                std::cout << "  " << typeid(*shapeptr).name() << std::endl;
            }
        }
    }

    std::cout << "version: " << map->version << std::endl
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
    for(auto& layer : map->layers) {
        std::cout << "  Layer #" << layer.id
                  << ' ' << '(' << layer.name << ')' << std::endl
                  << "  Width: " << layer.width << " Height: " << layer.height
                  << " #tiles: " << layer.data.size()
                  << std::endl;
        int x = 0;
        for(auto tile : layer.data) {
            if(tile == 0)
                std::cout << ' ';
            else std::cout << (tile - 1);

            if(x++ >= layer.width - 1) {
                std::cout << std::endl;
                x = 0;
            }
        }
        std::cout << std::endl;
    }

    delete tiles;
    delete map;
}

void SpriteScene::unload()
{
    delete animator;
    delete movie;
}

void SpriteScene::update()
{
    static glm::vec3 position(100.0f, 100.0f, 0.0f);

    if(controlsPressed(BTN_DIGITAL_OPTION)) {
        animator->setAnimation((animator->getAnimation() + 1) % animator->numAnimations());
    }

    animator->update();
    movie->update();
    
    //position.x = 0.9 * glm::cos(1 * (float)glfwGetTime());
    //position.y = 0.9 * glm::sin(2 * (float)glfwGetTime());

    auto mouse = controlsMousePos();
    position.x = mouse.x;
    position.y = mouse.y;

    // Model matrix should reflect the sprite size
    // through scaling, and then apply a translation to the center
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(glm::sign(position.x) * 30.0f, 30.0f, 1.0f));

    
    view = glm::mat4(1.0f);
    //projection = glm::ortho(0.0f, 640.0f, 0.0f, 360.0f, 1.0f, -1.0f);
    //projection = glm::ortho(0.0f, 640.0f, 360.0f, 0.0f, 1.0f, -1.0f);
    //projection = glm::ortho(0.0f, 320.0f, 224.0f, 0.0f, 1.0f, -1.0f);
    projection = glm::ortho(0.0f, 320.0f, 180.0f, 0.0f, 1.0f, -1.0f);
}

void SpriteScene::draw()
{
    // MVP
    glm::mat4 mvp = projection * view * model;

    glm::mat4 mvp_movie, movie_model;

    //auto mouse = controlsMousePos();
    movie_model = glm::mat4(1.0f);
    //movie_model = glm::translate(movie_model, glm::vec3(360.0f, 160.0f, 0.0f));
    //movie_model = glm::translate(movie_model, glm::vec3(mouse.x, mouse.y, 0.0f));
    movie_model = glm::translate(movie_model, glm::vec3(160.0f, 90.0f, 0.0f));
    //movie_model = glm::scale(movie_model, glm::vec3(60.0f, 40.0f, 1.0f));
    movie_model = glm::scale(movie_model, glm::vec3(120.0f, 80.0f, 1.0f));

    mvp_movie = projection * view * movie_model;
    
    animator->draw(mvp);
    movie->draw(mvp_movie);
}
