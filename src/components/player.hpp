#include "common.hpp"
#include "velocity.hpp"

namespace bh {
struct Alive{};
struct Health{
    int health = 100;
};
struct Player {

};

template <> inline void emplace<Player>(entt::registry &registry, entt::entity entity) {
    emplace<LocalTransform>(registry, entity);
    emplace<Health>(registry,entity);
    emplace<Alive>(registry,entity);
    registry.emplace<Player>(entity);
}

[[nodiscard]] auto make_player(entt::registry &registry) -> entt::entity {
    const auto entity = registry.create();
    bh::emplace<bh::Player>(registry, entity);

    return entity;
}
} // namespace bh
