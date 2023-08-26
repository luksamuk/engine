#include "sprite_scene.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "resources.hpp"

#include <glm/ext.hpp>


SpriteScene::SpriteScene() {}

SpriteScene::~SpriteScene() {}

void SpriteScene::load()
{
    animator = resourcesLoadAnimator("resources/animation/sonic.toml");
    animator->setAnimation(0);
}

void SpriteScene::unload()
{
    delete animator;
}

void SpriteScene::update()
{
    static glm::vec3 position(15.0f, 15.0f, 0.0f);

    if(controlsPressed(BTN_DIGITAL_OPTION)) {
        animator->setAnimation((animator->getAnimation() + 1) % animator->numAnimations());
    }

    animator->update();
    
    position.x = 0.9 * glm::cos(1 * (float)glfwGetTime());
    position.y = 0.9 * glm::sin(2 * (float)glfwGetTime());

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
    projection = glm::ortho(0.0f, 640.0f, 360.0f, 0.0f, 1.0f, -1.0f);
}

void SpriteScene::draw()
{
    // MVP
    glm::mat4 mvp = projection * view * model;
    animator->draw(mvp);
}
