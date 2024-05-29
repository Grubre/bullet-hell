#include "components/sprite.hpp"
#include <cstdint>
#include <entt.hpp>
#include <utility>
#include <imgui.h>
#include <raylib.h>

namespace bh {

template <typename T>
concept State = requires(T t, entt::registry &registry, entt::entity entity) {
    { t.advance(registry, entity) };
    { t.change_state(registry, entity) };
};

template <typename... States> struct StatelessMachine {
    static void iterate_states(auto &&f) { (f.template operator()<States>(), ...); }

    template<typename To>
    static void move_to_state(entt::registry& registry, entt::entity entity) {
        iterate_states([&]<typename T>() {
            registry.remove<T>(entity);
        });
        registry.emplace<To>(entity);
    }

    static void advance_states(entt::registry &registry) {
		iterate_states([&]<typename T>() {
			const auto view = registry.view<T>();
            for (auto &&[entity, state] : view.each()) {
				state.advance(registry, entity);
			}
		});
	}
    
    static void change_states(entt::registry &registry) {
		iterate_states([&]<typename T>() {
			const auto view = registry.view<T>();
			for (auto &&[entity, state] : view.each()) {
				state.change_state(registry, entity);
			}
		});
	}
};

struct EnemyShootingState;
struct EnemyRunningState;

using EnemyStatelessMachine = StatelessMachine<EnemyShootingState, EnemyRunningState>;

struct EnemyShootingState {
    void advance(entt::registry &registry, entt::entity entity) {
        
    }

    void change_state(entt::registry &registry, entt::entity entity) {
        EnemyStatelessMachine::move_to_state<EnemyRunningState>(registry, entity);
    }
};

template<typename StMach>
struct ShotingState {
    void advance(entt::registry &registry, entt::entity entity) {
        StMach::template move_to_state<EnemyRunningState>(registry, entity);
    }
};


struct EnemyRunningState {
    void advance(entt::registry &registry, entt::entity entity) {}

    void change_state(entt::registry &registry, entt::entity entity) {
        EnemyStatelessMachine::move_to_state<EnemyShootingState>(registry, entity);
    }
};
} // namespace bh
