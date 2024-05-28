#include "components/sprite.hpp"
#include <cstdint>
#include <entt.hpp>
#include <imgui.h>
#include <raylib.h>

namespace bh {

struct ShootingState;
struct MovingState;

template<typename From, typename To, typename ...Args>
void move_to_state(entt::registry& registry, entt::entity entity, const Args &...args) {
    registry.remove<From>(entity);
    registry.emplace<To>(entity, args...);
}

struct SpriteAnimation {
    int start_frame;
    int frames_num;
    float frame_time;
    float time_left;

    static constexpr auto name = "Shooting State";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragInt("Start frame", &start_frame);
        ImGui::DragInt("Number of frames", &frames_num);
        ImGui::DragFloat("Time of frame", &frame_time);
        ImGui::DragFloat("Time left", &time_left);
    }

    void animate(entt::registry& registry, entt::entity entity, float time_delta) {
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

struct ShootingState { 
    int shots_left; 
    float time_to_shot;

    static constexpr auto name = "Shooting State";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragInt("Shots Left", &shots_left);
        ImGui::DragFloat("Time to shoot", &time_to_shot);
    }

    void advance(entt::registry& registry, entt::entity entity) {
        
        // Akcja związana ze stanem
        time_to_shot -= GetFrameTime();
        while (time_to_shot <= 0.f) {
            // emplace<Bullet>(registry, ...) ...
            time_to_shot += 1.f;
            shots_left -= 1;
        }

        // Przejścia
        if (shots_left < 0) {
            move_to_state<ShootingState, MovingState>(registry, entity, 10.f, SpriteAnimation { 0, 10, 0.05f, 0.05f });
        }
    }
};

struct MovingState { 
    float time_left; 
    SpriteAnimation animation;

    static constexpr auto name = "Moving State";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat("Time left", &time_left);
        animation.inspect(registry, entity);
    }

    void advance(entt::registry& registry, entt::entity entity) {

        // Akcja związana ze stanem - Animacja sprite'a
        animation.animate(registry, entity, GetFrameTime());

        // Przejścia
        time_left -= GetFrameTime();
        if (time_left <= 0.f) {
            move_to_state<MovingState, ShootingState>(registry, entity, 5, 10.f);   
        }
    }
};

template<typename State>
void advanceState(entt::registry &registry) {
    auto view = registry.view<State>();
    for (auto &&[entity, state] : view.each()) {
        state.advance(registry, entity);
    }
}

}