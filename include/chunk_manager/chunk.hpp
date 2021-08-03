#pragma once
#include <chunk_manager/int2.hpp>
#include <format>
#include <mls/matrix_tuple.hpp>
#include <mutex>
#include <optional>
#include <stdexcept>

namespace ckm {

    using chunk_id_t = uint32_t;
    inline static constexpr chunk_id_t _invalid_chunk = UINT32_MAX;

    template <size_t width, size_t height, typename... Ts>
    class chunk {
       public:
        using storage_t = typename mls::matrix_tuple<width, height, Ts...>;

        inline static constexpr size_t width = storage_t::width;
        inline static constexpr size_t height = storage_t::height;
        inline static constexpr size_t size = storage_t::size;

        void init(chunk_id_t chunk_id, int2 pos) {
            chunk_id_ = chunk_id;
            pos_ = pos;
        }

        template <typename... Ts>
        auto get(int x, int y) {
            try {
                in_bounds(x, y);
                return storage_.get<Ts...>(static_cast<size_t>(x) % width,
                                           static_cast<size_t>(y) % height);
            } catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
                return storage_.get<Ts...>(0, 0);
            }
        }

        chunk_id_t id() { return chunk_id_; }

        auto to_string() {
            return std::format("\n\tchunk_id : {} \n\t pos : {}", chunk_id_,
                               details::to_string(pos_));
        }

        chunk_id_t chunk_id() { return chunk_id_; }

        int2 pos() { return pos_; }

       private:
        void in_bounds(int x, int y) {
            if (x < pos_.x || x >= pos_.x + width || y < pos_.y
                || y >= pos_.y + height) {
                throw std::runtime_error(
                    std::format("pos out of chunk bounds : {} \n {}",
                                this->to_string(), details::to_string(x, y)));
            }
        }

        storage_t storage_;
        chunk_id_t chunk_id_;
        int2 pos_;
    };
}  // namespace ckm
