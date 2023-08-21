#ifndef TEST_SCENE_INCLUDED_HPP
#define TEST_SCENE_INCLUDED_HPP

#include "scene.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>

class TestScene : public Scene
{
private:
    GLuint vbo, ebo, vao, program;
    GLint alpha, mvp, vpos, vcol;
    glm::mat4 model, view, projection;
public:
    TestScene();
    virtual ~TestScene();
    virtual void load() override;
    virtual void update() override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
