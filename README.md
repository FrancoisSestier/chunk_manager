[![Windows](https://github.com/FrancoisSestier/matrix_layer_stack/actions/workflows/windows.yml/badge.svg)](https://github.com/FrancoisSestier/matrix_layer_stack/actions/workflows/windows.yml) [![Ubuntu](https://github.com/FrancoisSestier/matrix_layer_stack/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/FrancoisSestier/matrix_layer_stack/actions/workflows/ubuntu.yml) [![codecov](https://codecov.io/gh/FrancoisSestier/matrix_layer_stack/branch/master/graph/badge.svg?token=ZPDP1TAO3Z)](https://codecov.io/gh/FrancoisSestier/matrix_layer_stack) [![License: Unlicense](https://img.shields.io/badge/license-Unlicense-blue.svg)](http://unlicense.org/)

### Matrix Layer Stack
stack of same sized matrices with heterogeneous types

## Goal
represent and access a 2d grid of heterogeneous type easily

## Usage Design 

```c++
    
    //Strong typing for integral and floating_point types

    using elevation = s_int16_t<struct elevation_tag>;
    using tile_id = s_int16_t<struct tile_id_tag>;

    //define matrix stack size and data structure

    using chunk = matrix_tuple<256, 256, elevation, tile_id>;

    chunk ck;

 
        // access
        
        auto [elevation_, tile_id_] = ck.get<elevation, tile_id>(10, 30);

        // modify
        
        elevation_ = 50;
        tile_id_ = 10;
    }

```
 
