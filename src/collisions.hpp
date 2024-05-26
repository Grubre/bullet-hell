#include "components/relations.hpp"
#include "components/velocity.hpp"
#include "components/common.hpp"
#include "fmt/base.h"
#include <raylib.h>
#include <raymath.h>
#include <variant>

namespace bh {

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

struct Circle {
    float radius;
};

struct CollisionBody {
    std::variant<Circle> body;
};

struct CollisionHandler {
    entt::dispatcher dispatcher;
};

struct CollidesWithEvent {
    entt::entity with;
};

template<typename T>
void add_collision_body_to(entt::registry &registry, entt::entity parent_entity, T body) {
    auto body_entity = registry.create();
    emplace<CollisionBody>(registry, body_entity, body);
    emplace<GlobalTransform>(registry, body_entity);
    emplace<Parented>(registry, body_entity, parent_entity);
}

inline bool circle_circle_test(Circle &a, Transform &a_tr, Circle &b, Transform &b_tr) {
    auto a_center = a_tr.position;
    auto b_center = b_tr.position;

    auto diff = Vector2Subtract(a_center, b_center);

    return Vector2Length(diff) <= a.radius + b.radius;
}

inline bool collides_with(CollisionBody& a, Transform &a_tr, CollisionBody &b, Transform &b_tr) {
    return std::visit(overloaded {
        [&](Circle& a_c, Circle& b_c) { return circle_circle_test(a_c, a_tr, b_c, b_tr); },
    }, a.body, b.body);
}

inline void test_collsions(entt::registry& registry) {
    auto view = registry.view<GlobalTransform, CollisionBody>();

    for (const auto &&[a_e, a_tr, a] : view.each()) {
        for (const auto &&[b_e, b_tr, b] : view.each()) {
            if (a_e < b_e && collides_with(a, a_tr.transform, b, b_tr.transform)) {
                if (registry.all_of<Parented>(a_e)) {
                    auto parent = registry.get<Parented>(a_e);
                    if (registry.all_of<CollisionHandler>(parent.parent)) {
                        auto &handler = registry.get<CollisionHandler>(parent.parent);
                        handler.dispatcher.enqueue(CollidesWithEvent { b_e });
                    }
                }
                if (registry.all_of<Parented>(b_e)) {
                    auto parent = registry.get<Parented>(b_e);
                    if (registry.all_of<CollisionHandler>(parent.parent)) {
                        auto &handler = registry.get<CollisionHandler>(parent.parent);
                        handler.dispatcher.enqueue(CollidesWithEvent { a_e });
                    }
                }
            }
        }   
    }
}

inline void debug_draw_collsions(entt::registry& registry) {
    auto view = registry.view<GlobalTransform, CollisionBody>();

    for (const auto &&[a_e, a_tr, a] : view.each()) {
        std::visit(overloaded {
            [&](Circle& a_c) { 
                auto center = a_tr.transform.position;
                DrawCircle((int)center.x, (int)center.y, a_c.radius, ColorAlpha(BLUE, 0.5f));
            },
        }, a.body);
    }
}

}
