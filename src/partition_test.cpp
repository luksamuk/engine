#include "partition_test.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "level_select.hpp"
#include "core.hpp"

#include <glm/ext.hpp>
#include <iostream>
#include <sstream>

#include "imgui.h"

//const glm::vec2 viewportSize(320.0f, 224.0f);
//const glm::vec2 viewportSize(480.0f, 336.0f);
const glm::vec2 viewportSize(640.0f, 448.0f);
//const glm::vec2 viewportSize(800.0f, 560.0f);

static float step = 0.0f; 
//static ObjArray hovered;
static std::vector<std::pair<std::pair<ObjPtr, ObjPtr>, glm::vec2>> collisions;
const int numobjs = 26;
static bool paused;
static int  pausesteps;

PartitionTest::PartitionTest() {
    step = 0.0f;
    paused = false;
    pausesteps = 0;
    // hovered.clear();
    collisions.clear();
    // mouseobj = nullptr;
}

PartitionTest::~PartitionTest() {}

void PartitionTest::load() {
    Resources::Manager::loadFont("resources/sprites/fonts/debugger.png",
                                 glm::vec2(10.0f, 10.0f));
    font = Resources::Manager::getFont("resources/sprites/fonts/debugger.png");
    
    grid = std::make_unique<Grid>(viewportSize, glm::vec2(64.0f, 64.0f));
    for(int i = 0; i < numobjs; i++) {
        auto obj = std::make_shared<TestObject>(i);
        obj->setRadius(12.0f + (i * (20.0f / numobjs)));
        obj->setCenter(glm::vec2(32.0f, 32.0f) + (64.0f * i));
        objs.push_back(obj);
        grid->insert(obj);
        // hovered.push_back(obj);
    }
    // mouseobj = std::make_shared<MouseHoverObject>();
    // grid->insert(mouseobj);
    
    Render::setClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    vp = glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, 1.0f, -1.0f);
}

void PartitionTest::unload() {
    // hovered.clear();
    collisions.clear();
}

void PartitionTest::update(double dt) {
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

    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }

    if(Controls::pressed(BTN_DIGITAL_START)) {
        paused = !paused;
    }

    if(paused && Controls::pressed(BTN_DIGITAL_SHOULDERR)) {
        pausesteps++;
    }

    // mouseobj->update(dt);
    // grid->move(mouseobj);
    
    if(!paused || (pausesteps > 0)) {
        for(int i = 0; i < (int)objs.size(); i++) {
            objs[i]->update(dt);
            grid->move(objs[i]);
        }
    
        step += 0.25f * dt;
        //step = glm::mod(step, glm::radians(360.0f));
    }

    collisions.clear();
    
    grid->testAll(
        [&](ObjPtr pA, ObjPtr pB) -> std::optional<glm::vec2> {
            float dist = glm::distance(pA->getCenter(), pB->getCenter());
            if(dist < (pA->getRadius() + pB->getRadius())) {

                auto average = (pA->getCenter() + pB->getCenter()) / 2.0f;
                
                auto a = std::dynamic_pointer_cast<TestObject>(pA);
                auto b = std::dynamic_pointer_cast<TestObject>(pB);
                if(a && b) {
                    collisions.push_back(
                        std::make_pair(
                            std::make_pair(pA, pB),
                            average));
                }
                
                return average;
            }
            
            return std::nullopt;
        });

    if(pausesteps > 0) pausesteps--;

}

static bool window_active = true;

void
PartitionTest::draw_debug_window()
{
    static ImGuiTableFlags tableflags =
        ImGuiTableFlags_SizingFixedFit
        | ImGuiTableFlags_RowBg
        | ImGuiTableFlags_BordersOuter
        | ImGuiTableFlags_Borders;
    static ImGuiTabBarFlags tabflags = ImGuiTabBarFlags_None;

    ImGui::SetNextWindowPos(ImVec2(6, 15), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Debug", &window_active);

    
    ImGui::Text("Step: %0.2f", step);
    ImGui::Text("Mouse: {%0.2f, %0.2f}",
                Controls::mousePos().x,
                Controls::mousePos().y);
    if(ImGui::SmallButton("Toggle Pause")) {
        paused = !paused;
    }
    ImGui::SameLine();
    if(ImGui::SmallButton("Step")) {
        if(paused) pausesteps++;
    }


    if(ImGui::BeginTabBar("Tabs", tabflags)) {

        if(ImGui::BeginTabItem("Collisions")) {
            
            if(ImGui::BeginTable("collisions", 3, tableflags)) {
                ImGui::TableSetupColumn("pA", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("pB", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Point", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                for(auto pair : collisions) {
                    auto pA = std::dynamic_pointer_cast<TestObject>(pair.first.first);
                    auto pB = std::dynamic_pointer_cast<TestObject>(pair.first.second);
                    if(pA && pB) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%c", (char)('A' + pA->getIdx()));
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%c", (char)('A' + pB->getIdx()));
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("{%0.2f, %0.2f}", pair.second.x, pair.second.y);
                    }
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Objects")) {
            if(ImGui::BeginTable("objs", 4, tableflags)) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Center", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Radius", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("# Cols.", ImGuiTableColumnFlags_WidthFixed);
                
                ImGui::TableHeadersRow();
                for(auto obj : objs) {
                    auto ptr = std::dynamic_pointer_cast<TestObject>(obj);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%c", (char)('A' + ptr->getIdx()));
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("{%0.2f, %0.2f}", ptr->getCenter().x, ptr->getCenter().y);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%0.2f", ptr->getRadius());
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%d", ptr->getCollisionCount());
                }

                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        // if(ImGui::BeginTabItem("Misc")) {
                        
        //     ImGui::EndTabItem();
        // }
        
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void PartitionTest::draw() {
    for(int i = 0; i < (int)objs.size(); i++) {
        objs[i]->draw(vp);
    }

    draw_debug_window();
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
    collisionCount = 0;
    
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
    float orbitRadius = viewportSize.x / 4.0f;
    switch(this->getIdx()) {
    case 0:
        pos.x += orbitRadius * glm::cos(step * 5.0f);
        break;
    case 1:
        pos.y += orbitRadius * glm::sin(step * 5.0f);
        break;
    case 2: break;
    default:
        float ratio = (this->idx + 1) * (10.0f / numobjs);
        pos.x += orbitRadius * glm::cos(ratio * step);
        pos.y += orbitRadius * glm::sin(ratio * step);
        break;
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

    glm::vec3 txtpos(this->getCenter(), 0.0f);
    txtpos.y -= font->getGlyphsize().y / 2.0f;
    
    glm::mat4 txtmvp = vp *
        glm::translate(glm::mat4(1.0f), txtpos);
    
    std::stringstream oss;
    oss.clear();
    oss << (char)((colliding ? 'A' : 'a') + this->getIdx());
    font->draw(txtmvp, oss.str().c_str());
    oss.str("");

    txtpos = glm::vec3(this->getCenter(), 0.0f);
    txtpos.y += font->getGlyphsize().y / 2.0f;

    oss.clear();
    oss << collisionCount;
    
    txtmvp = vp *
        glm::translate(glm::mat4(1.0f), txtpos);

    font->draw(txtmvp, oss.str().c_str());
    
    atlas->draw(mvp);
}

int TestObject::getIdx() const {
    return this->idx;
}

void TestObject::onCollision(ObjPtr o, glm::vec2) {
    if(std::dynamic_pointer_cast<TestObject>(o)) {
        this->colliding = true;
    }
    
    if(!paused || (pausesteps > 0)) {
        collisionCount++;
    }
}

int TestObject::getCollisionCount() const {
    return this->collisionCount;
}

// =============================================

// MouseHoverObject::MouseHoverObject() {
//     this->setRadius(2.0f);
// }

// MouseHoverObject::~MouseHoverObject() {}
// void MouseHoverObject::init() {}
// void MouseHoverObject::draw(glm::mat4&) {}

// void
// MouseHoverObject::update(double)
// {
//     //hovered.clear();

//     glm::vec2 objpos =
//         (Controls::mousePos() / glm::vec2(Render::windowSize())) *
//         viewportSize;
    
//     this->setCenter(objpos);
// }

// void
// MouseHoverObject::onCollision(ObjPtr o, glm::vec2)
// {
//     auto io = ImGui::GetIO();
//     if(Controls::mousePressed(BTN_MOUSE_LEFT) && !io.WantCaptureMouse) {
//         auto it = std::find(hovered.begin(), hovered.end(), o);
//         if(it == hovered.end())
//             hovered.push_back(o);
//         else hovered.erase(it);
//     }
// }

