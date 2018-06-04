#pragma once

#include "CoreHeader.hpp"

namespace StdLib::ApproxMath
{
    enum class Precision { Low, Medium, High };

    template <Precision precision> [[nodiscard]] f32 RSqrt(f32 input)
    {
        if constexpr (precision == Precision::Low)
        {
            /* The constant 0x5f37642f balances the relative error at +-0.034213.
            The constant 0x5f30c7f0 makes the relative error range from 0 to
            -0.061322.
            The constant 0x5f400000 makes the relative error range from 0 to
            +0.088662. */

            union { i32 ix; f32 x; };

            x = input;
            ix = 0x5f37642f - (ix >> 1); // Initial guess.
            return x;
        }
        else if constexpr (precision == Precision::Medium)
        {
            /* Notes: For more accuracy, repeat the Newton step (just duplicate the
            line). The routine always gets the result too low. According to Chris
            Lomont, the relative error is at most -0.00175228.
            Therefore, to cut its relative error in half, making it approximately
            plus or minus 0.000876, change the 1.5f in the Newton step to 1.500876f
            (1.5008908 works best for me, rel err is +-0.0008911).
            Chris says that changing the hex constant to 0x5f375a86 reduces the
            maximum relative error slightly, to 0.00175124. (I get 0.00175128. But
            the best I can do is use 5f375a82, which gives rel err = 0 to
            -0.00175123). However, using that value seems to usually give a slightly
            larger relative error, according to Chris.
            The routine can be adapted to IEEE double precision. */

            union { i32 ix; f32 x; };

            x = input;
            f32 xhalf = 0.5f * x;
            ix = 0x5f375a82 - (ix >> 1); // Initial guess.
            x = x * (1.5008908f - xhalf * x * x); // Newton step.
            return x;
        }
        else
        {
            /* This is rsqrt with an additional step of the Newton iteration, for
            increased accuracy. The constant 0x5f37599e makes the relative error
            range from 0 to -0.00000463.
            You can't balance the error by adjusting the constant. */

            f32 xhalf = 0.5f * input;
            i32 i = *(i32 *)&input;
            i = 0x5f37599e - (i >> 1); // Initial guess.
            input = *(f32 *)&i;
            input = input * (1.5f - xhalf * input * input); // Newton step.
            input = input * (1.5f - xhalf * input * input); // Newton step again.
            return input;
        }
    }

    template <Precision precision>[[nodiscard]] f32 Sqrt(f32 input)
    {
        return input * RSqrt<precision>(input);
    }
}