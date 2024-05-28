#include "components/sprite.hpp"
#include <cstdint>
#include <entt.hpp>
#include <utility>
#include <imgui.h>
#include <raylib.h>

namespace bh {

struct ShootingState;
struct MovingState;

struct StateI {
    virtual void remove_self(entt::registry &registry, entt::entity entity) = 0;
};

struct CurrentState {
    StateI *x;
};

template <typename From, typename To, typename... Args>
void move_to_state(entt::registry &registry, entt::entity entity, const Args &...args) {
    auto &current_state = registry.get<CurrentState>(entity);
    current_state.x = &registry.emplace<To>(entity, args...);
}

template <typename T, typename C>
concept StateC = requires(T t, entt::registry &registry, entt::entity entity) {
    { t.template move_to_state<C>(registry, entity) };
} && std::is_base_of_v<StateI, T> && std::is_base_of_v<StateI, C> && std::is_default_constructible_v<T>;

#define IMPL_STATEC                                                                                                    \
    void remove_self(entt::registry &registry, entt::entity entity) override {                                         \
        registry.remove<std::decay_t<decltype(*this)>>(entity);                                                        \
    }                                                                                                                  \
    template <typename To, typename... Args>                                                                           \
    void move_self_to_state(entt::registry &registry, entt::entity entity, const Args &...args) {                           \
        move_to_state<std::decay_t<decltype(*this)>, To>(registry, entity, args...);                                       \
    }

struct SpriteAnimation final : public StateI {
    int start_frame{};
    int frames_num{};
    float frame_time{};
    float time_left{};

    SpriteAnimation() = default;

    SpriteAnimation(int start_frame, int frames_num, float frame_time, float time_left)
        : start_frame(start_frame), frames_num(frames_num), frame_time(frame_time), time_left(time_left) {
    }

    static constexpr auto name = "Shooting State";

    IMPL_STATEC

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragInt("Start frame", &start_frame);
        ImGui::DragInt("Number of frames", &frames_num);
        ImGui::DragFloat("Time of frame", &frame_time);
        ImGui::DragFloat("Time left", &time_left);
    }

    void animate(entt::registry &registry, entt::entity entity, float time_delta) {
        time_left -= time_delta;
        while (time_left <= 0) {
            auto &sprite = registry.get<Sprite>(entity);
            time_left += frame_time;
            uint16_t curr = sprite.sprite_id;
            uint16_t next = (curr - start_frame + 1) % frames_num + start_frame;
            sprite.sprite_id = next;
        }
    }
};

struct ShootingState final : public StateI {
    int shots_left{};
    float time_to_shot{};

    ShootingState() = default;
    ShootingState(int shots_left, float time_to_shot) : shots_left(shots_left), time_to_shot(time_to_shot) {}

    IMPL_STATEC

    static constexpr auto name = "Shooting State";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragInt("Shots Left", &shots_left);
        ImGui::DragFloat("Time to shoot", &time_to_shot);
    }

    void advance(entt::registry &registry, entt::entity entity) {

        // Akcja związana ze stanem
        time_to_shot -= GetFrameTime();
        while (time_to_shot <= 0.f) {
            // emplace<Bullet>(registry, ...) ...
            time_to_shot += 1.f;
            shots_left -= 1;
        }

        // Przejścia
        if (shots_left < 0) {
            move_self_to_state<MovingState>(registry, entity, 10.f, SpriteAnimation{0, 10, 0.05f, 0.05f});
        }
    }
};

struct MovingState : public StateI {
    float time_left{};
    SpriteAnimation animation{};

    MovingState() = default;
    MovingState(float time_left, SpriteAnimation animation) : time_left(time_left), animation(std::move(animation)) {}
    IMPL_STATEC

    static constexpr auto name = "Moving State";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat("Time left", &time_left);
        animation.inspect(registry, entity);
    }

    void advance(entt::registry &registry, entt::entity entity) {

        // Akcja związana ze stanem - Animacja sprite'a
        animation.animate(registry, entity, GetFrameTime());

        // Przejścia
        time_left -= GetFrameTime();
        if (time_left <= 0.f) {
            move_self_to_state<ShootingState>(registry, entity, 5, 10.f);
        }
    }
};

template <typename State> void advanceState(entt::registry &registry) {
    auto view = registry.view<State>();
    for (auto &&[entity, state] : view.each()) {
        state.advance(registry, entity);
    }
}
} // namespace bh
