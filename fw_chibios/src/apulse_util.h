#ifndef __APULSE_UTIL_H_
#define __APULSE_UTIL_H_

#include <limits>

#define MASK_ASSIGN(mask, value) (((value & 1) ? mask##_0 : 0) | \
                                  ((value & 2) ? mask##_0 << 1 : 0) | \
                                  ((value & 4) ? mask##_0 << 2 : 0) | \
                                  ((value & 8) ? mask##_0 << 3 : 0) | \
                                  ((value & 16) ? mask##_0 << 4 : 0) | \
                                  ((value & 32) ? mask##_0 << 5 : 0) | \
                                  ((value & 64) ? mask##_0 << 6 : 0) | \
                                  ((value & 128) ? mask##_0 << 7 : 0) | \
                                  ((value & 256) ? mask##_0 << 8 : 0) | \
                                  ((value & 512) ? mask##_0 << 9 : 0) | \
                                  ((value & 1024) ? mask##_0 << 10 : 0) | \
                                  ((value & 2048) ? mask##_0 << 11 : 0) | \
                                  ((value & 4096) ? mask##_0 << 12 : 0) | \
                                  ((value & 8192) ? mask##_0 << 13 : 0) | \
                                  ((value & 16384) ? mask##_0 << 14 : 0) | \
                                  ((value & 32768) ? mask##_0 << 15 : 0))

constexpr uint32_t count_low_zeros(uint32_t i){
    return (i & 1) ? 0 : count_low_zeros(i >> 1) + 1;
}

constexpr uint32_t rec_count_ones(uint64_t i){
    return (i == 1) ? 0 : (i & 1) + rec_count_ones(i >> 1);
}

constexpr uint32_t count_ones(uint32_t i){
    return rec_count_ones(static_cast<uint64_t>(i) | 0x100000000l);
}

constexpr uint32_t rv(uint32_t mask, uint32_t value){
    return (value & (1 << count_ones(mask))) << count_low_zeros(mask);
}

template< typename to, typename from >
to narrow_cast( from value ) {
    static_assert( std::numeric_limits< to >::max() < std::numeric_limits< from >::max(),
        "narrow_cast used in non-narrowing context" );

    return static_cast< to >( value &
        ( static_cast< from >( std::numeric_limits< to >::max() ) ) );
}

#endif // __APULSE_UTIL_H_
