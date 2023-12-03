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

    struct Accelerator {
        glm::vec2 accel;
    };

    struct ViewportInfo {
        glm::vec2 size;
    };

    struct DebugCircleRender {
        Resources::AtlasPtr atlas;
        float               radius;
    };
    

    // Collision components

    struct Sensors {
        bool ground;
        bool ceiling;
        bool left;
        bool right;
    };
    

    // Player components
    
    struct PlayerControl {};


    // Functions
    void              RegisterDefaultComponents(flecs::world &ecs);
    DebugCircleRender MakeCircleRenderer(float radius);
}

#endif
