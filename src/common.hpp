#pragma once
#include <entt.hpp>

namespace bh {

template <typename T, typename... Args> void emplace(entt::registry &registry, entt::entity entity, Args &&...args) {
    registry.emplace<T>(entity, std::forward(args)...);
}

} // namespace bh
