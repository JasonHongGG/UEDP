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
    // 預設構造函數
    Property() : value{} {}
    explicit Property(T initialValue) : value(initialValue) {} // 帶初始值的構造函數

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



template<typename T>
class PropertyVector {
private:
    std::vector<T> value;               // 內部存儲 vector
    mutable std::shared_mutex mutex;    // 用於線程安全
    bool isInitialized = false;         // 判斷是否初始化

public:
    PropertyVector() = default;
    explicit PropertyVector(const std::vector<T>& initialValue) : value(initialValue), isInitialized(true) {}

    std::vector<T> GetAll() const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        if (!isInitialized)
            throw std::runtime_error("PropertyVector not initialized");
        return value;
    }

    T Get(size_t index) const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        if (!isInitialized)
            throw std::runtime_error("PropertyVector not initialized");
        if (index >= value.size())
            throw std::out_of_range("Index out of range");
        return value[index];
    }

    void SetAll(const std::vector<T>& newValue) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        value = newValue;
        isInitialized = true;
    }

    void Set(size_t index, const T& val) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        if (!isInitialized)
            throw std::runtime_error("PropertyVector not initialized");
        if (index >= value.size())
            throw std::out_of_range("Index out of range");
        value[index] = val;
    }

    void Add(const T& val) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        value.push_back(val);
        isInitialized = true;
    }

    void Remove(size_t index) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        if (!isInitialized)
            throw std::runtime_error("PropertyVector not initialized");
        if (index >= value.size())
            throw std::out_of_range("Index out of range");
        value.erase(value.begin() + index);
    }

    void Clear() {
        std::unique_lock<std::shared_mutex> lock(mutex);
        value.clear();
    }

    bool Contains(const T& element) const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return std::find(value.begin(), value.end(), element) != value.end();
    }

    bool IsInitialized() const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return isInitialized;
    }

    bool Empty() const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return value.empty();
    }
};


template<typename K, typename V>
class PropertyMap {
private:
    std::map<K, V> value;
    mutable std::shared_mutex mutex;
    bool isInitialized = false;

public:
    PropertyMap() = default;
    explicit PropertyMap(const std::map<K, V>& initialValue) : value(initialValue) {}

    std::map<K, V> GetAll() const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        if (!isInitialized)
            throw std::runtime_error("PropertyMap not initialized");
        return value;
    }

    V Get(const K& key) const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        if (!isInitialized)
            throw std::runtime_error("PropertyMap not initialized");
        auto it = value.find(key);
        if (it == value.end())
            throw std::out_of_range("Key not found");
        return it->second;
    }

    void SetAll(const std::map<K, V>& newValue) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        value = newValue;
        isInitialized = true;
    }

    void Set(const K& key, const V& val) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        value[key] = val;
        isInitialized = true;
    }

    void Remove(const K& key) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        value.erase(key);
    }

    void Clear() {
        std::unique_lock<std::shared_mutex> lock(mutex);
        value.clear();
    }

    bool Contains(const K& key) const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return value.find(key) != value.end();
    }

    bool IsInitialized() const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return isInitialized;
    }

    bool Empty() const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return value.empty();
    }
};