#include "sprite.hpp"

#include <glm/ext.hpp>

SpriteAtlas::SpriteAtlas(const char *texturePath, glm::vec2 framesize)
{
    texture = Texture::load(texturePath);
    program = ShaderProgram::link({
            "resources/shaders/spriteatlas/spriteatlas.vs.glsl",
            "resources/shaders/spriteatlas/spriteatlas.fs.glsl",
        });
    this->framesize = framesize;
    framesize_shader = framesize / this->texture.getSize();
    framesperline = glm::floor(texture.getSize().x / framesize.x);

    vbo = make_vbo(QuadGeometry::vertices(), QuadGeometry::verticesSize(), GL_STATIC_DRAW);
    ebo = make_ebo(QuadGeometry::elements(), QuadGeometry::elementsSize(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    vao = make_vao();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindVertexArray(vao);
    mvp_loc        = program.getUniformLocation("mvp");
    framecoord_loc = program.getUniformLocation("framecoord");
    framesize_loc  = program.getUniformLocation("framesize");
    tex_loc        = program.getUniformLocation("tex");
    vpos_loc       = program.getAttribLocation("vpos");

    glEnableVertexAttribArray(vpos_loc);
    glVertexAttribPointer(
        vpos_loc,
        QuadGeometry::numVertexComponents(),
        GL_FLOAT,
        GL_FALSE,
        QuadGeometry::singleVertexSize(),
        0);
}

SpriteAtlas::~SpriteAtlas()
{
    texture.dispose();
    program.dispose();
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void
SpriteAtlas::setFrame(unsigned int frame)
{
    this->frame = frame;
}

void
SpriteAtlas::draw(glm::mat4& mvp)
{
    // calculate framecoord
    framecoord.x = (float)frame;
    framecoord.y = glm::floor(framecoord.x / framesperline);
    framecoord.x = glm::mod(framecoord.x, framesperline);
    
    program.use();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glActiveTexture(GL_TEXTURE0);
    texture.bind();
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform2fv(framecoord_loc, 1, glm::value_ptr(framecoord));
    glUniform2fv(framesize_loc, 1, glm::value_ptr(framesize_shader));
    glUniform1i(tex_loc, 0);
    glDrawElements(GL_TRIANGLES, QuadGeometry::numElements(), GL_UNSIGNED_INT, 0);
}
