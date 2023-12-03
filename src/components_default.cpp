#include "components_default.hpp"

#include <flecs.h>
#include <glm/ext.hpp>

#include "controls.hpp"

namespace Components
{
    // Prototypes
    void RegisterDefaultSystems(flecs::world &ecs);


    /* impl */
    
    void
    RegisterDefaultComponents(flecs::world &ecs)
    {
        ecs.component<Transform>()
            .member<glm::vec2>("position")
            .member<float>("angle");

        ecs.component<Speed>()
            .member<glm::vec2>("speed");

        ecs.component<GroundSpeed>()
            .member<float>("gsp");

        ecs.component<Accelerator>()
            .member<glm::vec2>("accel");

        ecs.component<ViewportInfo>()
            .member<glm::vec2>("size");

        ecs.component<Sensors>()
            .member<bool>("ground")
            .member<bool>("ceiling")
            .member<bool>("left")
            .member<bool>("right");

        ecs.component<Player::Constants>()
            .member<float>("gravity")
            .member<float>("acceleration")
            .member<float>("air_acceleration")
            .member<float>("friction")
            .member<float>("deceleration")
            .member<float>("max_x_speed")
            .member<float>("top_x_speed")
            .member<float>("jump_strength")
            .member<float>("min_jump_strength")
            .member<float>("airdrag_factor")
            .member<float>("airdrag_min_x_speed")
            .member<float>("airdrag_min_y_speed")
            .member<float>("slope_factor")
            .member<float>("rolling_friction")
            .member<float>("rolling_deceleration")
            .member<float>("rolling_top_x_speed")
            .member<float>("rolling_min_x_speed")
            .member<float>("unroll_min_x_speed");

        ecs.component<DebugCircleRender>()
            .member<float>("radius");

        ecs.component<PlayerControl>();

        // Default behaviour for components
        RegisterDefaultSystems(ecs);
    }

    void
    RegisterDefaultSystems(flecs::world &ecs)
    {
        // Render debug circle
        ecs.system<const DebugCircleRender, const Transform, const ViewportInfo>("DebugCircle")
            .each([](const DebugCircleRender &r, const Transform &t, const ViewportInfo &v) {
                glm::mat4 mvp = glm::ortho(0.0f, v.size.x, v.size.y, 0.0f, 1.0f, -1.0f);
                mvp = glm::translate(mvp, glm::vec3(t.position, 0.0f));
                mvp = glm::scale(mvp, glm::vec3(r.radius, r.radius, 1.0f));
                r.atlas->draw(mvp);
            });
        
        // Default system for movement according to speed
        ecs.system<Transform, const Speed>("ApplySpeed")
            .iter([](flecs::iter& it, Transform *t, const Speed *s) {
                for(auto i : it) {
                    t[i].position += s[i].speed;
                }
            });

        // Convert ground speed to speed when grounded
        ecs.system<Speed, const Transform, const GroundSpeed, const Sensors>("ApplyGroundSpeed")
            .iter([](flecs::iter& it,
                     Speed *spd,
                     const Transform *t,
                     const GroundSpeed *gsp,
                     const Sensors *sensors) {
                for(auto i : it) {
                    // Only transform speed when grounded.
                    // Air speed is directly transformed
                    if(sensors[i].ground) {
                        spd[i].speed.x = gsp[i].gsp * glm::cos(t[i].angle);
                        spd[i].speed.y = gsp[i].gsp * -glm::sin(t[i].angle);
                    }
                }
            });

        // Transform speed with respect to acceleration (air accel / Y accel)
        ecs.system<Speed, const Accelerator, const Sensors>("ApplyAirAcceleration")
            .each([](Speed &s, const Accelerator &a, const Sensors &sensors) {
                if(!sensors.ground)
                    s.speed.x += a.accel.x;
                s.speed.y += a.accel.y;
            });

        // Transform speed with respect to acceleration (ground accel)
        ecs.system<GroundSpeed, const Accelerator, const Sensors>("ApplyGroundAcceleration")
            .each([](GroundSpeed &s, const Accelerator &a, const Sensors &sensors) {
                if(sensors.ground)
                    s.gsp += a.accel.x;
            });

        // Transform acceleration with respect to sensors
        ecs.system<Accelerator, const Sensors, const Player::Constants>("AccelerateFromSensors")
            .iter([](flecs::iter& it,
                     Accelerator *a,
                     const Sensors *sensors,
                     const Player::Constants *constants) {
                float delta = it.delta_time() * Player::BaseFrameRate;
                for(auto i : it) {
                    if(!sensors[i].ground) {
                        // Apply gravity
                        a[i].accel.y = constants[i].gravity * delta;
                    } else {
                        a[i].accel.y = 0.0f;
                    }
                }
            });


        // Transform player movement speed with respect to user input
        ecs.system<const PlayerControl,
                   Accelerator,
                   Speed,
                   const Sensors,
                   const Player::Constants>("PlayerAirMovement")
            .iter([](flecs::iter& it,
                     const PlayerControl*,
                     Accelerator *a,
                     Speed *s,
                     const Sensors *sensors,
                     const Player::Constants *constants) {
                for(auto i : it) {
                    // Ignore if not airborne
                    if(sensors[i].ground) continue;

                    float delta = it.delta_time() * Player::BaseFrameRate;

                    // Acceleration
                    float accel = constants[i].acceleration * delta;
                    if(Controls::pressing(BTN_DIGITAL_LEFT))
                        a[i].accel.x = -accel;
                    else if(Controls::pressing(BTN_DIGITAL_RIGHT))
                        a[i].accel.x = accel;

                    // Deceleration
                    float decel = constants[i].deceleration * it.delta_time();
                    if(glm::abs(s[i].speed.x) > 0.0) {
                        if(!Controls::pressing(BTN_DIGITAL_LEFT)
                           && !Controls::pressing(BTN_DIGITAL_RIGHT)) {
                            a[i].accel.x = -glm::sign(s[i].speed.x) * decel;

                            // Halt if abs speed is lower than deceleration.
                            // This is the only place where we change the speed
                            // directly
                            if(glm::abs(s[i].speed.x) < decel) {
                                a[i].accel.x = 0.0f;
                                s[i].speed.x = 0.0f;
                            }
                        }
                    }

                    // Jump limiter
                    float minjump = constants[i].min_jump_strength * delta;
                    if(!Controls::pressing(BTN_DIGITAL_ACTIONDOWN)
                       && (s[i].speed.y < minjump))
                        s[i].speed.y = minjump;
                }
            });

        ecs.system<const PlayerControl,
                   Accelerator,
                   Speed,
                   GroundSpeed,
                   Sensors,
                   const Player::Constants>("PlayerGroundMovement")
            .iter([](flecs::iter& it,
                     const PlayerControl*,
                     Accelerator *a,
                     Speed *airspd,
                     GroundSpeed *s,
                     Sensors *sensors,
                     const Player::Constants *constants) {
                for(auto i : it) {
                    // Ignore if not on ground
                    if(!sensors[i].ground) continue;

                    float delta = it.delta_time() * Player::BaseFrameRate;

                    // X Acceleration
                    float accel = constants[i].acceleration * delta;
                    if(Controls::pressing(BTN_DIGITAL_LEFT))
                        a[i].accel.x = -accel;
                    if(Controls::pressing(BTN_DIGITAL_RIGHT))
                        a[i].accel.x = accel;

                    // X Deceleration
                    float decel = constants[i].deceleration * it.delta_time();
                    if(glm::abs(s[i].gsp) > 0.0) {
                        if(!Controls::pressing(BTN_DIGITAL_LEFT)
                           && !Controls::pressing(BTN_DIGITAL_RIGHT)) {
                            a[i].accel.x = -glm::sign(s[i].gsp) * decel;

                            // Halt if abs speed is lower than deceleration.
                            // This is the only place where we change the speed
                            // directly
                            if(glm::abs(s[i].gsp) < decel) {
                                a[i].accel.x = 0.0f;
                                s[i].gsp = 0.0f;
                            }
                        }
                    }

                    // Jump
                    if(Controls::pressed(BTN_DIGITAL_ACTIONDOWN)) {
                        sensors[i].ground = false;
                        airspd[i].speed.y = constants[i].jump_strength * delta;
                    }
                }
            });
    }

    DebugCircleRender
    MakeCircleRenderer(float radius) {
        Resources::Manager::loadAtlas("resources/sprites/circle.png", glm::vec2(64.0f, 64.0f));
        auto atlas = Resources::Manager::getAtlas("resources/sprites/circle.png");
        atlas->setFrame(0);
        return { atlas, radius };
    }
}
