#ifndef COMPONENTS_HPP_INCLUDED
#define COMPONENTS_HPP_INCLUDED

#include <flecs.h>
#include "components_base.hpp"
#include "components_default.hpp"

namespace Components
{
    void RegisterComponents(flecs::world &ecs);
}

#endif
