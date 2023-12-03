#include "components_base.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Components
{
    inline void
    RegisterGLMComponents(flecs::world &ecs)
    {
        ecs.component<glm::vec2>()
            .member<float>("x")
            .member<float>("y");

        ecs.component<glm::vec3>()
            .member<float>("x")
            .member<float>("y")
            .member<float>("z");

        ecs.component<glm::mat4>();

        ecs.component<glm::quat>();
    }
    
    void
    RegisterBaseComponents(flecs::world &ecs)
    {
        RegisterGLMComponents(ecs);
    }

}
