#pragma once

//#define DISABLE_DEBUG_VALIDATING_MATH_VALUES

namespace StdLib
{
#if defined(DEBUG) && !defined(DISABLE_DEBUG_VALIDATING_MATH_VALUES)
	#define VALIDATE_FP_VALUE(value) \
		ASSUME(value == value); /* check for NaN */ \
		ASSUME(value * 0 == value * 0); /* check for Inf */ \
		if constexpr (std::is_same_v<decltype(value), f32>) \
			ASSUME(value == 0 || value == -0 || value <= -FLT_MIN || value >= FLT_MIN); /* check for denormals */ \
		else \
			ASSUME(value == 0 || value == -0 || value <= -DBL_MIN || value >= DBL_MIN); /* check for denormals */ \

	#define VALIDATE_VALUES_ARRAY(values, count) \
		for (uiw index = 0; index < (count); ++index) \
		{ \
			VALIDATE_FP_VALUE((values)[index]); \
		}

	template <typename ScalarType, uiw Dim> constexpr bool __IsVector(_VectorBase<ScalarType, Dim>) { return true; }
	template <typename ScalarType = void, uiw Dim = 0> constexpr bool __IsVector(f32) { return false; }
	template <typename ScalarType = void, uiw Dim = 0> constexpr bool __IsVector(f64) { return false; }

	#define VALIDATE_VALUE(v) \
		{ \
			using t = std::remove_reference_t<std::remove_cv_t<decltype(v)>>; \
			if constexpr (std::is_floating_point_v<t>) \
			{ \
				if constexpr (__IsVector(t())) \
				{ \
					if constexpr (std::is_same_v<t::ScalarType, f32>) \
					{ \
						VALIDATE_VALUES_ARRAY(&v.x, t::dim); \
					} \
				} \
				else \
				{ \
					VALIDATE_FP_VALUE(v); \
				} \
			} \
		}

	template <typename T0> constexpr void _ValidateValues(const T0 &v0)
	{
		VALIDATE_VALUE(v0);
	}

	template <typename T0, typename T1> constexpr void _ValidateValues(const T0 &v0, const T1 &v1)
	{
		VALIDATE_VALUE(v0);
		VALIDATE_VALUE(v1);
	}

	template <typename T0, typename T1, typename T2> constexpr void _ValidateValues(const T0 &v0, const T1 &v1, const T2 &v2)
	{
		VALIDATE_VALUE(v0);
		VALIDATE_VALUE(v1);
		VALIDATE_VALUE(v2);
	}

	#undef VALIDATE_FP_VALUE
	#undef VALIDATE_VALUES_ARRAY
	#undef VALIDATE_VALUE
#else
	#define _ValidateValues(...)
#endif
}