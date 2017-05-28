#ifndef __APULSE_UTIL_H_
#define __APULSE_UTIL_H_

#include <limits>

template< typename to, typename from >
to narrow_cast( from value ) {
    static_assert( std::numeric_limits< to >::max() < std::numeric_limits< from >::max(),
        "narrow_cast used in non-narrowing context" );

    return static_cast< to >( value &
        ( static_cast< from >( std::numeric_limits< to >::max() ) ) );
}

#endif // __APULSE_UTIL_H_
