#pragma once
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

template<typename T>
class Property {
private:
    T value;
	bool useLock = false;
    mutable std::shared_mutex mutex;
    bool isInitialized = false;

public:
    // 預設構造函數
    Property() : value{} {}
    explicit Property(T initialValue) : value(initialValue) {} // 帶初始值的構造函數

    T Get() {
        if(useLock) std::shared_lock<std::shared_mutex> lock(mutex);
        return value;
    }

    void Set(const T& newValue) {
        if (useLock) std::unique_lock<std::shared_mutex> lock(mutex);
        value = newValue;
        isInitialized = true;
    }

    bool IsInitialized() const {
        if (useLock) std::shared_lock<std::shared_mutex> lock(mutex);
        return isInitialized;
    }

    void UnIsInitialized() {
        if (useLock) std::shared_lock<std::shared_mutex> lock(mutex);
        isInitialized = false;
    }
};



template<typename T>
class PropertyVector {
private:
    std::vector<T> value;             
    bool useLock = false;
    mutable std::shared_mutex mutex;    
    bool isInitialized = false;        

public:
    PropertyVector() = default;
    explicit PropertyVector(const std::vector<T>& initialValue) : value(initialValue), isInitialized(true) {}

    std::vector<T> GetAll() const {
        if (useLock) std::shared_lock<std::shared_mutex> lock(mutex);
        return value;
    }

    T Get(size_t index) const {
        if (useLock) std::shared_lock<std::shared_mutex> lock(mutex);
        if (index >= value.size())
            throw std::out_of_range("Index out of range");
        return value[index];
    }

    void SetAll(const std::vector<T>& newValue) {
        if (useLock) std::unique_lock<std::shared_mutex> lock(mutex);
        value = newValue;
        isInitialized = true;
    }

    void Set(size_t index, const T& val) {
        if (useLock) std::unique_lock<std::shared_mutex> lock(mutex);
        if (index >= value.size())
            throw std::out_of_range("Index out of range");
        value[index] = val;
    }

    void Add(const T& val) {
        if (useLock) std::unique_lock<std::shared_mutex> lock(mutex);
        value.push_back(val);
        isInitialized = true;
    }

    void Remove(size_t index) {
        if (useLock) std::unique_lock<std::shared_mutex> lock(mutex);
        if (index >= value.size())
            throw std::out_of_range("Index out of range");
        value.erase(value.begin() + index);
    }

    void Clear() {
        if (useLock) std::unique_lock<std::shared_mutex> lock(mutex);
        value.clear();
    }

    bool Contains(const T& element) const {
        return std::find(value.begin(), value.end(), element) != value.end();
    }

    bool IsInitialized() const {
        return isInitialized;
    }

    bool Empty() const {
        return value.empty();
    }
};


template<typename K, typename V>
class PropertyMap {
private:
    std::map<K, V> value;
    bool useLock = false;
    mutable std::shared_mutex mutex;
    bool isInitialized = false;

public:
    PropertyMap() = default;
    explicit PropertyMap(bool Lock) : useLock(Lock) {}

    std::map<K, V> GetAll() const {
        if (useLock) std::shared_lock<std::shared_mutex> lock(mutex);
        return value;
    }

    V Get(const K& key) const {
        if (useLock) std::shared_lock<std::shared_mutex> lock(mutex);
        auto it = value.find(key);
        if (it == value.end())
            return V();
        return it->second;
    }

    void SetAll(const std::map<K, V>& newValue) {
        if (useLock) std::unique_lock<std::shared_mutex> lock(mutex);
        value = newValue;
        isInitialized = true;
    }

    void Set(const K& key, const V& val) {
        if (useLock) std::unique_lock<std::shared_mutex> lock(mutex);
        value[key] = val;
        isInitialized = true;
    }

    void Remove(const K& key) {
        if (useLock) std::unique_lock<std::shared_mutex> lock(mutex);
        value.erase(key);
    }

    void Clear() {
        if (useLock) std::unique_lock<std::shared_mutex> lock(mutex);
        value.clear();
    }

    bool Contains(const K& key) const {
        return value.find(key) != value.end();
    }

    bool IsInitialized() const {
        return isInitialized;
    }

    void UnIsInitialized() {
        isInitialized = false;
    }

    bool Empty() const {
        return value.empty();
    }

	int Size() const {
		return value.size();
	}
};