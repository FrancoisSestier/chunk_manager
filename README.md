[![Windows](https://github.com/FrancoisSestier/chunk_manager/actions/workflows/windows.yml/badge.svg)](https://github.com/FrancoisSestier/chunk_manager/actions/workflows/windows.yml) [![Ubuntu](https://github.com/FrancoisSestier/chunk_manager/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/FrancoisSestier/chunk_manager/actions/workflows/ubuntu.yml) [![codecov](https://codecov.io/gh/FrancoisSestier/chunk_manager/branch/master/graph/badge.svg?token=ZPDP1TAO3Z)](https://codecov.io/gh/FrancoisSestier/chunk_manager) [![License: Unlicense](https://img.shields.io/badge/license-Unlicense-blue.svg)](http://unlicense.org/)

# Chunk Manager
Open source chunk manager granting safe mutlithreaded acess 

## Usage Design
```c++
#include <chunk_manager/chunk_manager.hpp>
#include <mls/strong_types.hpp>

// Define underlying chunk meta types using mls::strong_type or another strong type implementation
using elevation = mls::s_int16_t<struct elevation_tag>;
using tile_id = mls::s_int16_t<struct tile_id_tag>;

// Define chunk width, height, data_types respectively;
using chunk_t = ckm::chunk<10, 20, elevation, tile_id>;

// Define chunk manager width, height, chunk_type respectively;
using chunk_manager_t = ckm::chunk_mananger<10, 10, chunk_t>;

// create the chunk_manager
chunk_manager_t chunk_manager;
{
    // aquire chunk with world_pos or chunk_id
    auto locked_chunk = chunk_manager.aquire(chunk_id_t(5)); 
    // get and modified chunk data
    auto [el, tid] = locked_chunk->get<elevation, tile_id>(22, 181);
    el++;
    tile_id = 51;


    auto locked_chunk2 = chunk_manager.try_aquire(chunk_id_t(5)); // won't work since the chunk is already locked
    assert(!locked_chunk2.has_value()); 

}// lock is released when locked_chunk gets out of scope
```
## Dependency 
[matrix_layer_stack](https://github.com/FrancoisSestier/matrix_layer_stack)

## Thread-safety
aquire() is blocking, and will wait for another thread to release the chunk, before returning, if necessary.
try_aquire() is non blocking, and returns an optional<locked_chunk<chunk_t>>, that is empty if the requested chunk is already aquired.
They both grant thread safety.

## Warnings
it's not safe to call aquire() on the same chunk_id, in the same scope. since it operates on the same underlying mutex;





