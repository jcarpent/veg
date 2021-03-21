#ifdef VEG_PROLOGUE
#error "missing epilogue"
#endif
#define VEG_PROLOGUE

#if (                                                                          \
		!defined(VEG_DISABLE_CPP14_EXTENSIONS) &&                                  \
		(defined(VEG_ENABLE_CPP14_EXTENSIONS) || defined(__clang__))) ||           \
		__cplusplus >= 201402L
#define VEG_HAS_CPP14_EXTENSIONS
#endif

#define VEG_DECLVAL(...) (static_cast<__VA_ARGS__ (*)()>(nullptr)())

#if __cplusplus >= 201703L
#define VEG_DECLVAL_NOEXCEPT(...)                                              \
	(static_cast<__VA_ARGS__ (*)() noexcept>(nullptr)())
#else
#define VEG_DECLVAL_NOEXCEPT(...)                                              \
	(::veg::meta::internal::declval<__VA_ARGS__>())
#endif

#define VEG_DEDUCE_RET(...)                                                    \
	noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__) {                     \
		return __VA_ARGS__;                                                        \
	}                                                                            \
	VEG_NOM_SEMICOLON

#define VEG_CHECK_CONCEPT(...) ::veg::concepts::check_##__VA_ARGS__()

#define VEG_IMPL_AND(r, _, param) &&__VEG_PP_UNWRAP(param)
#define VEG_IMPL_OR(r, _, param) || __VEG_PP_UNWRAP(param)

#define __VEG_PP_UNWRAP(...) __VEG_PP_HEAD __VA_ARGS__ __VEG_PP_TAIL __VA_ARGS__

#if __cplusplus >= 202002L

#define VEG_IMPL_CONJUNCTION(First, ...)                                       \
	(__VEG_PP_UNWRAP(First) __VEG_PP_SEQ_FOR_EACH(                               \
			VEG_IMPL_AND, _, __VEG_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#define VEG_IMPL_DISJUNCTION(First, ...)                                       \
	(__VEG_PP_UNWRAP(First) __VEG_PP_SEQ_FOR_EACH(                               \
			VEG_IMPL_OR, _, __VEG_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#define VEG_DEF_CONCEPT(tpl, name, ...)                                        \
	template <__VEG_PP_REMOVE_PAREN1(tpl)>                                       \
	concept name = __VA_ARGS__;                                                  \
	template <__VEG_PP_REMOVE_PAREN1(tpl)>                                       \
	requires(__VA_ARGS__) void check_##name()

#define VEG_CONCEPT(...) (::veg::concepts::__VA_ARGS__)
#define VEG_DEF_CONCEPT_CONJUNCTION(tpl, name, terms)                          \
	VEG_DEF_CONCEPT(tpl, name, VEG_IMPL_CONJUNCTION terms)
#define VEG_DEF_CONCEPT_DISJUNCTION(tpl, name, terms)                          \
	VEG_DEF_CONCEPT(tpl, name, VEG_IMPL_DISJUNCTION terms)

#define VEG_ALL_OF(...) ((__VA_ARGS__) && ...)

#else

#if __cplusplus >= 201703L
#define VEG_IMPL_DEF_CONCEPT(tpl, name, Value, ...)                            \
	namespace _ {                                                                \
	template <__VEG_PP_REMOVE_PAREN1(tpl)>                                       \
	struct name : __VA_ARGS__ {};                                                \
	}                                                                            \
	template <__VEG_PP_REMOVE_PAREN1(tpl)>                                       \
	inline constexpr bool const& name = Value
#elif __cplusplus >= 201402L || defined(VEG_HAS_CPP14_EXTENSIONS)
#define VEG_IMPL_DEF_CONCEPT(tpl, name, Value, ...)                            \
	namespace _ {                                                                \
	template <__VEG_PP_REMOVE_PAREN1(tpl)>                                       \
	struct name : __VA_ARGS__ {};                                                \
	}                                                                            \
	namespace /* NOLINT */ {                                                     \
	VEG_IGNORE_CPP14_EXTENSION_WARNING(                                          \
			template <__VEG_PP_REMOVE_PAREN1(tpl)>                                   \
			constexpr bool const& name = ::veg::meta::bool_constant<Value>::value);  \
	}                                                                            \
	VEG_NOM_SEMICOLON
#else
#define VEG_IMPL_DEF_CONCEPT(tpl, name, Value, ...)                            \
	template <__VEG_PP_REMOVE_PAREN1(tpl)>                                       \
	struct name : __VA_ARGS__ {}
#endif

#if __cplusplus >= 201402L || defined(VEG_HAS_CPP14_EXTENSIONS)
#define VEG_CONCEPT(...) ::veg::concepts::__VA_ARGS__
#define VEG_IMPL_ADD_VALUE(r, _, param) (param)
#define VEG_IMPL_TRAIT(param) __VEG_PP_HEAD param _::__VEG_PP_TAIL param
#else
#define VEG_CONCEPT(...) ::veg::concepts::__VA_ARGS__::value
#define VEG_IMPL_ADD_VALUE(r, _, param) ((__VEG_PP_REMOVE_PAREN(param)::value))
#define VEG_IMPL_TRAIT(param) __VEG_PP_UNWRAP(param)
#endif
#define VEG_IMPL_PUT_TRAIT(r, _, param) , VEG_IMPL_TRAIT(param)

#define VEG_DEF_CONCEPT(tpl, name, ...)                                        \
	VEG_IMPL_DEF_CONCEPT(                                                        \
			tpl, name, (__VA_ARGS__), ::veg::meta::bool_constant<__VA_ARGS__>);      \
	VEG_TEMPLATE(tpl, requires(__VA_ARGS__), void check_##name, (_ = 0, int))    \
	noexcept

#define VEG_IMPL_SFINAE(r, _, param)                                           \
	, typename ::veg::meta::enable_if<__VEG_PP_UNWRAP(param), int>::type = 0

#define VEG_IMPL_OVERLOAD(r, name_tpl, param)                                  \
	template <                                                                   \
			__VEG_PP_REMOVE_PAREN(__VEG_PP_TAIL name_tpl),                           \
			typename ::veg::meta::enable_if<__VEG_PP_UNWRAP(param), int>::type = 0>  \
	void __VEG_PP_CAT(check_, __VEG_PP_HEAD name_tpl)() noexcept;

#define VEG_DEF_CONCEPT_BOOL_CONJUNCTION_IMPL(tpl, name, base, seq)            \
	VEG_IMPL_DEF_CONCEPT(                                                        \
			tpl,                                                                     \
			name,                                                                    \
			(__VEG_PP_REMOVE_PAREN1(base)::value),                                   \
			__VEG_PP_REMOVE_PAREN1(base));                                           \
	template <__VEG_PP_REMOVE_PAREN(tpl)                                         \
	              __VEG_PP_SEQ_FOR_EACH(VEG_IMPL_SFINAE, _, seq)>                \
	void check_##name() noexcept
#define VEG_DEF_CONCEPT_BOOL_DISJUNCTION_IMPL(tpl, name, base, terms)          \
	VEG_IMPL_DEF_CONCEPT(                                                        \
			tpl,                                                                     \
			name,                                                                    \
			(__VEG_PP_REMOVE_PAREN1(base)::value),                                   \
			__VEG_PP_REMOVE_PAREN1(base));                                           \
	__VEG_PP_SEQ_FOR_EACH(VEG_IMPL_OVERLOAD, (name, tpl), terms)                 \
	VEG_NOM_SEMICOLON

#define VEG_DEF_CONCEPT_CONJUNCTION(tpl, name, terms)                          \
	VEG_DEF_CONCEPT_BOOL_CONJUNCTION_IMPL(                                       \
			tpl,                                                                     \
			name,                                                                    \
			(VEG_IMPL_CONJUNCTION terms),                                            \
			__VEG_PP_SEQ_FOR_EACH(                                                   \
					VEG_IMPL_ADD_VALUE, _, __VEG_PP_VARIADIC_TO_SEQ terms))

#define VEG_DEF_CONCEPT_DISJUNCTION(tpl, name, terms)                          \
	VEG_DEF_CONCEPT_BOOL_DISJUNCTION_IMPL(                                       \
			tpl,                                                                     \
			name,                                                                    \
			(VEG_IMPL_DISJUNCTION terms),                                            \
			__VEG_PP_SEQ_FOR_EACH(                                                   \
					VEG_IMPL_ADD_VALUE, _, __VEG_PP_VARIADIC_TO_SEQ terms))

#define VEG_IMPL_CONJUNCTION(First, ...)                                       \
	conjunction<VEG_IMPL_TRAIT(First) __VEG_PP_SEQ_FOR_EACH(                     \
			VEG_IMPL_PUT_TRAIT, _, __VEG_PP_VARIADIC_TO_SEQ(__VA_ARGS__))>
#define VEG_IMPL_DISJUNCTION(First, ...)                                       \
	disjunction<VEG_IMPL_TRAIT(First) __VEG_PP_SEQ_FOR_EACH(                     \
			VEG_IMPL_PUT_TRAIT, _, __VEG_PP_VARIADIC_TO_SEQ(__VA_ARGS__))>

#if __cplusplus >= 201703L
#define VEG_ALL_OF(...) ((__VA_ARGS__) && ...)
#else
#define VEG_ALL_OF(...) (::veg::meta::all_of({(__VA_ARGS__)...}))
#endif

#endif

#define VEG_IMPL_PARAM_EXPAND(r, _, param)                                     \
	__VEG_PP_COMMA_IF(__VEG_PP_IS_BEGIN_PARENS(param))                           \
	__VEG_PP_REMOVE_PAREN(__VEG_PP_IF(                                           \
			__VEG_PP_IS_BEGIN_PARENS(param),                                         \
			(__VEG_PP_TAIL param)__VEG_PP_HEAD param, ))

#define VEG_TEMPLATE(tparams, requirement, attr_name, ...)                     \
	VEG_IMPL_TEMPLATE(                                                           \
			attr_name, tparams, requirement, __VEG_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#if __cplusplus >= 202002L
#define VEG_TEMPLATE_EXPLICIT(                                                 \
		cond, tparams, requirement, attr_name, args, ...)                          \
	VEG_TEMPLATE(                                                                \
			tparams,                                                                 \
			requirement,                                                             \
			explicit(cond) attr_name,                                                \
			__VEG_PP_REMOVE_PAREN(args))                                             \
	__VA_ARGS__
#else
#define VEG_TEMPLATE_EXPLICIT(                                                 \
		cond, tparams, requirement, attr_name, args, ...)                          \
	VEG_TEMPLATE(                                                                \
			(__VEG_PP_REMOVE_PAREN tparams,                                          \
	     typename ::veg::meta::enable_if<(cond), int>::type = 0),                \
			requirement,                                                             \
			explicit attr_name,                                                      \
			__VEG_PP_REMOVE_PAREN(args))                                             \
	__VA_ARGS__                                                                  \
	VEG_TEMPLATE(                                                                \
			(__VEG_PP_REMOVE_PAREN tparams,                                          \
	     typename ::veg::meta::enable_if<!(cond), unsigned>::type = 0),          \
			requirement,                                                             \
			attr_name,                                                               \
			__VEG_PP_REMOVE_PAREN(args))                                             \
	__VA_ARGS__
#endif

#define VEG_IMPL_TEMPLATE(attr_name, tparams, requirement, args)               \
	VEG_IMPL_TEMPLATE2(                                                          \
			attr_name,                                                               \
			tparams,                                                                 \
			(__VEG_PP_CAT2(VEG_IMPL_PREFIX_, requirement)),                          \
			__VEG_PP_SEQ_HEAD(args),                                                 \
			__VEG_PP_SEQ_TAIL(args))

#define VEG_IMPL_PREFIX_requires

#if __cplusplus >= 202002L
#define VEG_IMPL_TEMPLATE2(attr_name, tparams, requirement, first_param, args) \
	template <__VEG_PP_REMOVE_PAREN(tparams)>                                    \
	requires(requirement) attr_name /* NOLINT */ (                               \
			__VEG_PP_TAIL first_param __VEG_PP_HEAD first_param                      \
					__VEG_PP_SEQ_FOR_EACH(VEG_IMPL_PARAM_EXPAND, _, args))
#else
#define VEG_IMPL_TEMPLATE2(attr_name, tparams, requirement, first_param, args) \
	template <__VEG_PP_REMOVE_PAREN(tparams)>                                    \
	attr_name /* NOLINT */ (                                                     \
			::veg::meta::discard_1st<                                                \
					typename ::veg::meta::enable_if<(requirement)>::type,                \
					__VEG_PP_TAIL first_param> __VEG_PP_HEAD first_param                 \
					__VEG_PP_SEQ_FOR_EACH(VEG_IMPL_PARAM_EXPAND, _, args))

#endif

#define VEG_HAS_BUILTIN_OR_0(true, false) __VEG_PP_REMOVE_PAREN(false)
#define VEG_HAS_BUILTIN_OR_1(true, false) __VEG_PP_REMOVE_PAREN(true)
#define VEG_HAS_BUILTIN_OR(builtin, true, false)                               \
	__VEG_PP_CAT(VEG_HAS_BUILTIN_OR_, VEG_HAS_BUILTIN(builtin))(true, false)
#define VEG_DEF_CONCEPT_FROM_BUILTIN_OR_TRAIT(tpl, trait, std_trait, ...)      \
	VEG_DEF_CONCEPT(                                                             \
			tpl,                                                                     \
			trait,                                                                   \
			VEG_HAS_BUILTIN_OR(                                                      \
					__VEG_PP_CAT(__, std_trait),                                         \
					__VEG_PP_CAT(__, std_trait)(__VA_ARGS__),                            \
					(::std::std_trait<__VA_ARGS__>::value)))
#define VEG_DEF_CONCEPT_FROM_BUILTIN_OR_STD(tpl, trait, ...)                   \
	VEG_DEF_CONCEPT_FROM_BUILTIN_OR_TRAIT(                                       \
			tpl, trait, __VEG_PP_CAT(is_, trait), __VA_ARGS__)

#define VEG_DEFAULT_CTOR_ASSIGN(class_name)                                    \
	~class_name() = default;                                                     \
	class_name /* NOLINT */ (class_name &&) = default;                           \
	class_name(class_name const&) = default;                                     \
	auto operator= /* NOLINT */(class_name&&)&->class_name& = default;           \
	auto operator=(class_name const&)&->class_name& = default

#if __cplusplus >= 201402L
#define VEG_CPP14(...) __VA_ARGS__
#else
#define VEG_CPP14(...)
#endif

#if __cplusplus >= 201703L
#define VEG_CPP17(...) __VA_ARGS__
#else
#define VEG_CPP17(...)
#endif

#if __cplusplus >= 202002L
#define VEG_CPP20(...) __VA_ARGS__
#else
#define VEG_CPP20(...)
#endif

#if defined(__has_builtin)
#define VEG_HAS_BUILTIN(x) __has_builtin(x)
#else
#define VEG_HAS_BUILTIN(x) 0
#endif

#if __cplusplus >= 201703L
#define VEG_NODISCARD [[nodiscard]]
#elif defined(__clang__)
#define VEG_NODISCARD HEDLEY_WARN_UNUSED_RESULT
#else
#define VEG_NODISCARD
#endif

#ifdef VEG_INTERNAL_ASSERTIONS
#define VEG_INTERNAL_ASSERT(...)                                               \
	VEG_ASSERT_ELSE("inner assertion failed", __VA_ARGS__)
#else
#define VEG_INTERNAL_ASSERT(...)                                               \
	VEG_DEBUG_ASSERT_ELSE("inner assertion failed", __VA_ARGS__)
#endif

#ifdef VEG_HAS_CPP14_EXTENSIONS

#ifdef HEDLEY_HAS_WARNING
#if HEDLEY_HAS_WARNING("-Wc++14-extensions")
#define VEG_IGNORE_CPP14_EXTENSION_WARNING(...)                                \
	HEDLEY_DIAGNOSTIC_PUSH                                                       \
	HEDLEY_PRAGMA(clang diagnostic ignored "-Wc++14-extensions")                 \
	__VA_ARGS__                                                                  \
	HEDLEY_DIAGNOSTIC_POP
#else
#define VEG_IGNORE_CPP14_EXTENSION_WARNING(...) __VA_ARGS__
#endif
#else
#error "[veg] hedley.h missing"
#endif

#else
#define VEG_IGNORE_CPP14_EXTENSION_WARNING(...) VEG_NOM_SEMICOLON
#endif

#define VEG_NOM_SEMICOLON static_assert(true, ".")

#if __cplusplus >= 202002L
#define VEG_ABI abi20
#elif __cplusplus >= 201703L
#define VEG_ABI abi17
#elif __cplusplus >= 201402L
#define VEG_ABI abi14
#elif __cplusplus >= 201102L
#define VEG_ABI abi11
#else
#error "[veg] c++ standards earlier than c++11 are not supported"
#endif

#define VEG_ABI_VERSION lol
