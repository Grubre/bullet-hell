#include "player.hpp"

auto bh::make_player(entt::registry &registry) -> entt::entity {
    const auto entity = registry.create();
    bh::emplace<bh::Player>(registry, entity);
    return entity;
}
