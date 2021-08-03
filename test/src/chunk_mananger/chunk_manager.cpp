#pragma once
#include <gtest/gtest.h>

#include <chunk_manager/chunk_manager.hpp>
#include <mls/strong_types.hpp>

using elevation = mls::s_int16_t<struct elevation_tag>;
using tile_id = mls::s_int16_t<struct tile_id_tag>;

using chunk_t = ckm::chunk<10, 20, elevation, tile_id>;

using chunk_manager_t = ckm::chunk_mananger<10, 10, chunk_t>;
using map_utils = chunk_manager_t::map_utils;

TEST(map_utils, chunk_id) {
    ckm::int2 pos{59, 125};
    ckm::chunk_id_t chunk_id = map_utils::world_pos_to_chunk_id(pos.x, pos.y);
    ckm::int2 chunk_pos = map_utils::chunk_id_to_world_pos(chunk_id);
    ASSERT_EQ(chunk_pos.x, 50);
    ASSERT_EQ(chunk_pos.y, 120);
}

TEST(chunk_mananger, ctor) {
    chunk_manager_t chunk_manager;
    ASSERT_EQ(chunk_manager_t::map::width, 100);
    ASSERT_EQ(chunk_manager_t::map::height, 200);
}

TEST(chunk_mananger, aquire) {
    ckm::int2 pos{24, 182};

    chunk_manager_t chunk_manager;
    {
        auto locked_chunk = chunk_manager.aquire(pos.x, pos.y);

        ASSERT_EQ(locked_chunk->pos().x, 20);
        ASSERT_EQ(locked_chunk->pos().y, 180);
        
            auto [el, tid] = locked_chunk->get<elevation, tile_id>(22, 181);
            //el = 10;
           // tid = 22;
        
        auto locked_chunk2 = chunk_manager.try_aquire(pos.x, pos.y);
        ASSERT_FALSE(locked_chunk2.has_value());
    }
    auto locked_chunk_opt = chunk_manager.try_aquire(pos.x, pos.y);
    ASSERT_TRUE(locked_chunk_opt.has_value());
    auto& locked_chunk = locked_chunk_opt.value();
    ASSERT_EQ(locked_chunk->pos().x, 20);
    ASSERT_EQ(locked_chunk->pos().y, 180);
    //auto [el, tid] = locked_chunk->get<elevation, tile_id>(22, 181);
    //ASSERT_EQ(el, 10);
    //ASSERT_EQ(tid, 22);
}