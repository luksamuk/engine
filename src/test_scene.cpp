#include "test_scene.hpp"

#include "render.hpp"
#include "controls.hpp"
#include "level_select.hpp"

#include <glm/ext.hpp>

/* == READ THIS! ==
 * I know that this code is much too "low-level" (disregarding the text drawing
 * elements), but this scene is supposed to be like this. It is somewhat a
 * reference for rendering a very simple tetrahedron on screen by leveraging
 * what OpenGL ES 3.2 has to offer, and as a reference for any future project
 * or routine on this engine that may require a "low-level" understanding of
 * OpenGL ES 3.2.
 *
 * Shaders are located at resources/shaders/default/default.{fs,vs}.glsl. They
 * serve the single purpose of being shaders for this render test, and may even
 * be discarded in a final build, even though attempting to load this scene may
 * cause problems if these are absent.
 */

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
    fov_angle = 45.0f;
    using_ortho = false;
}

TestScene::~TestScene() {}

void TestScene::load()
{   
    // Load vertex data
    vbo = Render::make_vbo(vertices, sizeof(vertices), GL_STATIC_DRAW);
    ebo = Render::make_ebo(elements, sizeof(elements), GL_STATIC_DRAW);

    // bind vertex data and create vertex arrays
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    vao = Render::make_vao();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Load shaders
    GLuint vs = Render::raw_load_shader("resources/shaders/default/default.vs.glsl");
    GLuint fs = Render::raw_load_shader("resources/shaders/default/default.fs.glsl");
    program = Render::raw_link_program(vs, fs);
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
    view       = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
    projection = glm::perspective(glm::radians(fov_angle), 1.0f, 2.0f, -2.0f);
    
    // Text.
    // Assumes a 480x336 viewport, renders at 10x10
    Resources::Manager::loadFont("resources/sprites/fonts/levelselect.png",
                                 glm::vec2(10.0f, 10.0f));
    font = Resources::Manager::getFont("resources/sprites/fonts/levelselect.png");
    text_mvp =
        glm::ortho(0.0f, 480.0f, 336.0f, 0.0f, 1.0f, -1.0f) *
        glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 0.0f));

    // Background color
    Render::setClearColor(glm::vec4(0.2f));
}

void TestScene::unload()
{
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}


void TestScene::update()
{
    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }
    
    // Rotation parameters
    //static float rotationX = glm::radians(1.0f);
    //static float rotationY = 0.0f;
    //static float rotationZ = glm::radians(1.0f);
    static glm::vec3 rotation(0.0f, glm::radians(1.0f), 0.0f);

    if(Controls::pressing(BTN_DIGITAL_UP)) {
        //rotationX = glm::radians(1.0f);
        rotation = glm::vec3(glm::radians(1.0f), 0.0f, 0.0f);
    }
    if(Controls::pressing(BTN_DIGITAL_DOWN)) {
        //rotationX = glm::radians(-1.0f);
        rotation = glm::vec3(glm::radians(-1.0f), 0.0f, 0.0f);
    }

    if(Controls::pressing(BTN_DIGITAL_RIGHT)) {
        //rotationY = glm::radians(1.0f);
        rotation = glm::vec3(0.0f, glm::radians(1.0f), 0.0f);
    }
    if(Controls::pressing(BTN_DIGITAL_LEFT)) {
        //rotationY = glm::radians(-1.0f);
        rotation = glm::vec3(0.0f, glm::radians(-1.0f), 0.0f);
    }

    if(Controls::pressing(BTN_DIGITAL_ACTIONUP)) {
        //rotationZ = glm::radians(1.0f);
        rotation = glm::vec3(0.0f, 0.0f, glm::radians(1.0f));
    }
    if(Controls::pressing(BTN_DIGITAL_ACTIONDOWN)) {
        //rotationZ = glm::radians(-1.0f);
        rotation = glm::vec3(0.0f, 0.0f, glm::radians(-1.0f));
    }

    if(Controls::pressed(BTN_DIGITAL_START)) {
        //rotationX = rotationY = rotationZ = 0.0f;
        rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    if(Controls::pressed(BTN_DIGITAL_ACTIONRIGHT) && !using_ortho) {
        using_ortho = true;
        view       = glm::mat4(1.0f);
        projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
    }

    if(Controls::pressed(BTN_DIGITAL_ACTIONLEFT) && using_ortho) {
        using_ortho = false;
        view       = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
        projection = glm::perspective(glm::radians(fov_angle), 1.0f, 2.0f, -2.0f);
    }

    if(!using_ortho) {
        const float fov_change_rate = 1.0f;
        if(Controls::pressing(BTN_DIGITAL_SHOULDERL)) {
            fov_angle = std::max(30.0f, fov_angle - fov_change_rate);
            projection = glm::perspective(glm::radians(fov_angle), 1.0f, 2.0f, -2.0f);
        }
        else if(Controls::pressing(BTN_DIGITAL_SHOULDERR)) {
            fov_angle = std::min(90.0f, fov_angle + fov_change_rate);
            projection = glm::perspective(glm::radians(fov_angle), 1.0f, 2.0f, -2.0f);
        }
    }

    //view = glm::rotate(view, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
    //view = glm::rotate(view, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    //view = glm::rotate(view, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::rotate(view, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
}

void TestScene::draw()
{
    font->draw(
        text_mvp,
        "Up   / Down  : Rotate X\n"
        "Left / Right : Rotate Y\n"
        "   Y / A     : Rotate Z\n"
        "   X / B     : Perspective / Orthographic\n"
        "  LB / RB    : Change FOV (Perspective)\n"
        "   Start     : Stop rotation\n"
        "   Back      : Level Select");
    
    glm::mat4 mvpmat = projection * view * model;

    // Order matters here!
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(mvpmat));
    glUniform1f(alpha, (0.4f * glm::cos(5.0f * (float)glfwGetTime())));
        
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
}

