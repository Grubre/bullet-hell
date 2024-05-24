#pragma once
#include <entt.hpp>

namespace bh::common {

template <typename T> void emplace(entt::registry &registry, entt::entity entity) { registry.emplace<T>(entity); }

} // namespace bh::common
