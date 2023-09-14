#include "partition_test.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "level_select.hpp"
#include "core.hpp"

#include <glm/ext.hpp>
#include <iostream>
#include <sstream>

const glm::vec2 viewportSize(320.0f, 224.0f);
static float step = 0.0f;

PartitionTest::PartitionTest() {}

PartitionTest::~PartitionTest() {}

void PartitionTest::load() {
    Resources::Manager::loadFont("resources/sprites/fonts/debugger.png",
                                 glm::vec2(10.0f, 10.0f));
    font = Resources::Manager::getFont("resources/sprites/fonts/debugger.png");
    
    grid = std::make_unique<Grid>(viewportSize, glm::vec2(64.0f, 64.0f));
    for(int i = 0; i < 5; i++) {
        obj[i] = std::make_shared<TestObject>(i);
        obj[i]->setRadius(32.0f);
        obj[i]->setCenter(glm::vec2(32.0f, 32.0f) + (64.0f * i));
        grid->insert(obj[i]);
    }
    Render::setClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    vp = glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, 1.0f, -1.0f);
}

void PartitionTest::unload() {}

void PartitionTest::update(double dt) {
    for(int i = 0; i < 5; i++) {
        obj[i]->update(dt);
        grid->move(obj[i]);
    }

    std::ostringstream oss;
    oss.clear();
    
    step += 0.25f * dt;
    step = glm::mod(step, glm::radians(360.0f));

    grid->testAll(
        [&](ObjPtr pA, ObjPtr pB) -> std::optional<glm::vec2> {
            auto a = std::dynamic_pointer_cast<TestObject>(pA);
            auto b = std::dynamic_pointer_cast<TestObject>(pB);

            float dist = glm::distance(a->getCenter(), b->getCenter());
            if(dist < (a->getRadius() + b->getRadius())) {
                oss << (char)('a' + a->getIdx())
                    << " X "
                    << (char)('a' + b->getIdx())
                    << std::endl;
                
                return glm::vec2(0.0f, 0.0f);
            }
            
            return std::nullopt;
        });

    collidingmsg = oss.str();
    
    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }
}

void PartitionTest::draw() {
    glm::mat4 txtmvp = vp *
        glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 0.0f));

    font->draw(txtmvp, collidingmsg.c_str());
    
    for(int i = 0; i < 5; i++) {
        obj[i]->draw(vp);
    }

}


// =============================================

TestObject::TestObject(int i) {
    Resources::Manager::loadAtlas("resources/sprites/circle.png",
                                  glm::vec2(64.0f, 64.0f));
    Resources::Manager::loadFont("resources/sprites/fonts/debugger.png",
                                 glm::vec2(10.0f, 10.0f));
    atlas = Resources::Manager::getAtlas("resources/sprites/circle.png");
    atlas->setFrame(0);
    font = Resources::Manager::getFont("resources/sprites/fonts/debugger.png");
    idx = i;
}

TestObject::~TestObject() {}

void TestObject::init() {}

void TestObject::update(double dt) {
    static double oldReportTime = 0.0;
    double currentReportTime = glfwGetTime();

    if(currentReportTime - oldReportTime > 2.0) {
        if(dt > 0.0f) {
            std::ostringstream oss;
            oss.clear();
            oss << "FPS: " << 1.0 / dt;
            Core::setWindowCaptionAppendix(oss.str());
        }
        oldReportTime = currentReportTime;
    }
    
    auto pos = viewportSize / 2.0f;
    if(this->getIdx() == 0) {
        pos.x += 80.0f * glm::cos(step * 10.0f);
    } else {
        pos.x += 80.0f * glm::cos((this->idx+1) * step);
        pos.y += 80.0f * glm::sin((this->idx+1) * step);
    }

    this->setCenter(pos);
    
    
    model = glm::translate(glm::mat4(1.0f), glm::vec3(this->getCenter(), 0.0f));
    model = glm::scale(
        model,
        glm::vec3(this->getRadius(), this->getRadius(), 1.0f));

    colliding = false;
}

void TestObject::draw(glm::mat4& vp) {
    glm::mat4 mvp = vp * model;
    
    glm::mat4 txtmvp = vp *
        glm::translate(glm::mat4(1.0f), glm::vec3(this->getCenter(), 0.0f));
    
    std::ostringstream oss;
    oss.clear();
    oss << (char)((colliding ? 'A' : 'a') + this->getIdx());
    font->draw(txtmvp, oss.str().c_str());
    
    atlas->draw(mvp);
}

int TestObject::getIdx() const {
    return this->idx;
}

void TestObject::onCollision(ObjPtr, glm::vec2) {
    this->colliding = true;
}

