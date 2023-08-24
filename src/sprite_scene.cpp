#include "sprite_scene.hpp"
#include "render.hpp"
#include "controls.hpp"

#include <glm/ext.hpp>

/* Sprite atlas geometry */
static const float quadvertices[] = {
    /* position (3f)     color (3f) */
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
};

static const unsigned int quadelements[] = {
    0, 1, 2,
    0, 2, 3,
};

SpriteScene::SpriteScene() {}

SpriteScene::~SpriteScene() {}

void SpriteScene::load()
{
    /* Loading textures */
    tex = Texture::load("resources/sprites/sonic.png");

    // Normalized (texels) framesize = frame / sheetsize
    framesize_shader = glm::vec2(60.0f, 60.0f) / tex.getSize();
    
    // Load vertex data
    vbo = make_vbo(quadvertices, sizeof(quadvertices), GL_STATIC_DRAW);
    ebo = make_ebo(quadelements, sizeof(quadelements), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    vao = make_vao();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    // Load shaders
    prog = ShaderProgram::link({
            "resources/shaders/spriteatlas/spriteatlas.vs.glsl",
            "resources/shaders/spriteatlas/spriteatlas.fs.glsl",
        });

    glBindVertexArray(vao);
    locmvp        = prog.getUniformLocation("mvp");
    locframecoord = prog.getUniformLocation("framecoord");
    locframesize  = prog.getUniformLocation("framesize");
    loctex        = prog.getUniformLocation("tex");
    locvpos       = prog.getAttribLocation("vpos");

    glEnableVertexAttribArray(locvpos);
    glVertexAttribPointer(locvpos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
}

void SpriteScene::unload()
{
    tex.dispose();
    prog.dispose();
    // glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

static int currentFrameIter = 0;
static glm::vec2 currentFrameCoord;
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

    if(controlsPressed(BTN_DIGITAL_OPTION)) {
        animation = (animation + 1) % 5;
        currentFrameIter = 0;
    }

    const float frames_per_line = glm::floor(tex.getSize().x / 60.0f);
    currentFrameCoord.x = currentFrameIter + minFrame;
    currentFrameCoord.y = glm::floor(currentFrameCoord.x / frames_per_line);
    currentFrameCoord.x = glm::mod(currentFrameCoord.x, frames_per_line);
   
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
    // glUseProgram(program);
    prog.use();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    glActiveTexture(GL_TEXTURE0);
    tex.bind();
    
    // MVP
    glm::mat4 mvp = projection * view * model;
    glUniformMatrix4fv(locmvp, 1, GL_FALSE, glm::value_ptr(mvp));
    
    // Frame size
    glUniform2fv(locframesize, 1, glm::value_ptr(framesize_shader));
    
    // Frame coordinates
    //glUniform2f(locframecoord, 0.0f, 0.0f);
    glUniform2fv(locframecoord, 1, glm::value_ptr(currentFrameCoord));
    
    // Texture
    glUniform1i(loctex, 0);

    // Draw elements
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
