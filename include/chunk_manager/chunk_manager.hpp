#pragma once
#include <array>
#include <chunk_manager/chunk.hpp>
#include <chunk_manager/chunk_matrix.hpp>
#include <chunk_manager/map.hpp>
#include <mls/matrix_tuple.hpp>
#include <type_traits>

namespace ckm {

    template <size_t width, size_t height, typename chunk_type>
    class chunk_mananger {
       public:
        using chunk_matrix_t = chunk_matrix<width, height, chunk_type>;
        using chunk_t = chunk_type;
        using storage_type = std::array<chunk_t, chunk_t::size>;
        using map = map_t<chunk_matrix_t>;
        using map_utils = map_utils_t<map>;


        chunk_mananger() { chunk_matrix_.init(); }

        auto aquire(chunk_id_t chunk_id) {
            return chunk_matrix_.aquire(chunk_id);
        }

        auto try_aquire(chunk_id_t chunk_id) {
            return chunk_matrix_.try_aquire(chunk_id);
        }

        auto aquire(int x, int y) {
            return chunk_matrix_.aquire(x,y);
        }

        auto try_aquire(int x, int y) {
            return chunk_matrix_.try_aquire(x,y);
        }

       private:
        chunk_matrix_t chunk_matrix_;
    };

}  // namespace ckm