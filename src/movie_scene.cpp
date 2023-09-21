#include "movie_scene.hpp"
#include "sprite.hpp"
#include "controls.hpp"
#include "level_select.hpp"
#include <glm/ext.hpp>

MovieScene::MovieScene() {}
MovieScene::~MovieScene() {}

void MovieScene::load()
{
    Resources::Manager::loadAnimator("resources/animation/movie.toml");
    movie = Resources::Manager::getAnimator("resources/animation/movie.toml");
    movie->setAnimation(0);

    // projection * view * model (view is identity and not represented here)
    mvp = glm::ortho(0.0f, 320.0f, 224.0f, 0.0f, 1.0f, -1.0f) *
        glm::translate(glm::mat4(1.0f), glm::vec3(160.0f, 112.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(90.0f, 60.0f, 1.0f));

    Resources::Manager::loadFont("resources/sprites/fonts/levelselect.png",
                                 glm::vec2(10.0f, 10.0f));
    font = Resources::Manager::getFont("resources/sprites/fonts/levelselect.png");
    text_mvp =
        glm::ortho(0.0f, 480.0f, 336.0f, 0.0f, 1.0f, -1.0f) *
        glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 0.0f));

    Render::setClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    
}

void MovieScene::update(double)
{
    movie->update();

    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }
}

void MovieScene::draw()
{
    font->draw(text_mvp, "Press Back for Level Select");
    movie->draw(mvp);
}

void MovieScene::unload() {}
