#pragma once
#include <atomic>
#include <future>
#include <mls/matrix_tuple.hpp>
#include <mutex>
#include <optional>
#include <semaphore>

namespace ckm {

    template <typename chunk_type>
    class protected_chunk {
       public:
        using chunk_t = chunk_type;

        template <typename underlying_chunk_type>
        class locked_chunk {
           public:
            using underlying_chunk_t = underlying_chunk_type;
            using owner_t = protected_chunk<chunk_t>;
            using dtor_callback_t = void (owner_t::*)(void);
            locked_chunk() = default;

            locked_chunk(underlying_chunk_t* chunk,
                         dtor_callback_t dtor_callback, owner_t* owner)
                : chunk_(chunk), dtor_callback_(dtor_callback), owner_(owner) {}

            ~locked_chunk() {
                if (owner_) {
                    std::invoke(dtor_callback_, owner_);
                }
            }
            locked_chunk(const locked_chunk& other) = delete;
            locked_chunk& operator=(const locked_chunk& otherp) = delete;
            locked_chunk(locked_chunk&& other)
                : chunk_(other.chunk_),
                  dtor_callback_(other.dtor_callback_),
                  owner_(other.owner_) {
                other.chunk_ = nullptr;
                other.dtor_callback = nullptr;
                other.owner = nullptr;
            }
            locked_chunk& operator=(locked_chunk&& other) {
                if (this != &other) {
                    chunk_ = other.chunk_;
                    dtor_callback_ = other.dtor_callback_;
                    owner_ = other.owner_;
                    other.locked_chunk_ = nullptr;
                    other.dtor_callback_ = nullptr;
                    other.owner_ = nullptr;
                }
                return *this;
            }

            bool has_value() { return chunk_ != nullptr; }
            underlying_chunk_t* operator->() { return chunk_; }
            underlying_chunk_t& operator*() { return *chunk_; }

           protected:
            underlying_chunk_t* chunk_{nullptr};
            owner_t* owner_{nullptr};
            dtor_callback_t dtor_callback_{nullptr};
        };

        locked_chunk<const chunk_t> acquire_read();

        locked_chunk<const chunk_t> try_acquire_read();

        locked_chunk<chunk_t> acquire_write();

        locked_chunk<chunk_t> try_acquire_write();

        template <typename... Ts>
        void init(Ts... ts) {
            underlying_chunk_.init(std::forward<Ts>(ts)...);
        }

       private:
        void reader_leave() {
            if (reader_count_.fetch_sub(1) == 1) {
                reader_gate_.clear();
                writer_sem_.release();
            }
        }

        void writer_leave() {
            decrease_writer_count();
            writer_sem_.release();
        }

        void wait_for_writers() {
            auto writer_count = writer_count_.load();
            while (writer_count != 0) {
                writer_count_.wait(writer_count);
                writer_count = writer_count_.load();
            }
        }

        void decrease_writer_count() {
            if (writer_count_.fetch_sub(1) == 1) {
                writer_count_.notify_all();
            }
        }

        void increase_reader_count() {
            if (reader_count_.fetch_add(1) == 0) {
                writer_sem_.acquire();
                while (reader_gate_.test_and_set()) {
                }
                reader_gate_.notify_all();
            }
        }



        auto empty_lock() { return locked_chunk<chunk_t>(); }

       private:
        chunk_t underlying_chunk_;
        std::binary_semaphore writer_sem_{1};
        std::atomic_unsigned_lock_free writer_count_{0};
        std::atomic_unsigned_lock_free reader_count_{0};
        std::atomic_flag reader_gate_;
    };

    template <typename chunk_t>
    protected_chunk<chunk_t>::locked_chunk<const chunk_t>
    protected_chunk<chunk_t>::try_acquire_read() {
        if (writer_count_ != 0) {
            locked_chunk<const chunk_t>();
        }
        if (reader_count_.fetch_add(1) == 0) {
            if (!writer_sem_.try_acquire()) {
                unsigned int expected = 1;
                if (!writer_sem_.compare_exchange_strong(expected, 0)) {
                    std::async(std::launch::async, [=, this]() {
                        writer_sem_.acquire();
                        while (reader_gate_.test_and_set()) {
                        }
                        reader_gate_.notify_all();
                        reader_leave();
                    });
                }
                return locked_chunk<const chunk_t>();
            }
            while (reader_gate_.test_and_set()) {
            }
            reader_gate_.notify_all();
        }
        if (!reader_gate_.test()) {
            if (reader_count_.fetch_sub(1) == 1) {
                reader_gate_.clear();
                writer_sem_.release();
            }
            return locked_chunk<const chunk_t>();
        }
        return locked_chunk<const chunk_t>(
            &underlying_chunk_, &protected_chunk::reader_leave, this);
    }

    template <typename chunk_t>
    protected_chunk<chunk_t>::locked_chunk<const chunk_t>
    protected_chunk<chunk_t>::acquire_read() {
        wait_for_writers();
        increase_reader_count();
        reader_gate_.wait(false);
        return locked_chunk<const chunk_t>(
            &underlying_chunk_, &protected_chunk::reader_leave, this);
    }

    template <typename chunk_t>
    protected_chunk<chunk_t>::locked_chunk<chunk_t>
    protected_chunk<chunk_t>::try_acquire_write() {
        ++writer_count_;
        if (!writer_sem_.try_acquire()) {
            decrease_writer_count();
            return locked_chunk<chunk_t>();
        }
        return locked_chunk<chunk_t>(&underlying_chunk_,
                                     &protected_chunk::writer_leave, this);
    }

    template <typename chunk_t>
    protected_chunk<chunk_t>::locked_chunk<chunk_t>
    protected_chunk<chunk_t>::acquire_write() {
        ++writer_count_;
        writer_sem_.acquire();
        return locked_chunk<chunk_t>(&underlying_chunk_,
                                     &protected_chunk::writer_leave, this);
    }

}  // namespace ckm
