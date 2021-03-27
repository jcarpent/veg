#ifndef VEG_PRIMITIVES_HPP_A8O6GDV1S
#define VEG_PRIMITIVES_HPP_A8O6GDV1S

#include "veg/type_traits/core.hpp"
#include "veg/internal/prologue.hpp"

#if !VEG_HAS_BUILTIN(__is_integral) || !VEG_HAS_BUILTIN(__is_floating_point)
#include <type_traits>
#endif

namespace veg {
inline namespace VEG_ABI {
namespace concepts {
namespace aux {
VEG_DEF_CONCEPT(typename T, no_wraps_around, (T(-1) <= T(0)));
VEG_DEF_CONCEPT(typename T, wraps_around, (T(-1) > T(0)));
} // namespace aux
VEG_DEF_CONCEPT_FROM_BUILTIN_OR_STD(typename T, integral, T);
VEG_DEF_CONCEPT_FROM_BUILTIN_OR_STD(typename T, floating_point, T);
VEG_DEF_CONCEPT_DISJUNCTION(
		typename T, arithmetic, ((, integral<T>), (, floating_point<T>)));
VEG_DEF_CONCEPT_CONJUNCTION(
		typename T,
		signed_integral,
		((, integral<T>), (aux::, no_wraps_around<T>)));
VEG_DEF_CONCEPT_CONJUNCTION(
		typename T, unsigned_integral, ((, integral<T>), (aux::, wraps_around<T>)));
} // namespace concepts
} // namespace VEG_ABI
} // namespace veg

#include "veg/internal/epilogue.hpp"
#endif /* end of include guard VEG_PRIMITIVES_HPP_A8O6GDV1S */
