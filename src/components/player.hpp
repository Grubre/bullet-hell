#include "assets/asset_manager.hpp"
#include "common.hpp"
#include "components/sprite.hpp"
#include "velocity.hpp"
#include "keyinput.hpp"
#include <fmt/printf.h>
namespace bh {
struct Alive {
    static constexpr auto name = "Alive";
    static void inspect() { ImGui::Text("Entity Alive :>"); }
};

struct Health {
    static constexpr auto name = "Health";
    int maxHealth = 100;
    int health = 100;
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        static constexpr auto minimum = 0;
        ImGui::DragInt("Health", &health, 1, minimum, maxHealth);
    }
};

void deal_damage(entt::registry &registry, entt::entity &entity, int amount) {
    auto &health = registry.get<Health>(entity);
    health.health -= amount;
    if (health.health <= 0) {
        registry.remove<Alive>(entity);
    }
}

struct Player {
    static constexpr auto name = "Player";
    float speed = 1;
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        static constexpr auto minimum = 0;
        ImGui::DragFloat("Speed", &speed, 0.5f, minimum, 1000);
    }
};

template <> inline void emplace<Player>(entt::registry &registry, entt::entity entity) {
    emplace<Velocity>(registry, entity);
    emplace<Health>(registry, entity);
    emplace<Alive>(registry, entity);
    emplace<Sprite>(registry, entity, TextureEnum::PLAYER_TEXTURE);
    safe_emplace<Player>(registry, entity);
}

[[nodiscard]] auto make_player(entt::registry &registry) -> entt::entity {
    const auto entity = registry.create();
    bh::emplace<bh::Player>(registry, entity);
    using KE = bh::KeyboardEvent;
    auto &vel = registry.get<Velocity>(entity);
    auto &manager = registry.ctx().get<bh::KeyManager>();
    auto &player = registry.get<Player>(entity);
    manager.subscribe(KE::DOWN, KEY_S, [&]() { vel.y += player.speed; });
    manager.subscribe(KE::DOWN, KEY_W, [&]() { vel.y -= player.speed; });
    manager.subscribe(KE::DOWN, KEY_A, [&]() { vel.x -= player.speed; });
    manager.subscribe(KE::DOWN, KEY_D, [&]() { vel.x += player.speed; });
    return entity;
}
} // namespace bh
