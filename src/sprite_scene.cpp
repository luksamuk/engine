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
    texture = load_texture("resources/sprites/sonic.png", sheetsize);

    // Normalized (texels) framesize = frame / sheetsize
    framesize_shader = glm::vec2(60.0f, 60.0f) / sheetsize;
    
    // Load vertex data
    vbo = make_vbo(quadvertices, sizeof(quadvertices), GL_STATIC_DRAW);
    ebo = make_ebo(quadelements, sizeof(quadelements), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    vao = make_vao();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    // Load shaders
    GLuint vs = load_shader("resources/shaders/spriteatlas/spriteatlas.vs.glsl");
    GLuint fs = load_shader("resources/shaders/spriteatlas/spriteatlas.fs.glsl");
    program = link_program(vs, fs);
    glDeleteShader(vs); glDeleteShader(fs);

    glBindVertexArray(vao);
    locmvp        = glGetUniformLocation(program, "mvp");
    locframecoord = glGetUniformLocation(program, "framecoord");
    locframesize  = glGetUniformLocation(program, "framesize");
    loctex        = glGetUniformLocation(program, "tex");
    locvpos       = glGetAttribLocation(program, "vpos");

    glEnableVertexAttribArray(locvpos);
    glVertexAttribPointer(locvpos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    // Unbind buffers
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpriteScene::unload()
{
    glDeleteTextures(1, &texture);
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

static int currentFrameIter = 0;
static glm::vec2 currentFrameCoord;
#include<iostream>
void SpriteScene::update()
{
    static double oldTime = 0.0;
    static glm::vec3 position;

    double currentTime = glfwGetTime();

    // const int animFrames = 6;
    // const int minFrame = 5;
    // const double frameDuration = 0.016 * 8;

    const int animFrames = 4;
    const int minFrame = 24;
    const double frameDuration = 0.016 * 4;

    if(currentTime - oldTime > frameDuration) {
        // frame must be in range [5, 10], so variate over 5 frames
        currentFrameIter = (currentFrameIter + 1) % (animFrames - 1);
        oldTime = currentTime;
    }

    const float frames_per_line = glm::floor(sheetsize.x / 60.0f);
    currentFrameCoord.x = currentFrameIter + minFrame;
    currentFrameCoord.y = glm::floor(currentFrameCoord.x / frames_per_line);
    currentFrameCoord.x = glm::mod(currentFrameCoord.x, frames_per_line);
   
    position.x = 0.9 * glm::cos(1 * (float)glfwGetTime());
    position.y = 0.9 * glm::sin(2 * (float)glfwGetTime());

    // animation: 5-10
    
    // Model matrix should reflect the sprite size
    // through scaling, and then apply a translation to the center
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(glm::sign(position.x) * 0.15f, -0.225f, 1.0f));

    
    view = glm::mat4(1.0f);
    projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f);
}

void SpriteScene::draw()
{
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
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
