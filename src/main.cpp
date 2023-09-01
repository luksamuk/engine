#include "core.hpp"
#include "scene.hpp"

#include "test_scene.hpp"
#include "sprite_scene.hpp"
#include "level_select.hpp"

int
main(void)
{
    initCore();

    SceneManager::add(new LevelSelect());
    //SceneManager::add(new TestScene());

    coreGameLoop();
    disposeCore();
    return 0;
}
