#pragma once
#include <chunk_manager/chunk.hpp>
#include <chunk_manager/int2.hpp>
#include <format>
#include <stdexcept>

namespace ckm {

    template <typename chunk_matrix>
    struct map_t {
        using chunk_t = typename chunk_matrix::chunk_t;
        using chunk_matrix_t = typename chunk_matrix;
        inline static constexpr size_t width
            = chunk_matrix_t::width * chunk_t::width;
        inline static constexpr size_t height
            = chunk_matrix_t::width * chunk_t::height;
        inline static constexpr size_t size = width * height;
    };

  
}  // namespace ckm