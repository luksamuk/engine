#include "entity_test.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "level_select.hpp"
#include "core.hpp"

EntityTest::EntityTest()
{
    // Test by curl'ing http://localhost:27750/entity/flecs
    // or access https://flecs.dev/explorer
    ecs.set<flecs::Rest>({});

    
}

EntityTest::~EntityTest() {}

void EntityTest::load() {
    
}

void EntityTest::unload() {}

void EntityTest::update(double dt)
{
    ecs.progress();
    
    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }
}

void EntityTest::draw() {}
