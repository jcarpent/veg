#ifndef __VEG_STORAGE_HPP_X0B4XDKES
#define __VEG_STORAGE_HPP_X0B4XDKES

#include "veg/internal/type_traits.hpp"
#include "veg/internal/memory.hpp"
#include "veg/assert.hpp"

namespace veg {
namespace internal {
namespace storage {

struct delete_copy {
  delete_copy() = default;
  ~delete_copy() = default;
  delete_copy(delete_copy const&) = delete;
  delete_copy(delete_copy&&) noexcept = default;
  auto operator=(delete_copy const&) -> delete_copy& = delete;
  auto operator=(delete_copy&&) noexcept -> delete_copy& = default;
};

template <typename T, typename Arg>
struct arg_ctor_to_fn {
  Arg&& arg;
  HEDLEY_ALWAYS_INLINE constexpr auto operator()() const
      noexcept(meta::nothrow_constructible<T, Arg&&>::value) -> T {
    return T{VEG_FWD(arg)};
  }
};
template <typename T>
using copy_ctor_fn = arg_ctor_to_fn<T, T const&>;
template <typename T>
using move_ctor_fn = arg_ctor_to_fn<T, T&&>;

struct hidden_tag1 {};
struct hidden_tag2 {};

template <typename T, bool Rebind = true>
struct storage {
  T inner_val;

  storage() = default;

  template <typename Fn, typename... Args>
  HEDLEY_ALWAYS_INLINE constexpr storage(
      hidden_tag1 /*tag*/,
      Fn&& fn,
      Args&&... args) noexcept(meta::nothrow_invocable<Fn&&, Args&&...>::value)
      : inner_val(invoke(VEG_FWD(fn), VEG_FWD(args)...)) {}

  template <typename U>
  HEDLEY_ALWAYS_INLINE explicit constexpr storage(
      hidden_tag2 /*tag*/,
      U&& arg) noexcept(meta::nothrow_constructible<T, U&&>::value)
      : inner_val(static_cast<T>(VEG_FWD(arg))) {}

  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto _get() const noexcept
      -> T const& {
    return inner_val;
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mut() noexcept -> T& {
    return inner_val;
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mov_ref() && noexcept
      -> T&& {
    return static_cast<T&&>(inner_val);
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mov() && noexcept -> T {
    return static_cast<T&&>(inner_val);
  }
};

template <typename T>
struct storage_reference_base {
  T&& inner_ref = nullptr;

  HEDLEY_ALWAYS_INLINE explicit constexpr storage_reference_base(
      hidden_tag2 /*tag*/, T&& arg) noexcept
      : inner_ref{VEG_FWD(arg)} {}
  template <typename Fn, typename... Args>
  HEDLEY_ALWAYS_INLINE constexpr storage_reference_base(
      hidden_tag1 /*tag*/,
      Fn&& fn,
      Args&&... args) noexcept(meta::nothrow_invocable<Fn&&, Args&&...>::value)
      : inner_ref(invoke(VEG_FWD(fn), VEG_FWD(args)...)) {}

  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto _get() const noexcept -> T& {
    return inner_ref;
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mut() noexcept -> T& {
    return inner_ref;
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mov_ref() && noexcept
      -> T&& {
    return VEG_FWD(inner_ref);
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mov() && noexcept
      -> T&& {
    return VEG_FWD(inner_ref);
  }
};

template <typename T>
struct storage<T&, false> : storage_reference_base<T&> {
  using storage_reference_base<T&>::storage_reference_base;
};

template <typename T>
struct storage<T&&, false> : storage_reference_base<T&&> {
  using storage_reference_base<T&&>::storage_reference_base;
};

template <typename T>
HEDLEY_ALWAYS_INLINE auto as_lvalue(T&& arg) noexcept -> T& {
  return arg;
}
struct null_key {};

template <typename T>
struct storage<T&, true> {
  T* inner_ptr = nullptr;

  HEDLEY_ALWAYS_INLINE explicit constexpr storage(
      hidden_tag2 /*tag*/, T& arg) noexcept
      : inner_ptr{mem::addressof(arg)} {}
  template <typename Fn, typename... Args>
  HEDLEY_ALWAYS_INLINE constexpr storage(
      hidden_tag1 /*tag*/,
      Fn&& fn,
      Args&&... args) noexcept(meta::nothrow_invocable<Fn&&, Args&&...>::value)
      : inner_ptr(mem::addressof(invoke(VEG_FWD(fn), VEG_FWD(args)...))) {}

  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto _get() const noexcept -> T& {
    return *inner_ptr;
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mut() noexcept -> T& {
    return *inner_ptr;
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mov_ref() && noexcept
      -> T& {
    return *inner_ptr;
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mov() && noexcept -> T& {
    return *inner_ptr;
  }

private:
  HEDLEY_ALWAYS_INLINE explicit constexpr storage(
      null_key /*unused*/) noexcept {}
  HEDLEY_ALWAYS_INLINE static constexpr auto null() noexcept -> storage {
    return storage{null_key{}};
  }
  template <typename U>
  friend struct meta::value_sentinel_for;
};

template <typename T>
struct storage<T&&, true> : delete_copy {
  T* inner_ptr = nullptr;

  storage() = default;
  HEDLEY_ALWAYS_INLINE explicit constexpr storage(
      hidden_tag2 /*tag*/, T&& arg) noexcept
      : inner_ptr{mem::addressof(arg)} {}
  template <typename Fn, typename... Args>
  HEDLEY_ALWAYS_INLINE constexpr storage(
      hidden_tag1 /*tag*/,
      Fn&& fn,
      Args&&... args) noexcept(meta::nothrow_invocable<Fn&&, Args&&...>::value)
      : inner_ptr(mem::addressof(internal::storage::as_lvalue(
            invoke(VEG_FWD(fn), VEG_FWD(args)...)))) {}

  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto _get() const noexcept -> T& {
    return *inner_ptr;
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mut() noexcept -> T& {
    return *inner_ptr;
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mov_ref() && noexcept
      -> T&& {
    return static_cast<T&&>(*inner_ptr);
  }
  HEDLEY_ALWAYS_INLINE __VEG_CPP14(constexpr) auto get_mov() && noexcept
      -> T&& {
    return static_cast<T&&>(*inner_ptr);
  }

private:
  HEDLEY_ALWAYS_INLINE explicit constexpr storage(
      null_key /*unused*/) noexcept {}
  HEDLEY_ALWAYS_INLINE static constexpr auto null() noexcept -> storage {
    return storage{null_key{}};
  }
  template <typename U>
  friend struct meta::value_sentinel_for;
};

// intentionally unsupported. please don't
template <typename T>
struct storage<T const&&, true>;
template <typename T>
struct storage<T const&&, false>;

template <meta::category_e C>
struct get_inner;

template <>
struct get_inner<meta::category_e::own> {
  template <typename T, bool Rebind>
  HEDLEY_ALWAYS_INLINE static constexpr auto apply(storage<T, Rebind>&& arg)
      __VEG_DEDUCE_RET(static_cast<storage<T, Rebind>&&>(arg).get_mov());

  template <typename Idx, template <Idx, typename> class Indexed>
  struct with_idx {
    template <Idx I, typename T>
    HEDLEY_ALWAYS_INLINE static constexpr auto
    get_type(Indexed<I, T> const& arg) noexcept -> T;
    template <Idx I, typename T>
    HEDLEY_ALWAYS_INLINE static constexpr auto apply(Indexed<I, T>&& arg)
        __VEG_DEDUCE_RET(static_cast<storage<T, false>&&>(arg).get_mov());
  };
};

template <>
struct get_inner<meta::category_e::ref> {
  template <typename T, bool Rebind>
  HEDLEY_ALWAYS_INLINE static constexpr auto
  apply(storage<T, Rebind> const& arg) noexcept -> T const& {
    return arg._get();
  }

  template <typename Idx, template <Idx, typename> class Indexed>
  struct with_idx {
    template <Idx I, typename T>
    HEDLEY_ALWAYS_INLINE static constexpr auto
    apply(Indexed<I, T> const& arg) noexcept -> T const& {
      return arg._get();
    }

    template <Idx I, typename T>
    HEDLEY_ALWAYS_INLINE static constexpr auto
    impl(Indexed<I, T> const& arg) noexcept -> T const& {
      return arg;
    }
  };
};
template <>
struct get_inner<meta::category_e::ref_mut> {
  template <typename T, bool Rebind>
  HEDLEY_ALWAYS_INLINE static constexpr auto
  apply(storage<T, Rebind>& arg) noexcept -> T& {
    return arg.get_mut();
  }

  template <typename Idx, template <Idx, typename> class Indexed>
  struct with_idx {
    template <Idx I, typename T>
    HEDLEY_ALWAYS_INLINE static constexpr auto
    apply(Indexed<I, T>& arg) noexcept -> T& {
      return arg.get_mut();
    }

    template <Idx I, typename T>
    HEDLEY_ALWAYS_INLINE static constexpr auto impl(Indexed<I, T>& arg) noexcept
        -> Indexed<I, T>& {
      return arg;
    }
  };
};
template <>
struct get_inner<meta::category_e::ref_mov> {
  template <typename T, bool Rebind>
  static constexpr auto apply(storage<T, Rebind>&& arg) noexcept -> T&& {
    return static_cast<storage<T, Rebind>&&>(arg).get_mov_ref();
  }

  template <typename Idx, template <Idx, typename> class Indexed>
  struct with_idx {
    template <Idx I, typename T>
    HEDLEY_ALWAYS_INLINE static constexpr auto
    apply(Indexed<I, T>&& arg) noexcept -> T&& {
      return static_cast<storage<T, false>&&>(arg).get_mov_ref();
    }
  };
};

} // namespace storage
} // namespace internal

template <typename T>
struct meta::value_sentinel_for<veg::internal::storage::storage<T&>>
    : std::integral_constant<i64, 1> {
  HEDLEY_ALWAYS_INLINE static constexpr auto invalid(i64 i) noexcept
      -> ::veg::internal::storage::storage<T&> {
    return VEG_ASSERT(i == 0), ::veg::internal::storage::storage<T&>::null();
  }
  HEDLEY_ALWAYS_INLINE static constexpr auto
  id(::veg::internal::storage::storage<T&> const& arg) noexcept -> i64 {
    return arg.inner_ptr == nullptr ? 0 : -1;
  }
};

template <typename T>
struct meta::value_sentinel_for<veg::internal::storage::storage<T&&>>
    : std::integral_constant<i64, 1> {
  HEDLEY_ALWAYS_INLINE static constexpr auto invalid(i64 i) noexcept
      -> ::veg::internal::storage::storage<T&&> {
    return VEG_ASSERT(i == 0), ::veg::internal::storage::storage<T&&>::null();
  }
  HEDLEY_ALWAYS_INLINE static constexpr auto
  id(::veg::internal::storage::storage<T&&> const& arg) noexcept -> i64 {
    return arg.inner_ptr == nullptr ? 0 : -1;
  }
};

template <typename T>
struct meta::value_sentinel_for<veg::internal::storage::storage<T>>
    : std::integral_constant<i64, meta::value_sentinel_for<T>::value> {
  HEDLEY_ALWAYS_INLINE static constexpr auto invalid(i64 i) noexcept
      -> ::veg::internal::storage::storage<T> {
    return ::veg::internal::storage::storage<T>{
        veg::internal::storage::hidden_tag2{},
        meta::value_sentinel_for<T>::invalid(i)};
  }
  HEDLEY_ALWAYS_INLINE static constexpr auto
  id(::veg::internal::storage::storage<T> const& arg) noexcept -> i64 {
    return meta::value_sentinel_for<T>::id(arg.inner_val);
  }
};

} // namespace veg

#endif /* end of include guard __VEG_STORAGE_HPP_X0B4XDKES */
