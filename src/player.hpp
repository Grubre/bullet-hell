#include "common.hpp"
#include "components.hpp"

namespace bh {
struct Player {};

template <> inline void emplace<Player>(entt::registry &registry, entt::entity entity) {
    emplace<Transform>(registry, entity);
    registry.emplace<Player>(entity);
}

[[nodiscard]] auto make_player(entt::registry &registry) -> entt::entity;
} // namespace bh
