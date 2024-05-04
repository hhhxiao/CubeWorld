#pragma once
#include <list>
#include <unordered_map>

template <typename T>
class LRUPolicy {
   public:
    T oldest() { return this->list_.back(); }

    void promote(T t) {
        auto it = this->cache_.find(t);
        if (it == this->cache_.end()) {
            // Assert(false, "Can not promote an non-exist cache");
        } else {
            this->list_.erase(this->cache_[t]);
            this->list_.push_front(t);
            this->cache_[t] = this->list_.begin();
        }
    }
    void evict(T t) {
        auto it = this->cache_.find(t);
        if (it != this->cache_.end()) {
            this->list_.erase(it->second);
            this->cache_.erase(it);
        }
    }

    void insert(T t) {
        auto it = this->cache_.find(t);
        if (it == this->cache_.end()) {
            this->list_.push_back(t);
            this->cache_[t] = std::prev(this->list_.end());
        }
        promote(t);
    }

    [[nodiscard]] size_t size() const { return this->list_.size(); }

   private:
    std::list<T> list_;
    std::unordered_map<T, typename std::list<T>::iterator> cache_;
};
