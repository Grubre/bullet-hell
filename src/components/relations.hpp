#pragma once

#include "components/velocity.hpp"
#include <entt.hpp>

namespace bh {

struct Parented {
    entt::entity parent;
};

inline void destroy_unparented(entt::registry &registry) {
    auto view = registry.view<Parented>();

    for (auto &&[self, parent] : view.each()) {
        if (!registry.valid(parent.parent)) {
            registry.destroy(self);
        }
    }
}

/// Zakłada, że wszystkie parenty są validne
/// Najlepiej wcześniej puścić "destroy_unparented", żeby się nie zdziwić
inline void propagate_parent_transform(entt::registry &registry) {
    auto view = registry.view<Parented, bh::GlobalTransform, bh::LocalTransform>();

    for (auto &&[self, parent, global, local] : view.each()) {
        auto parent_transform = registry.get<bh::GlobalTransform>(parent.parent);
        global.transform = parent_transform.transform.combine(local.transform);
    }
}

}