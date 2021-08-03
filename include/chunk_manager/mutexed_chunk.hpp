#pragma once
#include <mls/matrix_tuple.hpp>
#include <mutex>
#include <optional>

namespace ckm {

    template <typename chunk_type>
    class locked_chunk {
       public:
        locked_chunk(chunk_type& ck, std::unique_lock<std::mutex>&& lock)
            : chunk_(ck), lock_(std::move(lock)) {}

        chunk_type* operator->() { return &chunk_; }
        chunk_type& operator*() { return chunk_; }

       private:
        chunk_type& chunk_;
        std::unique_lock<std::mutex> lock_;
    };

    template <typename chunk_type>
    class mutexed_chunk {
       public:
        using chunk_t = chunk_type;

        template <typename... Ts>
        void init(Ts... ts) {
            underlying_chunk_.init(std::forward<Ts>(ts)...);
        }

        locked_chunk<chunk_t> aquire() {
            std::unique_lock<std::mutex> lock(chunk_mutex_);
            return locked_chunk<chunk_t>{underlying_chunk_, std::move(lock)};
        }

        std::optional<locked_chunk<chunk_t>> try_aquire() {
            std::unique_lock<std::mutex> lock(chunk_mutex_,std::defer_lock);
            if (lock.try_lock()) {
                return std::make_optional<locked_chunk<chunk_t>>(
                    underlying_chunk_, std::move(lock));
            }
            return std::nullopt;
        }

       private:
        mutable std::mutex chunk_mutex_;
        chunk_t underlying_chunk_;
    };

}  // namespace ckm
