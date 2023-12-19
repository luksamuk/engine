#include "components_default.hpp"

#include <flecs.h>
#include <glm/ext.hpp>

#include "controls.hpp"

namespace Components
{
    // Prototypes
    void RegisterDefaultSystems(flecs::world &ecs);
    void RegisterPlayerMovementSystems(flecs::world &ecs);


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
            .member<float>("airdrag_division_factor")
            .member<float>("airdrag_rem_factor")
            .member<float>("airdrag_min_y_speed")
            .member<float>("slope_factor")
            .member<float>("rolling_friction")
            .member<float>("rolling_deceleration")
            .member<float>("rolling_top_x_speed")
            .member<float>("rolling_min_x_speed")
            .member<float>("unroll_min_x_speed");

        ecs.component<Player::State>()
            .member<unsigned>("action")
            .member<float>("direction");

        ecs.component<PlayerControls>()
            .member<bool>("up")
            .member<bool>("down")
            .member<bool>("left")
            .member<bool>("right")
            .member<bool>("jump")
            .member<bool>("pressUp")
            .member<bool>("pressDown")
            .member<bool>("pressLeft")
            .member<bool>("pressRight")
            .member<bool>("pressJump");

        ecs.component<PlayerUseJoypad>();

        ecs.component<DebugCircleRender>()
            .member<float>("radius")
            .member<bool>("visible");

        ecs.component<FakeGround>()
            .member<float>("ypos");

        // Default behaviour for components
        RegisterDefaultSystems(ecs);
        RegisterPlayerMovementSystems(ecs);
    }

    void
    RegisterDefaultSystems(flecs::world &ecs)
    {
        // Render debug circle
        ecs.system<const DebugCircleRender, const Transform, const ViewportInfo>("DebugCircle")
            .kind(flecs::PostUpdate)
            .each([](const DebugCircleRender &r, const Transform &t, const ViewportInfo &v) {
                if(!r.visible) return;
                
                glm::vec2 position = t.position;
                // if(position.x > v.size.x / 2.0f)
                //     position.x = v.size.x / 2.0;
                    
                glm::mat4 mvp = glm::ortho(0.0f, v.size.x, v.size.y, 0.0f, 1.0f, -1.0f);
                mvp = glm::translate(mvp, glm::vec3(position, 0.0f));
                mvp = glm::scale(mvp, glm::vec3(r.radius, r.radius, 1.0f));
                r.atlas->draw(mvp);
            });
    }

    void
    RegisterPlayerMovementSystems(flecs::world &ecs)
    {
        // Default system for movement according to speed
        ecs.system<Transform, const Speed>("ApplySpeed")
            .iter([](flecs::iter& it, Transform *t, const Speed *s) {
                for(auto i : it) {
                    t[i].position += s[i].speed;
                }
            });

        // Default system for converting ground speed to speed.
        // GSP is converted to SPD if and only if the entity is not on ground.
        ecs.system<Speed,
                   const GroundSpeed,
                   const Transform,
                   const Sensors>("ConvertGroundSpeed")
            .each([](Speed &spd,
                     const GroundSpeed &gsp,
                     const Transform &t,
                     const Sensors &sensors) {
                if(sensors.ground) {
                    spd.speed.x = gsp.gsp * glm::cos(t.angle);
                    spd.speed.y = gsp.gsp * -glm::sin(t.angle);
                }
            });

        // Fake ground
        ecs.system<Transform,
                   Speed,
                   GroundSpeed,
                   Sensors,
                   Player::State,
                   const FakeGround>("LandOnFakeGround")
            .each([](Transform &t,
                     Speed &s,
                     GroundSpeed &gsp,
                     Sensors &sen,
                     Player::State &state,
                     const FakeGround &g) {
                // Land on fake solid ground
                if(t.position.y >= g.ypos && s.speed.y > 0.0f && !sen.ground) {
                    t.position.y = g.ypos;
                    s.speed.y = 0.0f;
                    sen.ground = true;
                    state.action = Player::ActionKind::Idle;

                    // "Partial" implementation of landing on ground.
                    // We always assume a flat ground, after all.
                    // See https://info.sonicretro.org/SPG:Slope_Physics#Landing_On_The_Ground
                    gsp.gsp = s.speed.x;
                }
            });

        // Apply joypad controls
        ecs.system<PlayerControls, const PlayerUseJoypad>("PlayerUpdateJoypad")
            .each([](PlayerControls &ctrl, const PlayerUseJoypad) {
                ctrl.up = Controls::pressing(BTN_DIGITAL_UP);
                ctrl.down = Controls::pressing(BTN_DIGITAL_DOWN);
                ctrl.left = Controls::pressing(BTN_DIGITAL_LEFT);
                ctrl.right = Controls::pressing(BTN_DIGITAL_RIGHT);
                ctrl.jump = Controls::pressing(BTN_DIGITAL_ACTIONDOWN);

                ctrl.pressUp = Controls::pressed(BTN_DIGITAL_UP);
                ctrl.pressDown = Controls::pressed(BTN_DIGITAL_DOWN);
                ctrl.pressLeft = Controls::pressed(BTN_DIGITAL_LEFT);
                ctrl.pressRight = Controls::pressed(BTN_DIGITAL_RIGHT);
                ctrl.pressJump = Controls::pressed(BTN_DIGITAL_ACTIONDOWN);
            });

        // Apply follower controls
        ecs.system<PlayerControls,
                   const Transform,
                   const Sensors,
                   const Speed,
                   const GroundSpeed,
                   const Player::Constants,
                   const PlayerFollowEntity>("PlayerUpdateFollow")
            .each([](PlayerControls &ctrl,
                     const Transform &t,
                     const Sensors &sensors,
                     const Speed &spd,
                     const GroundSpeed &gsp,
                     const Player::Constants &constants,
                     const PlayerFollowEntity &follow) {
                auto followg = follow.e.get<GroundSpeed>();
                auto followt = follow.e.get<Transform>();
                auto followc = follow.e.get<PlayerControls>();
                auto follows = follow.e.get<Speed>();
                
                if(!followg || !followt || !followc) return;

                // Press left/right to follow if target is too far.
                // Only works if movement direction corresponds to the direction
                // one should go
                ctrl.right = (t.position.x < (followt->position.x - 37.5f));
                ctrl.left = (t.position.x > (followt->position.x + 37.5f));

                // If target is on the other direction and we're moving way too
                // fast, backtrack immediately
                if(glm::abs(spd.speed.x) >= (constants.top_x_speed * 0.75)) {
                    if((follows->speed.x < spd.speed.x) && (t.position.x > followt->position.x)) {
                        ctrl.left = true;
                        ctrl.right = false;
                    }
                    
                    if((follows->speed.x > spd.speed.x) && (t.position.x < followt->position.x)) {
                        ctrl.left = false;
                        ctrl.right = true;
                    }
                }

                // Jumping
                auto withinJumpRange = glm::abs(t.position.x - followt->position.x) <= 200.0f;
                auto shouldJump = sensors.ground && (followt->position.y <= (t.position.y - 40.0f));
                auto remainJumping = !sensors.ground && (followt->position.y < (t.position.y - 20.0f));
                ctrl.pressJump = withinJumpRange && shouldJump;
                ctrl.jump = withinJumpRange && (shouldJump || remainJumping);

                // Look up and down
                ctrl.up = followc->up && sensors.ground && (gsp.gsp == 0.0f);
                ctrl.down = followc->down && sensors.ground && (gsp.gsp == 0.0f);
            });

        // Apply airborne movement.
        // See https://info.sonicretro.org/SPG:Air_State
        // See https://info.sonicretro.org/SPG:Jumping#Variable_Jump_Height
        ecs.system<Speed,
                   Player::State,
                   const Sensors,
                   const PlayerControls,
                   const Player::Constants>("AirborneMovement")
            .iter([](flecs::iter &it,
                     Speed *spd,
                     Player::State *state,
                     const Sensors *sensors,
                     const PlayerControls *ctrl,
                     const Player::Constants *constants) {
                for(auto i : it) {
                    if(sensors[i].ground) continue;
                    float delta = it.delta_time() * Player::BaseFrameRate;
                    float air_accel = constants[i].air_acceleration * delta;
                    float top_speed = constants[i].top_x_speed * delta;
                    float gravity = constants[i].gravity * delta;
                    float min_jump_force = constants[i].min_jump_strength * delta;
                    float air_drag_min_y = constants[i].airdrag_min_y_speed * delta;

                    /* X Movement */
                    if(ctrl[i].left) {
                        if(spd[i].speed.x > -top_speed) {
                            state[i].direction = -1.0f;
                            spd[i].speed.x -= air_accel;
                            if(spd[i].speed.x <= -top_speed)
                                spd[i].speed.x = -top_speed;
                        }
                    }

                    if(ctrl[i].right) {
                        if(spd[i].speed.x < top_speed) {
                            state[i].direction = 1.0f;
                            spd[i].speed.x += air_accel;
                            if(spd[i].speed.x >= top_speed)
                                spd[i].speed.x = top_speed;
                        }
                    }

                    /* Y Movement */

                    // Variable jump height.
                    // This should relate to an action, not just Y speed.
                    // This way, it will not interfere with gimmicks such as
                    // springs, etc.
                    // NOTE: min_jump_force is negative on this engine.
                    if((state[i].action == Player::ActionKind::Jumping)
                       && !ctrl[i].jump
                       && (spd[i].speed.y < min_jump_force)) {
                        spd[i].speed.y = min_jump_force;
                    }

                    // Apply gravity
                    spd[i].speed.y += gravity;

                    // TODO: Add Top Y Speed? Value is 16 * delta.

                    // Air drag
                    if((spd[i].speed.y < 0.0f) && (spd[i].speed.y > air_drag_min_y)) {
                        spd[i].speed.x -= (
                            std::remainder(spd[i].speed.x, constants[i].airdrag_rem_factor)
                            / constants[i].airdrag_division_factor)
                            * delta;
                    }
                    
                    
                    // TODO: Air rotation! Smoothly rotate player until
                    // reaching 0.
                }
            });

        // Apply ground X movement.
        // See https://info.sonicretro.org/SPG:Running
        ecs.system<GroundSpeed,
                   Player::State,
                   const Sensors,
                   const PlayerControls,
                   const Player::Constants>("GroundXMovement")
            .iter([](flecs::iter &it,
                     GroundSpeed *spd,
                     Player::State *state,
                     const Sensors *sensors,
                     const PlayerControls *ctrl,
                     const Player::Constants *constants) {
                for(auto i : it) {
                    if(!sensors[i].ground) continue;
                    
                    float delta = it.delta_time() * Player::BaseFrameRate;
                    float decel = constants[i].deceleration * delta;
                    float accel = constants[i].acceleration * delta;
                    float top_speed = constants[i].top_x_speed * delta;
                    float friction = constants[i].friction * delta;
                    
                    if(ctrl[i].left) {
                        if(spd[i].gsp > 0.0f) {
                            spd[i].gsp -= decel;
                            if(spd[i].gsp <= 0.0f) {
                                spd[i].gsp = -0.5f * delta;
                            }

                            if(glm::abs(spd[i].gsp) >= 4.0f) {
                                state[i].braking = true;
                            }
                        } else if(spd[i].gsp > -top_speed) {
                            state[i].direction = -1.0f;
                            spd[i].gsp -= accel;
                            state[i].braking = false;
                            if(spd[i].gsp <= -top_speed) {
                                spd[i].gsp = -top_speed;
                            }
                        }
                    }

                    if(ctrl[i].right) {
                        if(spd[i].gsp < 0.0f) {
                            spd[i].gsp += decel;
                            if(spd[i].gsp >= 0) {
                                spd[i].gsp = 0.5f * delta;
                            }

                            if(glm::abs(spd[i].gsp) >= 4.0f) {
                                state[i].braking = true;
                            }
                        } else if(spd[i].gsp < top_speed) {
                            state[i].direction = 1.0f;
                            spd[i].gsp += accel;
                            state[i].braking = false;
                            if(spd[i].gsp >= top_speed) {
                                spd[i].gsp = top_speed;
                            }
                        }
                    }

                    if(!ctrl[i].left && !ctrl[i].right) {
                        spd[i].gsp -=
                            glm::min(glm::abs(spd[i].gsp), friction)
                            * glm::sign(spd[i].gsp);
                    }
                }
            });

        // Apply ground Y movement.
        // See https://info.sonicretro.org/SPG:Jumping#Jump_Velocity
        ecs.system<Transform,
                   Speed,
                   Sensors,
                   Player::State,
                   const PlayerControls,
                   const Player::Constants>("GroundYMovement")
            .iter([](flecs::iter &it,
                     Transform *t,
                     Speed *spd,
                     Sensors *sensors,
                     Player::State *state,
                     const PlayerControls *ctrl,
                     const Player::Constants *constants) {
                for(auto i : it) {
                    if(!sensors[i].ground) continue;

                    float delta = it.delta_time() * Player::BaseFrameRate;
                    float jump_force = constants[i].jump_strength * delta;

                    // Start jump physics.
                    // NOTE: This does not control variable jump height, since
                    // it is mostly related to airborne state
                    if(ctrl[i].pressJump) {
                        sensors[i].ground = false;
                        state[i].braking = false;
                        state[i].action = Player::ActionKind::Jumping;
                        // (jump variables are always negative in this engine)
                        spd[i].speed.x += jump_force * glm::sin(t[i].angle);
                        spd[i].speed.y += jump_force * glm::cos(t[i].angle);

                        // reset angle before airborne state
                        t[i].angle = 0.0f; // TODO: remove when introducing air rotation
                    }
                }
            });

        // Apply animation.
        ecs.system<const ViewportInfo,
                   const Transform,
                   const GroundSpeed,
                   const Speed,
                   const Player::State,
                   const PlayerControls,
                   PlayerAnimation>("AnimatePlayer")
            .kind(flecs::PostUpdate)
            .iter([](flecs::iter &it,
                     const ViewportInfo *vwp,
                     const Transform *t,
                     const GroundSpeed *gsp,
                     const Speed *s,
                     const Player::State *state,
                     const PlayerControls *ctrl,
                     PlayerAnimation *anim) {
                for(auto i : it) {
                    float delta = it.delta_time() * Player::BaseFrameRate;
                    float abs_gsp = glm::abs(gsp[i].gsp);
                    float duration;
                    
                    // TODO
                    switch(state[i].action) {
                    case Player::ActionKind::Jumping:
                        anim[i].animator->setAnimationByName("Rolling");
                        duration = glm::floor(glm::max(0.0f, 2.0f - glm::abs(gsp[i].gsp)));
                        anim[i].animator->setFrameDuration(duration / Player::BaseFrameRate);
                        break;
                    default:
                        // Idle
                        if(abs_gsp == 0.0f) {
                            if(ctrl[i].down)
                                anim[i].animator->setAnimationByName("Crouching Down");
                            else if(ctrl[i].up)
                                anim[i].animator->setAnimationByName("Looking Up");
                            else
                                anim[i].animator->setAnimationByName("Idle");
                        } else {
                            if(state[i].braking
                               && (glm::sign(gsp[i].gsp) == state[i].direction))
                                anim[i].animator->setAnimationByName("Skidding");
                            else if(abs_gsp > 0.0f && abs_gsp < 6.0f) {
                                anim[i].animator->setAnimationByName("Walking");
                            } else if(abs_gsp >= 6.0f) {
                                anim[i].animator->setAnimationByName("Running");
                            }
                            duration = glm::floor(glm::max(0.0f, 8.0f - glm::abs(gsp[i].gsp)));
                            if(!state[i].braking)
                                anim[i].animator->setFrameDuration(duration / Player::BaseFrameRate);
                        }
                        break;
                    }

                    anim[i].animator->update();
                
                    // DEBUG!
                    // Projection and View should probably be in another component.
                    glm::mat4 projection, view, model;
                
                    projection = glm::ortho(0.0f, vwp[i].size.x, vwp[i].size.y, 0.0f, 1.0f, -1.0f);

                    view = glm::mat4(1.0f);

                    // DEBUG! Reposition at center of screen
                    glm::vec2 position = t[i].position;
                    // if(position.x > vwp[i].size.x / 2.0f)
                    //     position.x = vwp[i].size.x / 2.0;
                    position.y -= 8.0f;
                
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(position, 0.0f));
                    model = glm::scale(model, glm::vec3(state[i].direction * 30.0f, 30.0f, 1.0f));

                    glm::mat4 mvp = projection * view * model;
                    anim[i].animator->draw(mvp);
                }
            });
    }

    DebugCircleRender
    MakeCircleRenderer(float radius) {
        Resources::Manager::loadAtlas("resources/sprites/circle.png", glm::vec2(64.0f, 64.0f));
        auto atlas = Resources::Manager::getAtlas("resources/sprites/circle.png");
        atlas->setFrame(0);
        return { radius, false, atlas };
    }
}
