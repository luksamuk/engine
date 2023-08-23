#ifndef SPRITE_SCENE_HPP_INCLUDED
#define SPRITE_SCENE_HPP_INCLUDED

#include "scene.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>

class SpriteScene : public Scene
{
private:

    glm::mat4 model, view, projection;

    GLuint vbo, ebo, vao, program;
    GLuint texture;
    GLint  locmvp, locframecoord, locframesize, loctex, locvpos, locvcolor;
    glm::vec2 framesize_shader, sheetsize;
public:
    SpriteScene();
    virtual ~SpriteScene();
    virtual void load() override;
    virtual void update() override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
