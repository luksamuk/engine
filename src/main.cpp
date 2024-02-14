#include "engine/core.hpp"
#include "engine/scene.hpp"

#include "test_scene.hpp"
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
