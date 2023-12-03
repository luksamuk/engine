#include "level_scene.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "resources.hpp"

#include "components.hpp"

#include <iostream>

#include "level_select.hpp"

LevelScene::LevelScene(Tiled::LevelData l, unsigned act)
{
    // IF debug
    ecs.set<flecs::Rest>({});
    ecs.import<flecs::monitor>();
    std::cerr << "REST server: https://localhost:27750/entity/flecs" << std::endl
              << "Monitor: https://flecs.dev/explorer" << std::endl;

    Components::RegisterComponents(ecs);
}

LevelScene::~LevelScene() {}

void
LevelScene::load() {
    glm::vec2 viewportSize(320.0f, 224.0f);
    
    // TODO
    flecs::entity player = ecs.entity("Sonic")
        .add<Components::Speed>()
        .add<Components::GroundSpeed>()
        .add<Components::Accelerator>()
        //.add<Components::Sensors>()
        .set(Components::Sensors { true, false, false, false })
        .set(Components::Transform {
                glm::vec2(100.0f, 100.0f),
                0.0f
            })
        .set(Player::GetConstants(Player::Character::Sonic,
                                  Player::PhysicsMode::Normal))
        .add<Components::PlayerControl>()
        .set(Components::ViewportInfo { viewportSize })
        .set(Components::MakeCircleRenderer(15.0f));
    
    std::cout << "Sonic entity: " << player << std::endl;
}

void
LevelScene::unload() {}

void
LevelScene::update(double dt)
{
    ecs.progress(dt);

    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }
}

void
LevelScene::draw() {}
