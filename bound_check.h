#pragma once

#include <cstdint>
#include <stdexcept>

namespace rrl {

    inline void verify_size_bounds(uint64_t value) {
        if constexpr (sizeof(size_t) != sizeof(uint64_t)) {
            static_assert(sizeof(size_t) == sizeof(uint64_t) || sizeof(size_t) == 4, "unexpected sizeof(size_t), expected 4");
            if (value & 0xFFFFFFFF00000000) {
                throw std::overflow_error("uint64_t value is to large to be stored in size_t");
            }
        }
    }

    inline void verify_pointer_bounds(uint64_t value) {
        if constexpr (sizeof(uintptr_t) != sizeof(uint64_t)) {
            static_assert(sizeof(uintptr_t) == sizeof(uint64_t) || sizeof(uintptr_t) == 4, "unexpected sizeof(uintptr_t), expected 4");
            if (value & 0xFFFFFFFF00000000) {
                throw std::overflow_error("uint64_t value is to large to be stored in uintptr_t");
            }
        }
    }

}
