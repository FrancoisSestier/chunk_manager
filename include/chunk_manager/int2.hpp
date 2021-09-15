#pragma once
#include <fmt/format.h>

namespace ckm {

    struct int2 {
        int x;
        int y;
    };

    namespace details {

        inline static auto to_string(int x, int y) {
            return fmt::format("x : {},  y : {}", x, y);
        }

        inline static auto to_string(const int2& val) { return to_string(val.x, val.y); }

    }  // namespace details


}  // namespace ckm