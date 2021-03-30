#include "static_assert.hpp"
#include <fmt/core.h>
#include <limits>
#include <veg/tuple.hpp>
#include <utility>
#include <doctest.h>
#include "veg/memory/placement.hpp"
#include "veg/internal/prologue.hpp"

#define MOV VEG_MOV
#define FWD VEG_FWD
#define ASSERT_SAME(...) STATIC_ASSERT(::std::is_same<__VA_ARGS__>::value)

using namespace veg::literals;
using namespace veg::tags;

template <typename>
void get() = delete;

TEST_CASE("tuple: adl_get") {
	veg::tuple<int, float> t{};
	get<0>(t);
	STATIC_ASSERT(VEG_CONCEPT(same<decltype(get<0>(t)), int&>));
	STATIC_ASSERT(VEG_CONCEPT(
			same<
					decltype(get<0>(static_cast<veg::tuple<int, float> const&>(t))),
					int const&>));
	STATIC_ASSERT(VEG_CONCEPT(
			same<decltype(get<0>(static_cast<veg::tuple<int, float>&&>(t))), int&&>));
}

TEST_CASE("tuple: all") {
	using namespace veg;

	veg::tuple<int, char, bool> tup{cvt, 1, 'c', true};
	veg::tuple<int, char&&, char, bool&, bool const&> tup_ref{
			cvt,
			1,
			MOV(tup).as_ref()[1_c],
			'c',
			nb::get<2>{}(tup),
			nb::get<2>{}(tup)};

	{
		STATIC_ASSERT(VEG_CONCEPT(trivially_relocatable<decltype(tup)>));
		STATIC_ASSERT(VEG_CONCEPT(trivially_copyable<decltype(tup)>));

		STATIC_ASSERT(VEG_CONCEPT(
				trivially_copy_constructible<veg::tuple<int&, int const&>>));
		STATIC_ASSERT(VEG_CONCEPT(
				trivially_move_constructible<veg::tuple<int&, int const&>>));
		STATIC_ASSERT(!VEG_CONCEPT(copy_assignable<veg::tuple<int&, int const&>>));
		STATIC_ASSERT(!VEG_CONCEPT(move_assignable<veg::tuple<int&, int const&>>));

		STATIC_ASSERT(!VEG_CONCEPT(copy_assignable<veg::tuple<int&, float&>>));
		STATIC_ASSERT(VEG_CONCEPT(assignable<
															veg::tuple<int&, float&> const&&,
															veg::tuple<int&, float&>&&>));
		STATIC_ASSERT(VEG_CONCEPT(assignable<
															veg::tuple<int&, float&> const&&,
															veg::tuple<int&, float&>&&>));

		STATIC_ASSERT(!VEG_CONCEPT(copy_assignable<decltype(tup_ref)>));
		STATIC_ASSERT(!VEG_CONCEPT(move_assignable<decltype(tup_ref)>));
		STATIC_ASSERT(!VEG_CONCEPT(copy_constructible<decltype(tup_ref)>));
		STATIC_ASSERT(VEG_CONCEPT(trivially_move_constructible<decltype(tup_ref)>));
		STATIC_ASSERT(!std::is_copy_constructible<decltype(tup_ref)>::value);
		STATIC_ASSERT(std::is_copy_constructible<decltype(tup)>::value);
		STATIC_ASSERT(std::is_copy_constructible<veg::tuple<int&, bool&>>::value);
		using val_tup = veg::tuple<int, bool>;
		using ref_tup = veg::tuple<int&, bool&>;

		STATIC_ASSERT(VEG_CONCEPT(swappable<ref_tup const&&, ref_tup const&&>));
		STATIC_ASSERT(VEG_CONCEPT(swappable<ref_tup&&, ref_tup const&&>));
		STATIC_ASSERT(VEG_CONCEPT(swappable<ref_tup const&&, ref_tup&&>));
		STATIC_ASSERT(VEG_CONCEPT(swappable<ref_tup const&&, ref_tup const&&>));
		STATIC_ASSERT(VEG_CONCEPT(swappable<ref_tup&&, ref_tup&&>));
		STATIC_ASSERT(VEG_CONCEPT(nothrow_swappable<ref_tup&&, ref_tup&&>));

		STATIC_ASSERT(VEG_CONCEPT(swappable<ref_tup const&&, val_tup&>));
		STATIC_ASSERT(VEG_CONCEPT(swappable<val_tup&, val_tup&>));
		STATIC_ASSERT(!VEG_CONCEPT(swappable<val_tup&&, val_tup&&>));
		STATIC_ASSERT(!VEG_CONCEPT(swappable<val_tup&, val_tup&&>));

		STATIC_ASSERT(VEG_CONCEPT(nothrow_swappable<ref_tup&&, ref_tup&&>));
		STATIC_ASSERT(VEG_CONCEPT(nothrow_swappable<ref_tup&&, ref_tup const&&>));
		STATIC_ASSERT(VEG_CONCEPT(nothrow_swappable<ref_tup const&&, ref_tup&&>));
		STATIC_ASSERT(
				VEG_CONCEPT(nothrow_swappable<ref_tup const&&, ref_tup const&&>));
		STATIC_ASSERT(VEG_CONCEPT(nothrow_swappable<ref_tup&&, ref_tup&&>));
		STATIC_ASSERT(VEG_CONCEPT(nothrow_swappable<ref_tup&&, ref_tup&&>));

		STATIC_ASSERT(VEG_CONCEPT(nothrow_swappable<ref_tup const&&, val_tup&>));
		STATIC_ASSERT(VEG_CONCEPT(nothrow_swappable<val_tup&, val_tup&>));
		STATIC_ASSERT(!VEG_CONCEPT(nothrow_swappable<val_tup&&, val_tup&&>));
		STATIC_ASSERT(!VEG_CONCEPT(nothrow_swappable<val_tup&, val_tup&&>));
	}
	{
		using val_tup = veg::tuple<int, bool>;
		val_tup a{cvt, 5, true};
		val_tup b{cvt, 3, false};
		STATIC_ASSERT(VEG_CONCEPT(constructible<veg::tuple<long, bool>, val_tup>));
		STATIC_ASSERT(
				VEG_CONCEPT(constructible<veg::tuple<long, bool>, val_tup const&>));

		veg::swap(a, b);
		CHECK(a[0_c] == 3);
		CHECK(b[0_c] == 5);
		CHECK(a[1_c] == false);
		CHECK(b[1_c] == true);
		STATIC_ASSERT(val_tup{cvt, 1, true} == val_tup{cvt, 1, true});
		STATIC_ASSERT(noexcept(val_tup{cvt, 1, true} == val_tup{cvt, 1, true}));
		STATIC_ASSERT(
				cmp::three_way(val_tup{cvt, 1, true}, val_tup{cvt, 1, true}) == 0);

		CHECK(a == (val_tup{cvt, 3, false}));
		CHECK(b == (val_tup{cvt, 5, true}));
	}
	{
		using ref_tup = veg::tuple<int&>;
		using val_tup = veg::tuple<int>;
		int i = 13;
		val_tup j{cvt, 12};
		ref_tup a{cvt, i};
		ref_tup const b{j};
		veg::swap(FWD(a), FWD(b));

		CHECK(i == 12);
		CHECK(j[0_c] == 13);

		FWD(a) = b;
		CHECK(i == 13);
		CHECK(j[0_c] == 13);

		STATIC_ASSERT(VEG_CONCEPT(assignable<ref_tup const&&, ref_tup const&>));
	}

	{
		using ref_tup = veg::tuple<int&>;
		using rref_tup = veg::tuple<int&&>;
		STATIC_ASSERT(VEG_CONCEPT(trivially_copyable<ref_tup>));
		STATIC_ASSERT(VEG_CONCEPT(trivially_copyable<rref_tup>));
		STATIC_ASSERT(VEG_CONCEPT(constructible<ref_tup, ref_tup>));
		STATIC_ASSERT(VEG_CONCEPT(constructible<ref_tup, ref_tup&>));
		STATIC_ASSERT(VEG_CONCEPT(constructible<ref_tup, rref_tup&>));
		STATIC_ASSERT(VEG_CONCEPT(constructible<ref_tup, rref_tup const&>));
		STATIC_ASSERT(!VEG_CONCEPT(constructible<ref_tup, rref_tup&&>));
		int i = 13;
		int j = 12;
		ref_tup a{cvt, i};
		rref_tup b{cvt, FWD(j)};

		veg::swap(a.as_ref(), b.as_ref());

		CHECK(i == 12);
		CHECK(j == 13);

		FWD(a) = FWD(b);
		static_cast<ref_tup const&&>(a) = b;
		static_cast<ref_tup const&&>(a) = FWD(b);
		CHECK(i == 13);
		CHECK(j == 13);

		STATIC_ASSERT(VEG_CONCEPT(assignable<ref_tup const&&, ref_tup const&>));
	}

	CHECK(tup[0_c] == 1);
	CHECK(tup[1_c] == 'c');
	CHECK(tup[2_c]);

	{
		auto&& ref = VEG_MOV(tup)[2_c];
		auto&& ref2 = VEG_MOV(tup).as_ref()[2_c];
		CHECK(&ref != &tup[2_c]);
		CHECK(&ref2 == &tup[2_c]);
	}

	VEG_BIND(auto, (e, f, g), [&] { return tup; }());
#if __cplusplus >= 201703L
	auto [i, c, b] = [&] { return tup; }();
	CHECK(i == 1);
	CHECK(c == 'c');
	CHECK(b);
	veg::tuple tup_deduce{cvt, 1, 'c', true};
#endif

	STATIC_ASSERT(std::is_copy_assignable<veg::tuple<int, char>>());
	STATIC_ASSERT(std::is_trivially_copyable<veg::tuple<int, char>>());
	STATIC_ASSERT(!std::is_copy_assignable<veg::tuple<int&, char&>>());
	STATIC_ASSERT(!std::is_copy_assignable<veg::tuple<int&>>());
	ASSERT_SAME(decltype(tup[0_c]), int&);
	ASSERT_SAME(decltype(tup[0_c]), decltype(tup[0_c]));
	ASSERT_SAME(decltype(MOV(tup)[0_c]), decltype(MOV(tup)[0_c]));
	ASSERT_SAME(decltype(MOV(tup)[0_c]), int);
	ASSERT_SAME(decltype(MOV(tup).as_ref()[0_c]), int&&);

	ASSERT_SAME(decltype(tup.as_ref()), veg::tuple<int&, char&, bool&>);
	ASSERT_SAME(decltype(MOV(tup).as_ref()), veg::tuple<int&&, char&&, bool&&>);

	ASSERT_SAME(decltype(e), int&&);
	ASSERT_SAME(decltype((e)), int&);
	ASSERT_SAME(decltype(f), char&&);
	ASSERT_SAME(decltype((f)), char&);
	ASSERT_SAME(decltype((g)), bool&);

	ASSERT_SAME(decltype(tup_ref[0_c]), int&);
	ASSERT_SAME(decltype(MOV(tup_ref).as_ref()[0_c]), int&&);

	ASSERT_SAME(decltype(tup_ref[1_c]), char&);
	ASSERT_SAME(decltype(tup_ref[2_c]), char&);
	ASSERT_SAME(decltype(MOV(tup_ref)[1_c]), char&&);
	ASSERT_SAME(decltype(MOV(tup_ref)[2_c]), char);
	ASSERT_SAME(decltype(MOV(tup_ref).as_ref()[2_c]), char&&);

	ASSERT_SAME(decltype(tup_ref[3_c]), bool&);
	ASSERT_SAME(decltype(tup_ref[4_c]), bool const&);
	ASSERT_SAME(decltype(MOV(tup_ref)[3_c]), bool&);
	ASSERT_SAME(decltype(MOV(tup_ref)[4_c]), bool const&);

#if __cplusplus >= 201703L
	ASSERT_SAME(decltype(i), int);
	ASSERT_SAME(decltype((i)), int&);
	ASSERT_SAME(decltype((b)), bool&);
	ASSERT_SAME(decltype(tup_deduce), veg::tuple<int, char, bool>);
#endif

	STATIC_ASSERT(tuple<int>{cvt, 1} == tuple<int>{cvt, 1});
	STATIC_ASSERT(tuple<int>{cvt, 1} == tuple<double>{cvt, 1});
	STATIC_ASSERT(tuple<int>{cvt, 1} != tuple<double>{cvt, 2});
	STATIC_ASSERT(
			tuple<int, double>{cvt, 1, 2.0F} == tuple<double, int>{cvt, 1.0F, 2});
	STATIC_ASSERT(
			tuple<int, double>{cvt, 1, 2.0F} == tuple<double, int>{cvt, 1.0F, 2});
	STATIC_ASSERT(
			tuple<int, double>{cvt, 1, 2.0F} != tuple<double, int>{cvt, 2.0F, 2});
}

TEST_CASE("tuple: nested") {
	using namespace veg;
	tuple<int, tuple<int, float>> tup{cvt, 1, make::tuple(2, 3.0F)};
	CHECK(tup[0_c] == 1);

	CHECK(tup[1_c][1_c] == 3.0F);
	CHECK(tup[1_c][0_c] == 2);
	ASSERT_SAME(decltype(tup[1_c][0_c]), int&);
	ASSERT_SAME(decltype(tup[1_c][0_c]), int&);
	ASSERT_SAME(decltype(VEG_MOV(tup)[1_c][0_c]), int);

	STATIC_ASSERT(sizeof(tuple<int>) == sizeof(int));
	STATIC_ASSERT(sizeof(tuple<tuple<int>>) == sizeof(int));
	STATIC_ASSERT(sizeof(tuple<tuple<tuple<int>>>) == sizeof(int));
}

TEST_CASE("tuple: cmp") {
	using namespace veg;
	using veg::cmp::three_way;
	using tup_i = tuple<int, int, int>;
	using tup_d = tuple<int, int, double>;
	constexpr double nan = std::numeric_limits<double>::quiet_NaN();

	STATIC_ASSERT(VEG_CONCEPT(
			same<decltype(three_way(tup_d{}, tup_d{})), cmp::partial_ordering>));
	STATIC_ASSERT(VEG_CONCEPT(
			same<decltype(cmp_3way(tup_i{}, tup_d{})), cmp::partial_ordering>));
	STATIC_ASSERT(VEG_CONCEPT(
			same<decltype(cmp_3way(tup_d{}, tup_i{})), cmp::partial_ordering>));
	STATIC_ASSERT(VEG_CONCEPT(
			same<decltype(cmp_3way(tup_i{}, tup_i{})), cmp::strong_ordering>));

	{
		constexpr auto t1 = make::tuple(1, 2, 3);
		constexpr auto t2 = make::tuple(1, 2, 3);
		constexpr auto t3 = make::tuple(0, 2, 3);
		constexpr auto t4 = make::tuple(1, 2, 4);

		STATIC_ASSERT(three_way(t1, t2) == 0);
		STATIC_ASSERT(three_way(t1, t3) > 0);
		STATIC_ASSERT(three_way(t1, t4) < 0);
		STATIC_ASSERT(three_way(t2, t3) > 0);
		STATIC_ASSERT(three_way(t2, t4) < 0);
		STATIC_ASSERT(three_way(t3, t4) < 0);
	}
	CHECK(
			three_way(make::tuple(1, 1, nan), make::tuple(1, 1, 0)) ==
			cmp::partial_ordering::unordered);

	CHECK(
			three_way(make::tuple(0, 1, nan), make::tuple(1, 1, 0)) ==
			cmp::partial_ordering::less);

	CHECK(
			three_way(make::tuple(1, 2, nan), make::tuple(1, 1, 0)) ==
			cmp::partial_ordering::greater);
}

TEST_CASE("tuple: empty") {
	using namespace veg;
	tuple<> t1{};
	tuple<> t2(cvt);
	CHECK(t1 == t2);
	STATIC_ASSERT(sizeof(t1) == 1);
	STATIC_ASSERT(tuple<>{} == tuple<>{});
}

TEST_CASE("tuple: cvt") {
	using namespace veg;
	tuple<int, double> t1{cvt, 1, 1.5F};
	tuple<long, double> t2(cvt, 3, 2.5);
	tuple<long, double> t3(t1);
	tuple<int, double> t4(t3);

	STATIC_ASSERT(sizeof(t2) == sizeof(long) + sizeof(double));

	CHECK(t1 == t4);
	t1 = t2;
	CHECK(t1 == t2);
}

TEST_CASE("tuple: non_movable") {
	struct S {
		S() = default;
		S(S&&) = delete;
		S(S const&) = delete;
	};
	using namespace veg;
	{
		tuple<S> s{};
		(void)s;
	}
	VEG_CPP17({
		tuple<S> s{cvt, make::from_callable([] { return S{}; })};
		(void)s;
	})
}

TEST_CASE("tuple: get") {
	using namespace veg;
	int arr[] = {1, 2, 3};
	nb::get<0>{}(arr);
	nb::get<0>{}(FWD(arr));
}

struct T : veg::tuple<int, float> {
	using tuple::tuple;
	using tuple::operator=;
};
struct Tref : veg::tuple<int&, float&> {
	using tuple::tuple;
	using tuple::operator=;
};

TEST_CASE("tuple: derived") {
	T t{cvt, 1, 2.0F};
	T t2{cvt, 3, 4.0F};

	CHECK(t[0_c] == 1);
	CHECK(t[1_c] == 2.0F);
	swap(t, t);
	CHECK(t[0_c] == 1);
	CHECK(t[1_c] == 2.0F);
	Tref r{t};
	swap(t, FWD(r));
	CHECK(t[0_c] == 1);
	CHECK(t[1_c] == 2.0F);
	swap(FWD(r), FWD(r));
	CHECK(t[0_c] == 1);
	CHECK(t[1_c] == 2.0F);

	swap(t2, FWD(r));
	CHECK(t[0_c] == 3);
	CHECK(t[1_c] == 4.0F);
	swap(FWD(r), t2);
	CHECK(t[0_c] == 1);
	CHECK(t[1_c] == 2.0F);

	FWD(r) = static_cast<veg::tuple<int, float> const&>(t2);
	CHECK(t[0_c] == 3);
	CHECK(t[1_c] == 4.0F);

	{
		using namespace veg;
		STATIC_ASSERT(!VEG_CONCEPT(swappable<T, T>));
		STATIC_ASSERT(!VEG_CONCEPT(swappable<T const&, T const&>));
		STATIC_ASSERT(VEG_CONCEPT(swappable<Tref const&&, Tref const&&>));
		STATIC_ASSERT(VEG_CONCEPT(swappable<Tref, Tref>));
	}
	ASSERT_SAME(decltype(get<0>(r)), int&);
	ASSERT_SAME(decltype(get<0>(FWD(r))), int&);
	ASSERT_SAME(decltype(get<0>(t)), int&);
	ASSERT_SAME(decltype(get<0>(FWD(t))), int&&);
	ASSERT_SAME(decltype(t[0_c]), int&);
	ASSERT_SAME(decltype(FWD(t)[0_c]), int);
}
#include "veg/internal/epilogue.hpp"
