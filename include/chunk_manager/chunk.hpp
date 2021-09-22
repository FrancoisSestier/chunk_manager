#pragma once
#include <fmt/format.h>

#include <chunk_manager/int2.hpp>
#include <mls/matrix_tuple.hpp>
#include <mutex>
#include <optional>
#include <stdexcept>

namespace ckm {

    using chunk_id_t = uint32_t;
    inline static constexpr chunk_id_t _invalid_chunk = UINT32_MAX;

    namespace details {

        template <typename T, typename... Ts>
        struct are_distinct
            : std::conjunction<std::negation<std::is_same<T, Ts>>...,
                               are_distinct<Ts...>> {};

        template <typename T>
        struct are_distinct<T> : std::true_type {};

        template <typename... Ts>
        inline static constexpr bool are_distinct_v
            = are_distinct<Ts...>::value;

        template <typename T, typename... Ts>
        struct contains : std::disjunction<std::is_same<T, Ts>...> {};

        template <typename T, typename... Ts>
        inline static constexpr bool contains_v = contains<T, Ts...>::value;
    }  // namespace details

    template <size_t chunk_width, size_t chunk_height, typename... data_types>
    requires(details::are_distinct_v<data_types...>) class chunk {
       public:
        using storage_t
            = mls::matrix_tuple<chunk_width, chunk_height, data_types...>;

        inline static constexpr size_t width = storage_t::width;
        inline static constexpr size_t height = storage_t::height;
        inline static constexpr size_t size = storage_t::size;

        void init(chunk_id_t chunk_id, int2 pos) {
            chunk_id_ = chunk_id;
            pos_ = pos;
        }

        template <typename... Ts>
        requires(std::conjunction_v<details::contains<Ts, data_types...>...>)
            [[nodiscard]] auto get(int x, int y) {
            try {
                in_bounds(x, y);
                return storage_.template get<Ts...>(
                    static_cast<size_t>(x) % width,
                    static_cast<size_t>(y) % height);
            } catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
                return storage_.template get<Ts...>(0, 0);
            }
        }

        template <typename... Ts>
        requires(std::conjunction_v<details::contains<Ts, data_types...>...>)
            [[nodiscard]] auto get(int x, int y) const {
            try {
                in_bounds(x, y);
                return storage_.template get<std::add_const_t<Ts>...>(
                    static_cast<size_t>(x) % width,
                    static_cast<size_t>(y) % height);
            } catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
                return storage_.template get<std::add_const_t<Ts>...>(0, 0);
            }
        }

        chunk_id_t id() const { return chunk_id_; }

        auto to_string() const {
            return fmt::format("\n\tchunk_id : {} \n\t pos : {}", chunk_id_,
                               details::to_string(pos_));
        }

        chunk_id_t chunk_id() const { return chunk_id_; }

        int2 pos() { return pos_; }

        template <typename T>
        requires(std::conjunction_v<details::contains<Ts, data_types...>...>)
            T* raw() {
            return storage_.template raw<T>();
        }

        inline [[nodiscard]] storage_t* operator->() { return &storage_; }

        inline [[nodiscard]] storage_t& operator*() { return storage_; }

       private:
        void in_bounds(int x, int y) const {
            if (x < pos_.x || x >= pos_.x + width || y < pos_.y
                || y >= pos_.y + height) {
                throw std::runtime_error(
                    fmt::format("pos out of chunk bounds : {} \n {}",
                                this->to_string(), details::to_string(x, y)));
            }
        }

        storage_t storage_;
        chunk_id_t chunk_id_;
        int2 pos_;
    };
}  // namespace ckm
