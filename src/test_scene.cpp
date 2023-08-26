#include "test_scene.hpp"

#include "render.hpp"
#include "controls.hpp"

#include <glm/ext.hpp>

/* Tetrahedron geometry */
static const float vertices[] = {
    /* position (3f)     color (3f) */
    0.0f,  0.6f,  0.0f,  1.0f, 0.0f, 0.0f, // top
    0.6f,  -0.4f, 0.4f,  0.0f, 1.0f, 0.0f, // bottom right, close
    -0.6f, -0.4f, 0.4f,  0.0f, 0.0f, 1.0f, // bottom left, close
    0.0f,  -0.4f, -0.6f, 0.0f, 1.0f, 1.0f, // far, behind
};

static const unsigned int elements[] = {
    0, 1, 2, // front face
    0, 2, 3, // left face
    0, 1, 3, // right face
    3, 1, 2, // bottom face
};

TestScene::TestScene()
{
    vbo = vao = ebo = program = 0;
    alpha = mvp = vpos = vcol = -1;
}

TestScene::~TestScene() {}

void TestScene::load()
{   
    // Load vertex data
    vbo = make_vbo(vertices, sizeof(vertices), GL_STATIC_DRAW);
    ebo = make_ebo(elements, sizeof(elements), GL_STATIC_DRAW);

    // bind vertex data and create vertex arrays
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    vao = make_vao();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Load shaders
    GLuint vs = load_shader("resources/shaders/default/default.vs.glsl");
    GLuint fs = load_shader("resources/shaders/default/default.fs.glsl");
    program = link_program(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Get shaders uniforms / attributes
    glBindVertexArray(vao);

    alpha = glGetUniformLocation(program, "alpha");
    mvp   = glGetUniformLocation(program, "mvp");
    vpos  = glGetAttribLocation(program, "vpos");
    vcol  = glGetAttribLocation(program, "vcol");

    glEnableVertexAttribArray(vpos);
    glVertexAttribPointer(
        vpos, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*) 0);

    glEnableVertexAttribArray(vcol);
    glVertexAttribPointer(
        vcol, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*) (3 * sizeof(float)));

    // Unbind buffers
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Prepare matrices for model-view-projection matrix calculation
    model      = glm::mat4(1.0f);
    model      = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));
    view       = glm::mat4(1.0f);
    projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 2.0f, -2.0f);
}

void TestScene::unload()
{
    // glUseProgram(0);
    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}


void TestScene::update()
{
    // Rotation parameters
    static float rotationX = glm::radians(1.0f);
    static float rotationY = 0.0f;
    static float rotationZ = glm::radians(1.0f);

    if(controlsPressing(BTN_DIGITAL_UP)) {
        rotationX = glm::radians(1.0f);
    }
    if(controlsPressing(BTN_DIGITAL_DOWN)) {
        rotationX = glm::radians(-1.0f);
    }

    if(controlsPressing(BTN_DIGITAL_RIGHT)) {
        rotationY = glm::radians(1.0f);
    }
    if(controlsPressing(BTN_DIGITAL_LEFT)) {
        rotationY = glm::radians(-1.0f);
    }

    if(controlsPressing(BTN_DIGITAL_ACTIONUP)) {
        rotationZ = glm::radians(1.0f);
    }
    if(controlsPressing(BTN_DIGITAL_ACTIONDOWN)) {
        rotationZ = glm::radians(-1.0f);
    }

    if(controlsPressed(BTN_DIGITAL_START)) {
        rotationX = rotationY = rotationZ = 0.0f;
    }

    view = glm::rotate(view, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::rotate(view, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
}

void TestScene::draw()
{
    glm::mat4 mvpmat = projection * view * model;

    // Order matters here!
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(mvpmat));
    glUniform1f(alpha, (0.4f * glm::cos(5.0f * (float)glfwGetTime())));
        
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    // glUseProgram(0);
    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

