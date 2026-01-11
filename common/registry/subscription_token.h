#pragma once

#include <functional>
#include <utility>
#include <memory>

// RAII-style subscription token: unsubscribe on destruction.
// Move-only, copy disabled.
class SubscriptionToken {
public:
    SubscriptionToken() = default;

    explicit SubscriptionToken(std::function<void()> onDestroy)
        : _onDestroy(std::move(onDestroy))
    {}

    // Move semantics
    SubscriptionToken(SubscriptionToken&& other) noexcept
        : _onDestroy(std::move(other._onDestroy))
    {
        other._onDestroy = nullptr;
    }

    SubscriptionToken& operator=(SubscriptionToken&& other) noexcept {
        if (this != &other) {
            reset();
            _onDestroy = std::move(other._onDestroy);
            other._onDestroy = nullptr;
        }
        return *this;
    }

    // No copy
    SubscriptionToken(const SubscriptionToken&) = delete;
    SubscriptionToken& operator=(const SubscriptionToken&) = delete;

    ~SubscriptionToken() { reset(); }

    void reset() {
        if (_onDestroy) {
            try { _onDestroy(); } catch (...) { /* swallow */ }
            _onDestroy = nullptr;
        }
    }

    explicit operator bool() const noexcept { return static_cast<bool>(_onDestroy); }

private:
    std::function<void()> _onDestroy;
};
