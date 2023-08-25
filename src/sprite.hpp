#ifndef SPRITE_HPP_INCLUDED
#define SPRITE_HPP_INCLUDED

#include "render.hpp"

#include <glm/glm.hpp>


// SpriteAtlas
class SpriteAtlas
{
private:
    Texture texture;
    unsigned int frame = 0;
    glm::vec2 framesize, framesize_shader, framecoord;
    float framesperline;

    // TODO: The following should be shared resources
    ShaderProgram program;
    GLuint vbo, vao, ebo;
    GLuint mvp_loc, framecoord_loc, framesize_loc, tex_loc, vpos_loc;
public:
    SpriteAtlas(const char *texturePath, glm::vec2 framesize);
    ~SpriteAtlas();
    void  setFrame(unsigned int);
    void  draw(glm::mat4& mvp);
};

// Animator

#endif
