#include "core.hpp"
#include "scene.hpp"

#include "test_scene.hpp"
#include "sprite_scene.hpp"
#include "title_screen.hpp"

int
main(void)
{
    Core::init();

    Scenes::Manager::add(new TitleScreen());
    
    Core::gameLoop();
    Core::dispose();
    
    return 0;
}
