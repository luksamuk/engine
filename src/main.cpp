#include "core.hpp"
#include "scene.hpp"

#include "test_scene.hpp"
#include "sprite_scene.hpp"

int
main(void)
{
    initCore();

    //SceneManager::add(new TestScene());
    SceneManager::add(new SpriteScene());

    coreGameLoop();
    disposeCore();
    return 0;
}
