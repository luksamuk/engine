#include "level_scene.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "resources.hpp"

#include "components.hpp"

#include <iostream>
#include <sstream>

#include "level_select.hpp"

#include "imgui.h"

LevelScene::LevelScene(Tiled::LevelData l, unsigned act, std::vector<Player::Character> chars)
{
    // IF debug
    ecs.set<flecs::Rest>({});
    ecs.import<flecs::monitor>();
    std::cerr << "REST server: https://localhost:27750/entity/flecs" << std::endl
              << "Monitor: https://flecs.dev/explorer" << std::endl;
    this->chars = chars;

    Components::RegisterComponents(ecs);
}

LevelScene::~LevelScene() {}

flecs::entity
LevelScene::makePlayer(const char *name, Player::Character c, flecs::entity *follow)
{
    glm::vec2 viewportSize(320.0f, 224.0f);
    // glm::vec2 viewportSize(640.0f, 448.0f);
    
    // Player animation
    Resources::AnimatorPtr animator;

    switch(c) {
    default:
        animator = Resources::Manager::makeAnimator("resources/animation/sonic1mania.toml");
        break;
    case Player::Character::Tails:
        animator = Resources::Manager::makeAnimator("resources/animation/tails.toml");
        break;
    case Player::Character::Knuckles:
        animator = Resources::Manager::makeAnimator("resources/animation/knuckles.toml");
        break;
    }
    animator->setAnimation(0);

    flecs::entity player;

    player = ecs.entity(name);
    
    player
        .add<Components::Speed>()
        .add<Components::GroundSpeed>()
        .add<Components::Sensors>()
        .set(Player::GetConstants(c, Player::PhysicsMode::Normal))
        .add<Player::State>()
        .set(Components::ViewportInfo { viewportSize })
        .set(Components::MakeCircleRenderer(16.0f))
        .set(Components::FakeGround { 200.0f })
        .set(Components::PlayerAnimation { animator })
        .add<Components::PlayerControls>();

    if(!follow) {
        std::cout << "Not following anything" << std::endl;
        player
            .set(Components::Transform {
                glm::vec2(100.0f, 100.0f),
                0.0f
            })
            .add<Components::PlayerUseJoypad>();
    } else {
        auto t = follow->get<Components::Transform>();
        if(t != nullptr) {
            player.set(Components::Transform {
                    glm::vec2(t->position.x - 20.0f, t->position.y),
                    0.0f
                });
        } else {
            player.set(Components::Transform {
                    glm::vec2(100.0f, 100.0f),
                    0.0f
                });
        }
        
        player.set(Components::PlayerFollowEntity { *follow, 0.0f });
    }

    return player;
}

void
LevelScene::load() {
    Render::setClearColor(glm::vec4(0.392f, 0.584f, 0.929f, 1.0f));

    Resources::Manager::loadAnimator("resources/animation/sonic1mania.toml");
    Resources::Manager::loadAnimator("resources/animation/tails.toml");
    Resources::Manager::loadAnimator("resources/animation/knuckles.toml");

    flecs::entity last;
    for(unsigned i = 0; i < this->chars.size(); i++) {
        std::ostringstream ss;
        ss.clear();
        ss << "P" << i;
        auto name = ss.str();
        if(i == 0)
            last = makePlayer(name.c_str(), this->chars[i], nullptr);
        else last = makePlayer(name.c_str(), this->chars[i], &last);
        std::cout << name << ": " << last << std::endl;
    }
    
    // TODO
    // flecs::entity player = makePlayer("Sonic", Player::Character::Sonic, nullptr);
    // flecs::entity tails = makePlayer("Tails", Player::Character::Tails, &player);
    // flecs::entity knuckles = makePlayer("Knuckles", Player::Character::Knuckles, &tails);
    
    // std::cout << "Sonic entity: " << player << std::endl
    //           << "Tails entity: " << tails << std::endl
    //           << "Knuckles entity: " << knuckles << std::endl;
}

void
LevelScene::unload() {}

static bool dbg_window_active = true;

void
LevelScene::update(double dt)
{
    ecs.progress(dt);

    // Debug panel
    flecs::filter<const Components::Transform,
                  const Components::Speed,
                  const Components::GroundSpeed> f =
        ecs.filter<const Components::Transform,
                   const Components::Speed,
                   const Components::GroundSpeed>();

    ImGui::SetNextWindowPos(ImVec2(6, 15), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);

    static ImGuiTableFlags tableflags =
        ImGuiTableFlags_SizingFixedFit
        | ImGuiTableFlags_RowBg
        | ImGuiTableFlags_BordersOuter
        | ImGuiTableFlags_Borders;

    ImGui::Begin("Debug", &dbg_window_active, ImGuiWindowFlags_NoFocusOnAppearing);
    if(ImGui::BeginTable("Entities", 7, tableflags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        //ImGui::TableSetupColumn("Position", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Xpos",  ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Ypos",  ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Theta", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Xsp",   ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Ysp",   ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Gsp",   ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        f.iter([](flecs::iter &it,
                  const Components::Transform *t,
                  const Components::Speed *s,
                  const Components::GroundSpeed *g){
            for(auto i : it) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", std::string(it.entity(i).name()).c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%0.4f", t[i].position.x);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%0.4f", t[i].position.y);
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%0.1f", t[i].angle);
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%0.4f", s[i].speed.x);
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%0.4f", s[i].speed.y);
                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%0.4f", g[i].gsp);
            }
        });
        ImGui::EndTable();
    }

    ImGui::End();
    

    if(Controls::pressed(BTN_DIGITAL_OPTION)) {
        Scenes::Manager::add(new LevelSelect());
        setShouldUnload(true);
    }
}

void
LevelScene::draw() {}
