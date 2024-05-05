#pragma once
#include <list>
#include <unordered_map>
#include <mutex>
template <typename T>
class LRUPolicy {
   public:
    T oldest() {
        T res{};
        std::lock_guard<std::mutex> lk(mu_);
        { res = this->list_.back(); }
        return res;
    }

    void promote(T t) {
        std::lock_guard<std::mutex> lk(mu_);
        auto it = this->cache_.find(t);
        if (it != this->cache_.end()) {
            this->list_.erase(this->cache_[t]);
            this->list_.push_front(t);
            this->cache_[t] = this->list_.begin();
        }
    }
    void evict(T t) {
        std::lock_guard<std::mutex> lk(mu_);
        auto it = this->cache_.find(t);
        if (it != this->cache_.end()) {
            this->list_.erase(it->second);
            this->cache_.erase(it);
        }
    }

    void insert(T t) {
        std::lock_guard<std::mutex> lk(mu_);

        auto it = this->cache_.find(t);
        if (it != this->cache_.end()) {  // erase if exist
            this->list_.erase(it->second);
            this->cache_.erase(it);
        }
        // re insert
        this->list_.insert(this->list_.begin(), t);
        this->cache_[t] = this->list_.begin();
    }

    [[nodiscard]] size_t size() const { return this->list_.size(); }

   private:
    std::list<T> list_;
    std::unordered_map<T, typename std::list<T>::iterator> cache_;
    std::mutex mu_;
};
