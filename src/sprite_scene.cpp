#include "sprite_scene.hpp"
#include "render.hpp"
#include "controls.hpp"

#include <glm/ext.hpp>


SpriteScene::SpriteScene() {}

SpriteScene::~SpriteScene() {}

void SpriteScene::load()
{
    atlas = new SpriteAtlas("resources/sprites/sonic.png", glm::vec2(60.0f, 60.0f));
}

void SpriteScene::unload()
{
    delete atlas;
}

static int currentFrameIter = 0;
static int animation = 0;

void SpriteScene::update()
{
    static double oldTime = 0.0;
    static glm::vec3 position(15.0f, 15.0f, 0.0f);

    double currentTime = glfwGetTime();

    int animFrames;
    int minFrame;
    double frameDuration = 1.0;

    switch(animation) {
    default:
        animFrames = 1;
        minFrame = 0;
        break;
    case 1:
        animFrames = 6;
        minFrame = 5;
        frameDuration = 0.016 * 8;
        break;
    case 2:
        animFrames = 4;
        minFrame = 11;
        frameDuration = 0.016 * 4;
        break;
    case 3:
        animFrames = 8;
        minFrame = 15;
        frameDuration = 0.016 * 4;
        break;
    case 4:
        animFrames = 4;
        minFrame = 24;
        frameDuration = 0.016 * 4;
        break;
    }

    if(animFrames > 1) {
        if(currentTime - oldTime > frameDuration) {
            // frame must be in range [5, 10], so variate over 5 frames
            currentFrameIter = (currentFrameIter + 1) % (animFrames - 1);
            oldTime = currentTime;
        }
    }

    atlas->setFrame(minFrame + currentFrameIter);

    if(controlsPressed(BTN_DIGITAL_OPTION)) {
        animation = (animation + 1) % 5;
        currentFrameIter = 0;
    }

    position.x = 0.9 * glm::cos(1 * (float)glfwGetTime());
    position.y = 0.9 * glm::sin(2 * (float)glfwGetTime());

    auto mouse = controlsMousePos();
    position.x = mouse.x;
    position.y = mouse.y;

    // animation: 5-10
    
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
    atlas->draw(mvp);
}
