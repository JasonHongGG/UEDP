#pragma once
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

template<typename T>
class Property {
private:
    T value;
    mutable std::shared_mutex mutex;
    bool isInitialized = false;

public:
    T Get() const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        if (!isInitialized)
            throw std::runtime_error("Property not initialized");
        return value;
    }

    void Set(const T& newValue) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        value = newValue;
        isInitialized = true;
    }

    bool IsInitialized() const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return isInitialized;
    }
};