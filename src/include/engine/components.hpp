#ifndef COMPONENTS_HPP_INCLUDED
#define COMPONENTS_HPP_INCLUDED

#include <flecs.h>
#include "engine/player_constants.hpp"

#include "engine/sprite.hpp"
#include "engine/resources.hpp"
#include "engine/tiled.hpp"

#include "engine/sound.hpp"

namespace Components
{
    void RegisterComponents(flecs::world &ecs);
    void RegisterBaseComponents(flecs::world &ecs);

    // Basic components.
    // These components mostly relate to game logic!
    
    struct Transform {
        glm::vec2 position;
        float     angle;
    };

    struct Speed {
        glm::vec2 speed;
    };

    struct GroundSpeed {
        float gsp;
    };

    struct ViewportInfo {
        glm::vec2 size;
    };

    struct DebugCircleRender {
        float               radius;
        bool                visible = false;
        Resources::AtlasPtr atlas;
    };
    

    // Collision components

    struct Sensors {
        bool ground;
        bool ceiling;
        bool left;
        bool right;
    };

    struct LevelInfo {
        Resources::LevelPtr lvl;
    };
    

    // Player components
    
    struct FakeGround {
        float ypos;
    };

    struct PlayerAnimation {
        Resources::AnimatorPtr animator;
    };

    struct PlayerUseJoypad {};

    struct PlayerFollowEntity {
        flecs::entity e;
        float         jumpTimer;
    };

    struct PlayerControls {
        bool up;
        bool down;
        bool left;
        bool right;
        bool jump;

        bool pressUp;
        bool pressDown;
        bool pressLeft;
        bool pressRight;
        bool pressJump;
    };

    // Audio components

    struct SoundEmitter
    {
        std::string             current = "";
        Sound::AudioSourceIndex chn;
        Resources::BGMTablePtr  table;

        void play(std::string name);
    };

    // Other components

    struct CameraInfo {
        // A camera must have a Transform and a ViewportInfo.
        flecs::entity        camera;
    };

    struct CameraBox {
        bool                 visible = false;
        Resources::AtlasPtr  atlas;
    };

    struct CameraFollowed {};


    // Functions
    void              RegisterDefaultComponents(flecs::world &ecs);
    void              RegisterSoundComponents(flecs::world &ecs);
    DebugCircleRender MakeCircleRenderer(float radius);
    CameraBox         MakeCameraBox();
}

#endif
