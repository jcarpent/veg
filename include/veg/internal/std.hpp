#ifndef VEG_STD_HPP_NS3C5ON6S
#define VEG_STD_HPP_NS3C5ON6S

#ifdef __has_include
#define __VEG_HAS_INCLUDE(x) __has_include(x)
#else
#define __VEG_HAS_INCLUDE(x) 0
#endif

#include <cstdint>          // i64, u64
#include <initializer_list> // std::initializer_list
#include <new>              // placement new, launder
#include <utility>          // std::tuple_{size,element}

// construct_at, addressof
#if __VEG_HAS_INCLUDE(<bits/stl_construct.h>) &&                                              \
		__VEG_HAS_INCLUDE(<bits/move.h>) &&                                                   \
				__VEG_HAS_INCLUDE(<bits/stl_iterator_base_types.h>) &&                            \
						__VEG_HAS_INCLUDE(<bits/stl_iterator_base_funcs.h>)
#include <bits/stl_iterator_base_types.h>
#include <bits/stl_iterator_base_funcs.h>
#include <bits/stl_construct.h>
#include <bits/move.h>
#else
#include <memory>
#endif

#endif /* end of include guard VEG_STD_HPP_NS3C5ON6S */
