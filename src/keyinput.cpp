#include <algorithm>
#include <raylib.h>
#include "keyinput.hpp"

[[nodiscard]] auto bh::KeyManager::make_subscriber(callback_t &&callback) -> Subscriber {
    static subscriber_id_t id = 0;
    return Subscriber{std::move(callback), id++};
}

auto bh::KeyManager::subscribe(raylib_key_t key, callback_t &&callback) -> subscriber_id_t {
    auto subscriber = make_subscriber(std::move(callback));
    const auto id = subscriber.id;
    subscribers[key].push_back(std::move(subscriber));

    return id;
}

void bh::KeyManager::unsubscribe(subscriber_id_t id) {
    for (auto &[key, subs] : subscribers) {
        subs.erase(std::remove_if(subs.begin(), subs.end(), [id](const auto &sub) { return sub.id == id; }),
                   subs.end());
    }
}

void bh::KeyManager::notify(raylib_key_t key) {
    if (!subscribers.contains(key)) {
        return;
    }

    for (const auto &sub : subscribers[key]) {
        sub.callback();
    }
}

void bh::notify_keyboard_press_system(bh::KeyManager &manager) {
    while (const auto key = GetKeyPressed()) {
        manager.notify(key);
    }
}
