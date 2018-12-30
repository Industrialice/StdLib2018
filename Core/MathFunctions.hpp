#pragma once

#include "MatrixMathTypes.hpp"

namespace StdLib
{
	template <typename T> [[nodiscard]] constexpr inline T MathPi()
	{
		if constexpr (std::is_same_v<T, f32>) return 3.14159265358979323846f;
		else if constexpr (std::is_same_v<T, f64>) return 3.14159265358979323846;
	}

	template <typename T> [[nodiscard]] constexpr inline T MathPiHalf()
	{
		if constexpr (std::is_same_v<T, f32>) return 1.57079632679489661923f;
		else if constexpr (std::is_same_v<T, f64>) return 1.57079632679489661923;
	}

	template <typename T> [[nodiscard]] constexpr inline T MathPiQuarter()
	{
		if constexpr (std::is_same_v<T, f32>) return 0.785398163397448309616f;
		else if constexpr (std::is_same_v<T, f64>) return 0.785398163397448309616;
	}

	template <typename T> [[nodiscard]] constexpr inline T MathPiDouble()
	{
		if constexpr (std::is_same_v<T, f32>) return 6.283185307179586476925f;
		else if constexpr (std::is_same_v<T, f64>) return 6.283185307179586476925;
	}

	[[nodiscard]] constexpr inline f32 DegToRad(f32 deg)
	{
		return deg * (MathPi<f32>() / 180.0f);
	}

	[[nodiscard]] constexpr inline f64 DegToRad(f64 deg)
	{
		return deg * (MathPi<f64>() / 180.0);
	}

	[[nodiscard]] constexpr inline f32 RadToDeg(f32 rad)
	{
		return rad * (180.0f / MathPi<f32>());
	}

	[[nodiscard]] constexpr inline f64 RadToDeg(f64 rad)
	{
		return rad * (180.0 / MathPi<f64>());
	}

	template <typename T> [[nodiscard]] constexpr inline T _RadNormalize(T rad)
	{
		constexpr T pi2 = MathPiDouble<T>();

		if (rad >= 0 && rad < pi2)
		{
			return rad;
		}
		if (rad < 0 && rad >= -pi2)
		{
			return rad + pi2;
		}
		if (rad >= pi2 && rad < pi2 * 2)
		{
			return rad - pi2;
		}

		rad = (T)std::fmod(rad, pi2);
		if (rad < 0)
		{
			rad += pi2;
		}
		return rad;
	}
	
	[[nodiscard]] constexpr inline f32 RadNormalize(f32 rad)
	{
		return _RadNormalize(rad);
	}

	[[nodiscard]] constexpr inline f64 RadNormalize(f64 rad)
	{
		return _RadNormalize(rad);
	}

	template <typename T> [[nodiscard]] constexpr inline T _RadNormalizeClose(T rad)
	{
		constexpr T pi2 = MathPiDouble<T>();

		if (rad < 0)
		{
			ASSUME(rad + pi2 >= 0);
			return rad + pi2;
		}
		if (rad >= pi2)
		{
			ASSUME(rad - pi2 < pi2);
			return rad - pi2;
		}

		return rad;
	}

	[[nodiscard]] constexpr inline f32 RadNormalizeClose(f32 rad)
	{
		return _RadNormalizeClose(rad);
	}

	[[nodiscard]] constexpr inline f64 RadNormalizeClose(f64 rad)
	{
		return _RadNormalizeClose(rad);
	}

	[[nodiscard]] inline f32 DistanceSquare(f32 left, f32 right)
	{
		f32 delta = left - right;
		return delta * delta;
	}

	[[nodiscard]] inline f64 DistanceSquare(f64 left, f64 right)
	{
		f64 delta = left - right;
		return delta * delta;
	}

	[[nodiscard]] inline f32 DistanceSquare(Vector2 left, Vector2 right)
	{
		return DistanceSquare(left.x, right.x) + DistanceSquare(left.y, right.y);
	}

	[[nodiscard]] inline f32 DistanceSquare(Vector3 left, Vector3 right)
	{
		return DistanceSquare(left.ToVector2(), right.ToVector2()) + DistanceSquare(left.z, right.z);
	}

	[[nodiscard]] inline f32 DistanceSquare(Vector4 left, Vector4 right)
	{
		return DistanceSquare(left.ToVector3(), right.ToVector3()) + DistanceSquare(left.w, right.w);
	}

	[[nodiscard]] inline f32 Distance(f32 left, f32 right)
	{
		return std::abs(left - right);
	}

	[[nodiscard]] inline f64 Distance(f64 left, f64 right)
	{
		return std::abs(left - right);
	}

	[[nodiscard]] inline f32 Distance(Vector2 left, Vector2 right)
	{
		return std::sqrt(DistanceSquare(left, right));
	}

	[[nodiscard]] inline f32 Distance(Vector3 left, Vector3 right)
	{
		return std::sqrt(DistanceSquare(left, right));
	}

	[[nodiscard]] inline f32 Distance(Vector4 left, Vector4 right)
	{
		return std::sqrt(DistanceSquare(left, right));
	}

	[[nodiscard]] inline f32 Lerp(f32 left, f32 right, f32 interpolant)
	{
		ASSUME(interpolant >= -DefaultF32Epsilon && interpolant <= 1 + DefaultF32Epsilon);
		return left + ((right - left) * interpolant);
	}

	[[nodiscard]] inline f64 Lerp(f64 left, f64 right, f64 interpolant)
	{
		ASSUME(interpolant >= -DefaultF32Epsilon && interpolant <= 1 + DefaultF32Epsilon);
		return left + ((right - left) * interpolant);
	}

	[[nodiscard]] inline Vector2 Lerp(Vector2 left, Vector2 right, f32 interpolant)
	{
		return {Lerp(left.x, right.x, interpolant), Lerp(left.y, right.y, interpolant)};
	}

	[[nodiscard]] inline Vector3 Lerp(Vector3 left, Vector3 right, f32 interpolant)
	{
		return {Lerp(left.x, right.x, interpolant), Lerp(left.y, right.y, interpolant), Lerp(left.z, right.z, interpolant)};
	}

	[[nodiscard]] inline Vector4 Lerp(Vector4 left, Vector4 right, f32 interpolant)
	{
		return {Lerp(left.x, right.x, interpolant), Lerp(left.y, right.y, interpolant), Lerp(left.z, right.z, interpolant), Lerp(left.w, right.w, interpolant)};
	}

	[[nodiscard]] inline bool EqualsWithEpsilon(f32 left, f32 right, f32 epsilon = DefaultF32Epsilon)
	{
        return abs(left - right) <= epsilon;
    }

	[[nodiscard]] inline bool EqualsWithEpsilon(f64 left, f64 right, f64 epsilon = (f64)DefaultF32Epsilon)
	{
		return abs(left - right) <= epsilon;
	}

    [[nodiscard]] inline bool EqualsWithEpsilon(Vector2 left, Vector2 right, f32 epsilon = DefaultF32Epsilon)
    {
        return EqualsWithEpsilon(left.x, right.x, epsilon) && EqualsWithEpsilon(left.y, right.y, epsilon);
    }

    [[nodiscard]] inline bool EqualsWithEpsilon(Vector3 left, Vector3 right, f32 epsilon = DefaultF32Epsilon)
    {
        return EqualsWithEpsilon(left.x, right.x, epsilon) && EqualsWithEpsilon(left.y, right.y, epsilon) && EqualsWithEpsilon(left.z, right.z, epsilon);
    }

    [[nodiscard]] inline bool EqualsWithEpsilon(Vector4 left, Vector4 right, f32 epsilon = DefaultF32Epsilon)
    {
        return EqualsWithEpsilon(left.x, right.x, epsilon) && EqualsWithEpsilon(left.y, right.y, epsilon) && EqualsWithEpsilon(left.z, right.z, epsilon) && EqualsWithEpsilon(left.w, right.w, epsilon);
    }
}