#include "components/relations.hpp"
#include "components/velocity.hpp"
#include "components/common.hpp"
#include "fmt/base.h"
#include <raylib.h>
#include <raymath.h>
#include <utility>
#include <variant>
#include <vector>

namespace bh {

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

struct Circle {
    float radius;
};

struct CollisionBody {
    std::variant<Circle> body;
    entt::entity handler;
};

struct EnterCollisionEvent {
    entt::entity body;
    entt::entity with;
    entt::entity handler;
};

struct CollisionHandlerConcept: entt::type_list<void(entt::registry&, EnterCollisionEvent&)> {
    template<typename Base>
    struct type: Base {
        void on_collision_entered(entt::registry &registry, EnterCollisionEvent& event) { entt::poly_call<0>(*this,registry,  event); }
    };

    template<typename Type>
    using impl = entt::value_list<&Type::on_collision_entered>;
};

using CollisionHandler = entt::poly<CollisionHandlerConcept>;

template<typename T>
using EventQueue = std::vector<T>;

template<typename T>
void emplace_collision_body(entt::registry &registry, entt::entity entity, T body) {
    emplace<CollisionBody>(registry, entity, body, entt::tombstone);
    emplace<GlobalTransform>(registry, entity);
}

template<typename T>
void emplace_collision_body(entt::registry &registry, entt::entity entity, T body, entt::entity handler) {
    emplace<CollisionBody>(registry, entity, body, handler);
    emplace<GlobalTransform>(registry, entity);
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

inline void init_collision_event_queues(entt::registry& registry) {
    registry.ctx().emplace<EventQueue<EnterCollisionEvent>>();
}

inline void test_collisions(entt::registry& registry) {
    auto view = registry.view<GlobalTransform, CollisionBody>();

    auto &enter_collision_queue = registry.ctx().get<EventQueue<EnterCollisionEvent>>();

    for (const auto &&[a_e, a_tr, a] : view.each()) {
        for (const auto &&[b_e, b_tr, b] : view.each()) {
            if (a_e < b_e && collides_with(a, a_tr.transform, b, b_tr.transform)) {
                if (registry.valid(a.handler)) {
                    enter_collision_queue.emplace_back(a_e, b_e, a.handler);
                }
                if (registry.valid(b.handler)) {
                    enter_collision_queue.emplace_back(b_e, a_e, b.handler);
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

inline void dipatch_events(entt::registry& registry) {
    auto& queue = registry.ctx().get<EventQueue<EnterCollisionEvent>>();

    for (auto &event : queue) {
        auto handler = registry.get<CollisionHandler>(event.handler);
        handler->on_collision_entered(registry, event);
    }

    queue.clear();
}

}
