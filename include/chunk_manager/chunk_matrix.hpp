#pragma once
#include <chunk_manager/chunk.hpp>
#include <chunk_manager/int2.hpp>
#include <chunk_manager/map.hpp>
#include <chunk_manager/map_utils.hpp>
#include <chunk_manager/mutexed_chunk.hpp>
#include <mls/matrix_tuple.hpp>
#include <type_traits>

namespace ckm {

    template <size_t ckm_width, size_t ckm_height, typename chunk_type>
    struct chunk_matrix {
       public:
        inline static constexpr size_t height = ckm_width;
        inline static constexpr size_t width = ckm_height;
        inline static constexpr size_t size = width * height;
        using chunk_t = typename chunk_type;
        using mutexed_chunk_t = mutexed_chunk<chunk_t>;
        using storage_type = std::array<mutexed_chunk_t, size>;
        using map = typename map_t<chunk_matrix>;
        using map_utils = typename map_utils_t<map>;

        void init() {
            for (chunk_id_t i = 0; i < chunk_storage_.size(); i++) {
                chunk_storage_.at(i).init(i, pos(i));
            }
        }

        auto aquire(int x, int y) { return aquire(index(x, y)); }
        auto try_aquire(int x, int y) { return try_aquire(index(x, y)); }

        auto aquire(chunk_id_t chunk_id) {
            return chunk_storage_.at(chunk_id).aquire();
        }

        auto try_aquire(chunk_id_t chunk_id) {
            return chunk_storage_.at(chunk_id).try_aquire();
        }

       private:
        auto index(int x, int y) {
            return map_utils::world_pos_to_chunk_id(x, y);
        }

        auto pos(chunk_id_t id) { return map_utils::chunk_id_to_world_pos(id); }

        storage_type chunk_storage_;
    };

}  // namespace ckm