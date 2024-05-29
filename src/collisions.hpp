#include "components/relations.hpp"
#include "components/velocity.hpp"
#include "components/common.hpp"
#include <fmt/core.h>
#include <raylib.h>
#include <raymath.h>
#include <utility>
#include <variant>
#include <vector>

namespace bh {

template <class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};

struct Circle {
    constexpr static auto name = "Circle";
    float radius;

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat("Radius", &radius, 1.f);
    }
};

struct Rect {
    constexpr static auto name = "Rect";
    float width;
    float height;

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat2("Width/Height", &width, 1.f);
    }
};

template <typename... Shapes> struct CollisionBodyT {
    std::variant<Shapes...> body;
    entt::entity handler;

    static constexpr auto name = "CollisionBody";

    void iterate_shapes(auto &&f) { (f.template operator()<Shapes>(), ...); }

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        auto current_component = (int)body.index();
        auto i = 0;

        iterate_shapes([&]<typename T>() {
            ImGui::RadioButton(T::name, &current_component, i++);
            if (i != sizeof...(Shapes)) {
                ImGui::SameLine();
            }
        });

        i = 0u;
        if ((int)body.index() != current_component) {
            iterate_shapes([&]<typename T>() {
                if (i++ == current_component) {
                    body = T{};
                }
            });
        }

        std::visit(overloaded{[&](auto &c) { c.inspect(registry, entity); }}, body);

        ImGui::Text("Handler %d", (int)handler);
    }
};

using CollisionBody = CollisionBodyT<Circle, Rect>;

struct EnterCollisionEvent {
    entt::entity body;
    entt::entity with;
    entt::entity handler;
};

struct CollisionHandlerConcept : entt::type_list<void(entt::registry &, EnterCollisionEvent &)> {
    template <typename Base> struct type : Base {
        void on_collision_entered(entt::registry &registry, EnterCollisionEvent &event) {
            entt::poly_call<0>(*this, registry, event);
        }
    };

    template <typename Type> using impl = entt::value_list<&Type::on_collision_entered>;
};

using CollisionHandler = entt::poly<CollisionHandlerConcept>;

template <typename T> using EventQueue = std::vector<T>;

template <typename T> void emplace_collision_body(entt::registry &registry, entt::entity entity, T body) {
    emplace<CollisionBody>(registry, entity, body, entt::tombstone);
    emplace<GlobalTransform>(registry, entity);
}

template <typename T>
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

inline bool rect_circle_test(Rect &a, Transform &a_tr, Circle &b, Transform &b_tr) {
    auto a_pos = a_tr.position;
	auto b_pos = b_tr.position;

	auto a_half = Vector2{a.width / 2.f, a.height / 2.f};

	auto a_tl = Vector2Subtract(a_pos, a_half);
	auto a_br = Vector2Add(a_pos, a_half);

	auto b_center = b_pos;

	auto closest = Vector2{std::clamp(b_center.x, a_tl.x, a_br.x),
						   std::clamp(b_center.y, a_tl.y, a_br.y)};

	auto diff = Vector2Subtract(b_center, closest);

	return Vector2Length(diff) <= b.radius;
}

inline bool rect_rect_test(Rect &a, Transform &a_tr, Rect &b, Transform &b_tr) {
    auto a_pos = a_tr.position;
	auto b_pos = b_tr.position;

	auto a_half = Vector2{a.width / 2.f, a.height / 2.f};
	auto b_half = Vector2{b.width / 2.f, b.height / 2.f};

	auto a_tl = Vector2Subtract(a_pos, a_half);
	auto a_br = Vector2Add(a_pos, a_half);

	auto b_tl = Vector2Subtract(b_pos, b_half);
	auto b_br = Vector2Add(b_pos, b_half);

	return a_tl.x < b_br.x && a_br.x > b_tl.x && a_tl.y < b_br.y && a_br.y > b_tl.y;
}

inline bool collides_with(CollisionBody &a, Transform &a_tr, CollisionBody &b, Transform &b_tr) {
    return std::visit(overloaded{
                          [&](Circle &a, Circle &b) { return circle_circle_test(a, a_tr, b, b_tr); },
                          [&](Rect &a, Circle &b) { return rect_circle_test(a, a_tr, b, b_tr); },
                          [&](Circle &a, Rect &b) { return rect_circle_test(b, b_tr, a, a_tr); },
                          [&](Rect &a, Rect &b) { return rect_rect_test(a, a_tr, b, b_tr); },
                      },
                      a.body, b.body);
}

inline void init_collision_event_queues(entt::registry &registry) {
    registry.ctx().emplace<EventQueue<EnterCollisionEvent>>();
}

inline void test_collisions(entt::registry &registry) {
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

inline void debug_draw_collsions(entt::registry &registry) {
    auto view = registry.view<GlobalTransform, CollisionBody>();

    for (const auto &&[a_e, a_tr, a] : view.each()) {
        auto color = ColorAlpha(BLUE, 0.5f);

        std::visit(overloaded{
                       [&](Circle &c) {
                           auto center = a_tr.transform.position;
                           DrawCircle((int)center.x, (int)center.y, c.radius, color);
                       },
                       [&](Rect &r) {
                           auto pos = a_tr.transform.position;
                           auto rot = a_tr.transform.rotation;
                           DrawRectanglePro(Rectangle{pos.x, pos.y, r.width, r.height},
                                            Vector2(r.width / 2.f, r.height / 2.f), rot * RAD2DEG, color);
                       },
                   },
                   a.body);
    }
}

inline void dipatch_events(entt::registry &registry) {
    auto &queue = registry.ctx().get<EventQueue<EnterCollisionEvent>>();

    for (auto &event : queue) {
        auto handler = registry.get<CollisionHandler>(event.handler);
        handler->on_collision_entered(registry, event);
    }

    queue.clear();
}

} // namespace bh
