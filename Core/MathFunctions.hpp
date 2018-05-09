#pragma once

#include "MatrixMathTypes.hpp"

namespace StdLib
{
    template <typename T> constexpr inline T MathPi()
    {
        constexpr T pi = T(3.14159265358979323846);
        return pi;
    }

    template <typename T> constexpr inline T MathPiHalf()
    {
        constexpr T pi = T(1.57079632679489661923);
        return pi;
    }

    template <typename T> constexpr inline T MathPiQuarter()
    {
        constexpr T pi = T(0.785398163397448309616);
        return pi;
    }

    constexpr inline f32 DegToRad(f32 deg)
    {
        return deg * (MathPi<f32>() / 180.0f);
    }

    constexpr inline f64 DegToRad(f64 deg)
    {
        return deg * (MathPi<f64>() / 180.0);
    }

    constexpr inline f32 RadToDeg(f32 rad)
    {
        return rad * (180.0f / MathPi<f32>());
    }

    constexpr inline f64 RadToDeg(f64 rad)
    {
        return rad * (180.0 / MathPi<f64>());
    }

    template <typename T> constexpr inline T RadNormalize(T rad)
    {
        constexpr T pi2 = MathPi<T>() * 2;

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

        rad = (T)std::fmod(rad, 1) * pi2;
        if (rad < 0)
        {
            rad += pi2;
        }

        return rad;
    }

    template <typename T> constexpr inline T RadNormalizeFast(T rad)
    {
        constexpr T pi2 = MathPi<T>() * 2;

        if (rad < 0)
        {
            ASSUME(rad + pi2 >= 0);
            return rad + pi2;
        }
        if (rad > pi2)
        {
            ASSUME(rad - pi2 < pi2);
            return rad - pi2;
        }

        return rad;
    }

    inline f32 DistanceSquare(f32 left, f32 right)
    {
        f32 delta = left - right;
        return delta * delta;
    }

    inline f32 DistanceSquare(Vector2 left, Vector2 right)
    {
        return DistanceSquare(left.x, right.x) + DistanceSquare(left.y, right.y);
    }

    inline f32 DistanceSquare(Vector3 left, Vector3 right)
    {
        return DistanceSquare(left.ToVector2(), right.ToVector2()) + DistanceSquare(left.z, right.z);
    }

    inline f32 DistanceSquare(Vector4 left, Vector4 right)
    {
        return DistanceSquare(left.ToVector3(), right.ToVector3()) + DistanceSquare(left.w, right.w);
    }

    inline f32 Distance(f32 left, f32 right)
    {
        return std::abs(left - right);
    }

    inline f32 Distance(Vector2 left, Vector2 right)
    {
        return std::sqrt(DistanceSquare(left, right));
    }

    inline f32 Distance(Vector3 left, Vector3 right)
    {
        return std::sqrt(DistanceSquare(left, right));
    }

    inline f32 Distance(Vector4 left, Vector4 right)
    {
        return std::sqrt(DistanceSquare(left, right));
    }

    inline f32 Lerp(f32 left, f32 right, f32 interpolant)
    {
        ASSUME(interpolant >= -DefaultF32Epsilon && interpolant <= 1 + DefaultF32Epsilon);
        return left + ((right - left) * interpolant);
    }

    inline Vector2 Lerp(Vector2 left, Vector2 right, f32 interpolant)
    {
        return {Lerp(left.x, right.x, interpolant), Lerp(left.y, right.y, interpolant)};
    }

    inline Vector3 Lerp(Vector3 left, Vector3 right, f32 interpolant)
    {
        return {Lerp(left.x, right.x, interpolant), Lerp(left.y, right.y, interpolant), Lerp(left.z, right.z, interpolant)};
    }

    inline Vector4 Lerp(Vector4 left, Vector4 right, f32 interpolant)
    {
        return {Lerp(left.x, right.x, interpolant), Lerp(left.y, right.y, interpolant), Lerp(left.z, right.z, interpolant), Lerp(left.w, right.w, interpolant)};
    }

    inline bool EqualsWithEpsilon(f32 left, f32 right, f32 epsilon = DefaultF32Epsilon)
    {
        return abs(left - right) < epsilon;
    }

    inline bool EqualsWithEpsilon(Vector2 left, Vector2 right, f32 epsilon = DefaultF32Epsilon)
    {
        return EqualsWithEpsilon(left.x, right.x, epsilon) && EqualsWithEpsilon(left.y, right.y, epsilon);
    }

    inline bool EqualsWithEpsilon(Vector3 left, Vector3 right, f32 epsilon = DefaultF32Epsilon)
    {
        return EqualsWithEpsilon(left.x, right.x, epsilon) && EqualsWithEpsilon(left.y, right.y, epsilon) && EqualsWithEpsilon(left.z, right.z, epsilon);
    }

    inline bool EqualsWithEpsilon(Vector4 left, Vector4 right, f32 epsilon = DefaultF32Epsilon)
    {
        return EqualsWithEpsilon(left.x, right.x, epsilon) && EqualsWithEpsilon(left.y, right.y, epsilon) && EqualsWithEpsilon(left.z, right.z, epsilon) && EqualsWithEpsilon(left.w, right.w, epsilon);
    }
}