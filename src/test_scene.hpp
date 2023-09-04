#ifndef TEST_SCENE_INCLUDED_HPP
#define TEST_SCENE_INCLUDED_HPP

#include "scene.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "resources.hpp"

class TestScene : public Scenes::Scene
{
private:
    GLuint vbo, ebo, vao, program;
    GLint alpha, mvp, vpos, vcol;
    glm::mat4 model, view, projection, text_mvp;
    Resources::FontPtr font;
    float fov_angle;
    bool using_ortho;
public:
    TestScene();
    virtual ~TestScene();
    virtual void load() override;
    virtual void update() override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
