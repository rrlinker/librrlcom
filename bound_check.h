#pragma once

#include <cstdint>
#include <stdexcept>

namespace rrl {

    inline void verify_size_bounds(uint64_t value) {
        if constexpr (sizeof(size_t) != sizeof(uint64_t)) {
            if (value & 0xFFFFFFFF00000000) {
                throw std::overflow_error("uint64_t value is to large to be stored in size_t");
            }
        }
    }

    inline void verify_pointer_bounds(uint64_t value) {
        if constexpr (sizeof(uintptr_t) != sizeof(uint64_t)) {
            if (value & 0xFFFFFFFF00000000) {
                throw std::overflow_error("uint64_t value is to large to be stored in uintptr_t");
            }
        }
    }

}
