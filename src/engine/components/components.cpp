#include "engine/components.hpp"

namespace Components
{
    void
    RegisterComponents(flecs::world &ecs)
    {
        RegisterBaseComponents(ecs);
        RegisterDefaultComponents(ecs);
    }
}
