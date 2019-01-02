#pragma once

#include "MatrixMathTypes.hpp"
#include "_MathValidation.hpp"

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

    static constexpr f32 DegToRadF32Const = MathPi<f32>() / 180.0f;
    static constexpr f64 DegToRadF64Const = MathPi<f64>() / 180.0;

	[[nodiscard]] constexpr inline f32 DegToRad(f32 deg)
	{
		_ValidateValues(deg);
		return deg * DegToRadF32Const;
	}

	[[nodiscard]] constexpr inline f64 DegToRad(f64 deg)
	{
		_ValidateValues(deg);
		return deg * DegToRadF64Const;
	}

    static constexpr f32 RadToDegF32Const = 180.0f / MathPi<f32>();
    static constexpr f64 RadToDegF64Const = 180.0 / MathPi<f64>();

	[[nodiscard]] constexpr inline f32 RadToDeg(f32 rad)
	{
		_ValidateValues(rad);
		return rad * RadToDegF32Const;
	}

	[[nodiscard]] constexpr inline f64 RadToDeg(f64 rad)
	{
		_ValidateValues(rad);
		return rad * RadToDegF64Const;
	}

	template <typename T> [[nodiscard]] constexpr T _RotationNormalize(T rot, T fullRot)
	{
		_ValidateValues(rot);

		if (rot >= 0 && rot < fullRot)
		{
			return rot;
		}
		if (rot < 0 && rot >= -fullRot)
		{
			return rot + fullRot;
		}
		if (rot >= fullRot && rot < fullRot * 2)
		{
			return rot - fullRot;
		}

		rot = (T)std::fmod(rot, fullRot);
		if (rot < 0)
		{
			rot += fullRot;
		}
		return rot;
	}
	
	[[nodiscard]] constexpr inline f32 RadNormalize(f32 rad)
	{
		return _RotationNormalize<f32>(rad, MathPiDouble<f32>());
	}

	[[nodiscard]] constexpr inline f64 RadNormalize(f64 rad)
	{
		return _RotationNormalize<f64>(rad, MathPiDouble<f64>());
	}

    [[nodiscard]] constexpr inline f32 DegNormalize(f32 deg)
    {
        return _RotationNormalize<f32>(deg, 360.0f);
    }

    [[nodiscard]] constexpr inline f64 DegNormalize(f64 def)
    {
        return _RotationNormalize<f64>(def, 360.0);
    }

	template <typename T> [[nodiscard]] constexpr inline T _RotationNormalizeClose(T rot, T fullRot)
	{
		_ValidateValues(rot);

		if (rot < 0)
		{
			ASSUME(rot + fullRot >= 0);
			return rot + fullRot;
		}
		if (rot >= fullRot)
		{
			ASSUME(rot - fullRot < fullRot);
			return rot - fullRot;
		}

		return rot;
	}

	[[nodiscard]] constexpr inline f32 RadNormalizeClose(f32 rad)
	{
		return _RotationNormalizeClose(rad, MathPiDouble<f32>());
	}

	[[nodiscard]] constexpr inline f64 RadNormalizeClose(f64 rad)
	{
		return _RotationNormalizeClose(rad, MathPiDouble<f64>());
	}

    [[nodiscard]] constexpr inline f32 DegNormalizeClose(f32 deg)
    {
        return _RotationNormalizeClose(deg, 360.0f);
    }

    [[nodiscard]] constexpr inline f64 DegNormalizeClose(f64 deg)
    {
        return _RotationNormalizeClose(deg, 360.0);
    }

    template <typename T> [[nodiscard]] constexpr inline T _RotationDistance(T rot0, T rot1, T fullRot, T epsilon)
    {
        ASSUME(rot0 >= -epsilon && rot0 < fullRot + epsilon);
        ASSUME(rot1 >= -epsilon && rot1 < fullRot + epsilon);

        _ValidateValues(rot0, rot1);

        if (rot0 < rot1)
        {
            std::swap(rot0, rot1);
        }
        T dist = rot0 - rot1;
        if (dist >= fullRot)
        {
            dist -= fullRot;
        }
        return dist;
    }

    [[nodiscard]] constexpr inline f32 RadDistance(f32 rad0, f32 rad1)
    {
        return _RotationDistance(rad0, rad1, MathPiDouble<f32>(), DefaultF32Epsilon);
    }

    [[nodiscard]] constexpr inline f64 RadDistance(f64 rad0, f64 rad1)
    {
        return _RotationDistance(rad0, rad1, MathPiDouble<f64>(), (f64)DefaultF32Epsilon);
    }

    [[nodiscard]] constexpr inline f32 DegDistance(f32 deg0, f32 deg1)
    {
        return _RotationDistance(deg0, deg1, 360.0f, DefaultF32Epsilon * 10.0f);
    }

    [[nodiscard]] constexpr inline f64 DegDistance(f64 deg0, f64 deg1)
    {
        return _RotationDistance(deg0, deg1, 360.0, DefaultF32Epsilon * 10.0);
    }

	[[nodiscard]] inline f32 DistanceSquare(f32 left, f32 right)
	{
		f32 delta = left - right;
		f32 deltaSquare = delta * delta;
		_ValidateValues(left, right, delta);
		return deltaSquare;
	}

	[[nodiscard]] inline f64 DistanceSquare(f64 left, f64 right)
	{
		f64 delta = left - right;
		f64 deltaSquare = delta * delta;
		_ValidateValues(left, right, delta);
		return deltaSquare;
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
		f32 diff = std::abs(left - right);
		_ValidateValues(left, right, diff);
        return diff;
	}

	[[nodiscard]] inline f64 Distance(f64 left, f64 right)
	{
		f64 diff = std::abs(left - right);
		_ValidateValues(left, right, diff);
        return diff;
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
		f32 interpolated = left + ((right - left) * interpolant);
		_ValidateValues(left, right, interpolated);
		return interpolated;
	}

	[[nodiscard]] inline f64 Lerp(f64 left, f64 right, f64 interpolant)
	{
		ASSUME(interpolant >= -DefaultF32Epsilon && interpolant <= 1 + DefaultF32Epsilon);
		f64 interpolated = left + ((right - left) * interpolant);
		_ValidateValues(left, right, interpolated);
		return interpolated;
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
		_ValidateValues(left, right, epsilon);
        return abs(left - right) <= epsilon;
    }

	[[nodiscard]] inline bool EqualsWithEpsilon(f64 left, f64 right, f64 epsilon = (f64)DefaultF32Epsilon)
	{
		_ValidateValues(left, right, epsilon);
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

    [[nodiscard]] inline f32 HorizontalFOVToVertical(f32 rad, f32 aspectRatio)
    {
        ASSUME(rad > 0 && aspectRatio > 0);
        f32 vertical = 2.0f * atan(tan(rad * 0.5f) / aspectRatio);
        _ValidateValues(rad, aspectRatio, vertical);
        return vertical;
    }

    [[nodiscard]] inline f32 VerticalFOVToHorizontal(f32 rad, f32 aspectRatio)
    {
        ASSUME(rad > 0 && aspectRatio > 0);
        f32 horizontal = 2.0f * atan(tan(rad * 0.5f) * aspectRatio);
        _ValidateValues(rad, aspectRatio, horizontal);
        return horizontal;
    }
}