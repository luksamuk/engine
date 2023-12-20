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

        ecs.component<PlayerFollowEntity>()
            .member<flecs::entity>("e")
            .member<float>("jumpTimer");

        ecs.component<DebugCircleRender>()
            .member<float>("radius")
            .member<bool>("visible");

        ecs.component<FakeGround>()
            .member<float>("ypos");

        ecs.component<CameraInfo>()
            .member<flecs::entity>("camera");
        
        ecs.component<CameraBox>()
            .member<bool>("visible");

        ecs.component<CameraFollowed>();

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

        // Render camera box
        ecs.system<const CameraBox, const ViewportInfo>("RenderCameraBox")
            .kind(flecs::PostUpdate)
            .each([](const CameraBox &box, const ViewportInfo &vwp) {
                if(!box.visible) return;

                glm::mat4 mvp = glm::ortho(0.0f, vwp.size.x, vwp.size.y, 0.0f, 1.0f, -1.0f);
                mvp = glm::translate(mvp, glm::vec3(vwp.size / 2.0f, 0.0f));
                mvp = glm::scale(mvp, glm::vec3(16.0f, 64.0f, 1.0f));
                box.atlas->draw(mvp);
            });

        // Camera follows object
        ecs.system<const CameraInfo, const Transform, const CameraFollowed>("CameraFollowsEntity")
            .iter([](flecs::iter &it,
                     const CameraInfo *info,
                     const Transform *t,
                     const CameraFollowed*) {
                for(auto i : it) {
                    auto camera_t = info[i].camera.get_mut<Transform>();
                    auto camera_vwp = info[i].camera.get<ViewportInfo>();
                    if(!camera_t || !camera_vwp) continue;

                    const float catch_up_lag = 16.0f;
                    const float catch_up_lag_y_lowspd = 6.0f;
                    // const float catch_up_lag_y_crouchdown = 2.0f;
                    
                    // Calculate borders. Rectangle is 16x64
                    auto left = camera_t->position.x - 8.0f;
                    auto right = camera_t->position.x + 8.0f;
                    auto top = camera_t->position.y - 32.0f;
                    auto bottom = camera_t->position.y + 32.0f;
                    float displacement;

                    // Horizontal border
                    displacement = left - t[i].position.x;
                    if(displacement > 0.0f) {
                        camera_t->position.x -= glm::min(displacement, catch_up_lag);
                    }

                    displacement = t[i].position.x - right;
                    if(displacement > 0.0f) {
                        camera_t->position.x += glm::min(displacement, catch_up_lag);
                    }

                    // Vertical border
                    // Check if camera target has a Sensors component.
                    auto player_sensors = it.entity(i).get<Sensors>();
                    auto player_gsp = it.entity(i).get<GroundSpeed>();
                    if((player_sensors && player_gsp) && player_sensors->ground) {
                        // If following a player and it is on the ground...
                        displacement = t[i].position.y - camera_t->position.y;
                        if(displacement != 0.0f) {
                            camera_t->position.y += glm::min(
                                displacement,
                                glm::abs(player_gsp->gsp) >= 8.0f
                                ? catch_up_lag
                                : catch_up_lag_y_lowspd);
                        }
                    } else {
                        // If this is not a player or we're in the air
                        displacement = top - t[i].position.y;
                        if(displacement > 0.0f) {
                            camera_t->position.y -= glm::min(displacement, catch_up_lag);
                        }

                        displacement = t[i].position.y - bottom;
                        if(displacement > 0.0f) {
                            camera_t->position.y += glm::min(displacement, catch_up_lag);
                        }
                    }
                }
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
        // http://info.sonicretro.org/SPG:Player_2
        ecs.system<PlayerControls,
                   Transform,
                   const Sensors,
                   const GroundSpeed,
                   Player::State,
                   PlayerFollowEntity>("PlayerUpdateFollow")
            .iter([](flecs::iter& it,
                     PlayerControls *ctrl,
                     Transform *t,
                     const Sensors *sensors,
                     const GroundSpeed *gsp,
                     Player::State *state,
                     PlayerFollowEntity *follow) {
                for(auto i : it) {
                    auto followg = follow[i].e.get<GroundSpeed>();
                    auto followt = follow[i].e.get<Transform>();
                    auto followc = follow[i].e.get<PlayerControls>();
                    auto follows = follow[i].e.get<Speed>();
                    auto followstate = follow[i].e.get<Player::State>();
                    auto followsensors = follow[i].e.get<Sensors>();

                    const float near_factor = 20.0f;
                
                    if(!followg || !followt || !followc || !follows || !followstate || !followsensors)
                        return;

                    float delta = it.delta_time() * Player::BaseFrameRate;

                    // Update jump timer
                    follow[i].jumpTimer += 1.0f;
                    
                    // Reset inputs
                    ctrl[i].left = ctrl[i].right = ctrl[i].pressJump = ctrl[i].jump = false;

                    if(t[i].position.x > followt->position.x) {
                        if(glm::abs(t[i].position.x - followt->position.x) > near_factor) {
                            ctrl[i].left = true;
                            ctrl[i].right = false;
                        }

                        if((gsp[i].gsp != 0.0f)
                           && (state[i].action != Player::ActionKind::Pushing)
                           && (state[i].direction > 0.0f)) {
                            t[i].position.x -= 1.0f * delta;
                        }
                    }

                    if(t[i].position.x < followt->position.x) {
                        if(glm::abs(t[i].position.x - followt->position.x) > near_factor) {
                            ctrl[i].left = false;
                            ctrl[i].right = true;
                        }

                        if((gsp[i].gsp != 0.0f)
                           && (state[i].action != Player::ActionKind::Pushing)
                           && (state[i].direction < 0.0f)) {
                            t[i].position.x += 1.0f * delta;
                        }
                    }

                    if(glm::abs(t[i].position.x - followt->position.x) < near_factor) {
                        if(gsp[i].gsp == 0.0f)
                            state[i].direction = followstate->direction;
                    }

                    // Look up and down
                    ctrl[i].up = followc->up && sensors[i].ground && (gsp[i].gsp == 0.0f);
                    ctrl[i].down = followc->down && sensors[i].ground && (gsp[i].gsp == 0.0f);
                
                    /* Jumping */

                    // old code
                    auto withinJumpRange = glm::abs(t[i].position.x - followt->position.x) <= 64.0f;
                    auto shouldJump = sensors[i].ground && (followt->position.y <= (t[i].position.y - 40.0f));
                    auto remainJumping = !sensors[i].ground && (followt->position.y < (t[i].position.y - 20.0f));
                    ctrl[i].pressJump = withinJumpRange && shouldJump;
                    ctrl[i].jump = withinJumpRange && (shouldJump || remainJumping);

                //     // Continuing a jump
                //     if(!sensors[i].ground && (state[i].action == Player::ActionKind::Jumping)) {
                //         ctrl[i].jump = true;
                //         goto END_P2_JUMP_LOGIC;
                //     }

                //     // Too far away from target
                //     if(glm::abs(t[i].position.x - followt->position.x) > 64.0f) { 
                //         goto END_P2_JUMP_LOGIC;
                //     }

                //     // Too far below target
                //     if((followt->position.y >= t[i].position.y)
                //        || (followt->position.y < (t[i].position.y - 32.0f))) {
                //         goto END_P2_JUMP_LOGIC;
                //     }

                //     // Perform jump
                //     ctrl[i].pressJump = true;
                //     ctrl[i].jump = true;

                //     // Jump every 64 frames
                //     // if(follow[i].jumpTimer >= 64.0f) {
                //     //     follow[i].jumpTimer = std::remainder(follow[i].jumpTimer, 64.0f);
                //     //     // Perform jump
                //     //     ctrl[i].pressJump = true;
                //     //     ctrl[i].jump = true;
                //     //     continue;
                //     // }

                // END_P2_JUMP_LOGIC:

                //     if(follow[i].jumpTimer >= 64.0f) {
                //         // Reset timer
                //         follow[i].jumpTimer = std::remainder(follow[i].jumpTimer, 64.0f);
                //     }
                }
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
        ecs.system<//const ViewportInfo,
                   const Transform,
                   const GroundSpeed,
                   const Speed,
                   const Player::State,
                   const PlayerControls,
                   const CameraInfo,
                   PlayerAnimation>("AnimatePlayer")
            .kind(flecs::PostUpdate)
            .iter([](flecs::iter &it,
                     //const ViewportInfo *vwp,
                     const Transform *t,
                     const GroundSpeed *gsp,
                     const Speed *s,
                     const Player::State *state,
                     const PlayerControls *ctrl,
                     const CameraInfo *caminfo,
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

                    // Get camera stuff
                    auto camera_t = caminfo[i].camera.get<Transform>();
                    auto camera_vwp = caminfo[i].camera.get<ViewportInfo>();

                    if(!camera_t || !camera_vwp) continue;
                    
                    // DEBUG!
                    // Projection and View should probably be in another component.
                    glm::mat4 projection, view, model;
                
                    projection = glm::ortho(0.0f, camera_vwp->size.x, camera_vwp->size.y, 0.0f, 1.0f, -1.0f);

                    view = glm::mat4(1.0f);

                    // DEBUG! Reposition at center of screen
                    glm::vec2 position = t[i].position - camera_t->position;
                    // if(position.x > vwp[i].size.x / 2.0f)
                    //     position.x = vwp[i].size.x / 2.0;
                    position.y -= 8.0f;
                    position += camera_vwp->size / 2.0f;
                
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

    CameraBox
    MakeCameraBox() {
        Resources::Manager::loadAtlas("resources/sprites/camerabox.png", glm::vec2(16.0f, 64.0f));
        auto atlas = Resources::Manager::getAtlas("resources/sprites/camerabox.png");
        atlas->setFrame(0);
        return { false, atlas };
    }
}
