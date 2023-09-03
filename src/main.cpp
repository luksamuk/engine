#include "core.hpp"
#include "scene.hpp"

#include "test_scene.hpp"
#include "sprite_scene.hpp"
#include "level_select.hpp"

int
main(void)
{
    Core::init();

    Scenes::Manager::add(new LevelSelect());

    Core::gameLoop();
    Core::dispose();
    
    return 0;
}
