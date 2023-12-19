#ifndef COMPONENTS_DEFAULT_HPP_INCLUDED
#define COMPONENTS_DEFAULT_HPP_INCLUDED

#include <glm/glm.hpp>
#include <flecs.h>

#include "player_constants.hpp"

#include "sprite.hpp"
#include "resources.hpp"

namespace Components
{
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


    // Functions
    void              RegisterDefaultComponents(flecs::world &ecs);
    DebugCircleRender MakeCircleRenderer(float radius);
}

#endif
