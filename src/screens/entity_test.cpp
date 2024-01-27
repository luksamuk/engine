#include "entity_test.hpp"
#include "engine/render.hpp"
#include "engine/controls.hpp"
#include "level_select.hpp"
#include "engine/core.hpp"
#include <glm/ext.hpp>
#include <sstream>

const glm::vec2 viewportSize(320.0f, 224.0f);

EntityTest::EntityTest() {
    ecs.component<glm::vec2>()
        .member<float>("x")
        .member<float>("y");
    
    ecs.component<Transform>()
        .member<glm::vec2>("pos");
    
    ecs.component<Speed>()
        .member<glm::vec2>("speed");
    
    ecs.component<AutoControl>()
        .member<float>("step");
}

EntityTest::~EntityTest() {}

void EntityTest::load() {
    Resources::Manager::loadFont("resources/sprites/fonts/levelselect.png",
                                 glm::vec2(10.0f, 10.0f));
    Resources::Manager::loadAtlas("resources/sprites/circle.png",
                                  glm::vec2(64.0f, 64.0f));
    
    font = Resources::Manager::getFont("resources/sprites/fonts/levelselect.png");
    atlas = Resources::Manager::getAtlas("resources/sprites/circle.png");
    atlas->setFrame(0);

    fontmvp = glm::ortho(0.0f, 480.0f, 336.0f, 0.0f, 1.0f, -1.0f);
    fontmvp = glm::translate(fontmvp, glm::vec3(20.0f, 20.0f, 0.0f));

    // Test by curl'ing http://localhost:27750/entity/flecs
    // or access https://flecs.dev/explorer
    ecs.set<flecs::Rest>({});
    ecs.import<flecs::monitor>();

    ecs.system<Transform, const Speed>("Move")
        .iter([](flecs::iter& it, Transform *t, const Speed *s) {
            for(auto i : it) {
                t[i].pos += s[i].speed * it.delta_time();
            }
        });

    ecs.system<AutoControl>("AutoControlStep")
        .iter([](flecs::iter& it, AutoControl *c) {
            for(auto i : it) {
                c[i].step += 60.0f * it.delta_time();
            }
        });

    ecs.system<AutoControl, Transform>("PlayerAutoMove")
        .each([&](AutoControl& c, Transform& t) {
            glm::vec2 center = viewportSize / 2.0f;
            t.pos = center +
                (50.0f *
                 glm::vec2(
                     glm::cos(glm::radians(c.step)),
                     glm::sin(glm::radians(c.step))));
        });

    ecs.system<const PlayerControl, Speed>("PlayerMove")
        .iter([](flecs::iter& it, const PlayerControl*, Speed *s) {
            const float accel = 60.0f;
            const float decel = 70.0f;

            const float dt = it.delta_time();
            
            for(auto i : it) {
                // Acceleration
                if(Controls::pressing(BTN_DIGITAL_LEFT))
                    s[i].speed.x -= accel * dt;
                if(Controls::pressing(BTN_DIGITAL_RIGHT))
                    s[i].speed.x += accel * dt;
                if(Controls::pressing(BTN_DIGITAL_UP))
                    s[i].speed.y -= accel * dt;
                if(Controls::pressing(BTN_DIGITAL_DOWN))
                    s[i].speed.y += accel * dt;

                // Deceleration
                if(glm::abs(s[i].speed.x) >= decel * dt) {
                    if(!Controls::pressing(BTN_DIGITAL_LEFT)
                       && !Controls::pressing(BTN_DIGITAL_RIGHT)) {
                        s[i].speed.x -= glm::sign(s[i].speed.x) * decel * dt;

                        if(glm::abs(s[i].speed.x) < decel * dt) {
                            s[i].speed.x = 0.0f;
                        }
                    }
                }
                
                if(glm::abs(s[i].speed.y) >= decel * dt) {
                    if(!Controls::pressing(BTN_DIGITAL_UP)
                       && !Controls::pressing(BTN_DIGITAL_DOWN)) {
                        s[i].speed.y -= glm::sign(s[i].speed.y) * decel * dt;

                        if(glm::abs(s[i].speed.y) < decel * dt) {
                            s[i].speed.y = 0.0f;
                        }
                    }
                }
            }
        });
    
    ecs.system<const SphereRender, const Transform>("DrawACircle")
        .each([&](const SphereRender&, const Transform& t) {
            glm::mat4 mvp = glm::ortho(0.0f, viewportSize.x, viewportSize.y, 0.0f, 1.0f, -1.0f);
            mvp = glm::translate(mvp, glm::vec3(t.pos, 0.0f));
            mvp = glm::scale(mvp, glm::vec3(32.0f, 32.0f, 1.0f));
            atlas->draw(mvp);
        });

    flecs::entity Blah = ecs.entity("Blah")
        .set(Transform{viewportSize / 2.0f})
        .set(Speed{glm::vec2(0.0f, 0.0f)})
        .add<PlayerControl>()
        .add<SphereRender>();

    flecs::entity Bleh = ecs.entity("Bleh")
        .set(Transform{viewportSize / 2.0f})
        .set(AutoControl{0.0f})
        .add<SphereRender>();
}

void EntityTest::unload() {}

void EntityTest::update(double dt)
{
    ecs.progress(dt);
    
    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }

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
}

void EntityTest::draw() {
    font->draw(
        fontmvp,
        "Server: http://localhost:27750/entity/flecs\n"
        "Panel:  https://flecs.dev/explorer");
}
