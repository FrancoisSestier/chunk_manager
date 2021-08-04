#pragma once
#include "chunk_manager/map.hpp"

namespace ckm {
    template <typename map>
    struct map_utils_t {
        using chunk = typename map::chunk_t;
        using chunk_matrix = typename map::chunk_matrix_t;

        static chunk_id_t world_pos_to_chunk_id(int x, int y) {
            try {
                bound_check(x, y);
                return static_cast<chunk_id_t>(floor(x / chunk::width)
                                               + floor(y / chunk::height)
                                                     * chunk_matrix::width);
            } catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
                return _invalid_chunk;
            }
        }

        static int2 chunk_id_to_world_pos(chunk_id_t chunk_id) {
            try {
                id_check(chunk_id);
                return int2{(chunk_id % chunk_matrix::width)*chunk::width,
                            static_cast<int>((chunk_id / chunk_matrix::width)*chunk::height)};
            } catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
                return int2{0, 0};
            }
        }

        static bool in_bounds(int x, int y) {
            if (x >= 0 && x < map::width && y >= 0 && y < map::height) {
                return true;
            }
            return false;
        }

        static bool valid_chunk_id(chunk_id_t chunk_id) {
            if (chunk_id >= 0
                && chunk_id < static_cast<chunk_id_t>(chunk_matrix::size)) {
                return true;
            }
            return false;
        }

       private:
        static void bound_check(int x, int y) {
            if (!in_bounds(x, y)) {
                throw(std::runtime_error(fmt::format(
                    "pos out of map bounds : {} ", details::to_string(x, y))));
            }
        }

        static void id_check(chunk_id_t chunk_id) {
            if (!valid_chunk_id(chunk_id)) {
                throw(std::runtime_error(
                    fmt::format("invalid chunk id : {} ", chunk_id)));
            }
        }
    };
}  // namespace ckm