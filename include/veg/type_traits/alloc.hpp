#ifndef VEG_ALLOC_HPP_QHYOV5XDS
#define VEG_ALLOC_HPP_QHYOV5XDS

#include "veg/ref.hpp"
#include "veg/type_traits/constructible.hpp"
#include "veg/type_traits/assignable.hpp"
#include "veg/internal/typedefs.hpp"
#include "veg/internal/macros.hpp"
#include "veg/memory/placement.hpp"
#include "veg/internal/prologue.hpp"

namespace veg {
namespace mem {
template <typename T>
struct Alloc {};
template <typename T>
struct Cloner {};
} // namespace mem
namespace _detail {

#if defined(__clang__)
#if __has_feature(address_sanitizer)
#define VEG_HAS_ASAN 1
#else
#define VEG_HAS_ASAN 0
#endif

#elif defined(__SANITIZE_ADDRESS__) && __SANITIZE_ADDRESS__ == 1
#define VEG_HAS_ASAN 1
#else
#define VEG_HAS_ASAN 0
#endif

#if VEG_HAS_ASAN
extern "C" void __sanitizer_annotate_contiguous_container /* NOLINT */ (
		const void* begin,
		const void* end,
		const void* old_mid,
		const void* new_mid);
#endif

[[noreturn]] HEDLEY_NEVER_INLINE void throw_bad_alloc();
[[noreturn]] HEDLEY_NEVER_INLINE void terminate() noexcept;
namespace _mem {
struct DeferUnreachable /* NOLINT */ {
	bool is_unreachable;
	VEG_INLINE ~DeferUnreachable() {
		if (is_unreachable) {
			HEDLEY_UNREACHABLE();
		}
	}
};
} // namespace _mem
} // namespace _detail

namespace mem {
auto memmove(void* dest, void const* src, usize nbytes) noexcept -> void*;

using byte = unsigned char;
struct AllocBlock {
	void* data;
	usize byte_cap;
};
struct Layout {
	usize byte_size;
	usize align;
};
struct RelocFn {
	void* (*fn)(void*, void const*, usize);

	VEG_INLINE void operator()(void* dst, void* src, usize n) const noexcept {
		_detail::_mem::DeferUnreachable _{true};
		(*fn)(dst, src, n);
		_.is_unreachable = false;
	}
	VEG_INLINE auto is_trivial() const noexcept -> bool {
		return fn == &mem::memmove;
	}
};
} // namespace mem
namespace concepts {
namespace alloc {
VEG_CONCEPT_EXPR(
		(typename A),
		(A),
		dealloc,
		mem::Alloc<A>::dealloc( //
				VEG_DECLVAL(RefMut<A>),
				VEG_DECLVAL(void*),
				VEG_DECLVAL(mem::Layout)),
		true);

VEG_CONCEPT_EXPR(
		(typename A),
		(A),
		alloc,
		mem::Alloc<A>::alloc(VEG_DECLVAL(RefMut<A>), VEG_DECLVAL(mem::Layout)),
		VEG_CONCEPT(same<ExprType, mem::AllocBlock>));

VEG_CONCEPT_EXPR(
		(typename A),
		(A),
		owns_alloc,
		mem::Alloc<A>::owns(VEG_DECLVAL(Ref<A>), VEG_DECLVAL(mem::Layout)),
		VEG_CONCEPT(same<ExprType, bool>));

VEG_CONCEPT_EXPR(
		(typename A),
		(A),
		grow,
		mem::Alloc<A>::grow(
				VEG_DECLVAL(RefMut<A>),
				VEG_DECLVAL(void*),
				VEG_DECLVAL(mem::Layout),
				VEG_DECLVAL(usize),
				VEG_DECLVAL(mem::RelocFn)),
		VEG_CONCEPT(same<ExprType, mem::AllocBlock>));

VEG_CONCEPT_EXPR(
		(typename A),
		(A),
		shrink,
		mem::Alloc<A>::shrink(
				VEG_DECLVAL(RefMut<A>),
				VEG_DECLVAL(void*),
				VEG_DECLVAL(mem::Layout),
				VEG_DECLVAL(usize),
				VEG_DECLVAL(mem::RelocFn)),
		VEG_CONCEPT(same<ExprType, mem::AllocBlock>));

VEG_CONCEPT_EXPR(
		(typename C, typename T, typename A),
		(C, T, A),
		destroy,
		mem::Cloner<C>::destroy( //
				VEG_DECLVAL(RefMut<C>),
				VEG_DECLVAL(T*),
				VEG_DECLVAL(RefMut<A>)),
		true);

VEG_CONCEPT_EXPR(
		(typename C, typename T, typename A),
		(C, T, A),
		clone,
		mem::Cloner<C>::clone( //
				VEG_DECLVAL(RefMut<C>),
				VEG_DECLVAL(Ref<T>),
				VEG_DECLVAL(RefMut<A>)),
		VEG_CONCEPT(same<ExprType, T>));

VEG_CONCEPT_EXPR(
		(typename C, typename T, typename A),
		(C, T, A),
		clone_from,
		mem::Cloner<C>::clone_from(
				VEG_DECLVAL(RefMut<C>),
				VEG_DECLVAL(RefMut<T>),
				VEG_DECLVAL(Ref<T>),
				VEG_DECLVAL(RefMut<A>)),
		true);
} // namespace alloc
} // namespace concepts
} // namespace veg

#include "veg/internal/epilogue.hpp"
#endif /* end of include guard VEG_ALLOC_HPP_QHYOV5XDS */
