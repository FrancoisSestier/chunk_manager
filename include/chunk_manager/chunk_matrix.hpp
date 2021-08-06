#pragma once
#include <chunk_manager/chunk.hpp>
#include <chunk_manager/int2.hpp>
#include <chunk_manager/map.hpp>
#include <chunk_manager/map_utils.hpp>
#include <chunk_manager/mutexed_chunk.hpp>
#include <mls/matrix_tuple.hpp>
#include <type_traits>

namespace ckm {

    using acess_permission = uint8_t;

    inline static constexpr acess_permission read = 0;
    inline static constexpr acess_permission write = 1;

    template <size_t ckm_width, size_t ckm_height, typename chunk_type>
    struct chunk_matrix {
       public:
        inline static constexpr size_t height = ckm_width;
        inline static constexpr size_t width = ckm_height;
        inline static constexpr size_t size = width * height;
        using chunk_t = chunk_type;
        using mutexed_chunk_t = protected_chunk<chunk_t>;
        using storage_type = std::array<mutexed_chunk_t, size>;
        using map = map_t<chunk_matrix>;
        using map_utils = map_utils_t<map>;

        void init() {
            for (chunk_id_t i = 0; i < chunk_storage_.size(); i++) {
                chunk_storage_.at(i).init(i, pos(i));
            }
        }
        template <acess_permission access_level>
        auto acquire(int x, int y) {
            return acquire<access_level>(index(x, y));
        }

        template <acess_permission access_level>
        auto try_acquire(int x, int y) {
            return try_acquire<access_level>(index(x, y));
        }

        template <acess_permission access_level>
        auto acquire(chunk_id_t chunk_id) {
            if constexpr (access_level == read) {
                return chunk_storage_.at(chunk_id).acquire_read();
            } else if constexpr (access_level == write) {
                return chunk_storage_.at(chunk_id).acquire_write();
            }
        }

        template <acess_permission access_level>
        auto try_acquire(chunk_id_t chunk_id) {
            if constexpr (access_level == read) {
                return chunk_storage_.at(chunk_id).try_acquire_read();
            } else if constexpr (access_level == write) {
                return chunk_storage_.at(chunk_id).try_acquire_write();
            }
        }

       private:
        auto index(int x, int y) {
            return map_utils::world_pos_to_chunk_id(x, y);
        }

        auto pos(chunk_id_t id) { return map_utils::chunk_id_to_world_pos(id); }

        storage_type chunk_storage_;
    };

}  // namespace ckm