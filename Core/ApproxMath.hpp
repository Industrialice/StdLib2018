#pragma once

#include "CoreHeader.hpp"

namespace StdLib::ApproxMath
{
    enum class Precision { Low, Medium, High };

	// based on https://www.hackersdelight.org/hdcodetxt/rsqrt.c.txt
    template <Precision precision> [[nodiscard]] inline f32 RSqrt(f32 input)
    {
		ASSUME(input >= 0);

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
            /* This is the same as Precision::Medium, but with an additional step
			of the Newton iteration, for increased accuracy. The constant
			0x5f37599e makes the relative error range from 0 to -0.00000463.
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

    template <Precision precision> [[nodiscard]] inline f64 RSqrt(f64 input)
    {
        return 1.0 / sqrt(input);
    }

	// based on https://www.hackersdelight.org/hdcodetxt/asqrt.c.txt
    template <Precision precision> [[nodiscard]] inline f32 Sqrt(f32 input)
    {
		if constexpr (precision == Precision::Low)
		{
			/* This is a very approximate but very fast version of asqrt. It is just
			two integer instructions (shift right and add), plus instructions
			to load the constant.
			The constant 0x1fbb4f2e balances the relative error at +-0.0347474. */

			union { i32 ix; f32 x; };

			x = input;
			ix = 0x1fbb4f2e + (ix >> 1); // Initial guess.
			return x;
		}
		else if constexpr (precision == Precision::Medium)
		{
			/* This is a novel and fast routine for the approximate square root of
			an IEEE float (single precision). It is derived from a similar program
			for the approximate reciprocal square root.
			The relative error ranges from 0 to +0.0006011.
			Caution:
				Result for -0 is -1.35039e+19 (unreasonable).
				Result for 0 is 3.96845e-20.
			For denorms it is either within tolerance or gives a result < 1.0e-19.
			Gives the correct result (inf) for x = inf.
			Gives the correct result (NaN) for x = NaN. */

			union { i32 ix; f32 x; };

			x = input;
			ix = 0x1fbb67a8 + (ix >> 1); // Initial guess.
			x = 0.5f*(x + input / x); // Newton step.
			return x;
		}
		else
		{
			/* This is the same as Precision::Medium, but with an additional step
			of the Newton iteration, for increased accuracy.
			The relative error ranges from 0 to +0.00000023. */

			union { i32 ix; f32 x; };

			x = input;
			ix = 0x1fbb67a8 + (ix >> 1); // Initial guess.
			x = 0.5f*(x + input / x); // Newton step.
			x = 0.5f*(x + input / x); // Newton step again.
			return x;
		}
    }

    template <Precision precision> [[nodiscard]] inline f64 Sqrt(f64 input)
    {
        return sqrt(input);
    }

	// qube root, based on https://www.hackersdelight.org/hdcodetxt/acbrt.c.txt
	template <Precision precision> [[nodiscard]] inline f32 Qurt(f32 input)
	{
		ASSUME(input >= 0);

		if constexpr (precision == Precision::Low)
		{
			/* This is a very approximate but very fast version of acbrt. It is just
			two integer instructions (shift right and divide), plus instructions
			to load the constant.
			The constant 0x2a51067f balances the relative error at +-0.0316. */

			union { i32 ix; f32 x; };

			x = input;
			ix = 0x2a51067f + ix / 3; // Initial guess.
			return x;
		}
		else if constexpr (precision == Precision::Medium)
		{
			/* This is a novel and fast routine for the approximate cube root of
			an IEEE float (single precision). It is derived from a similar program
			for the approximate square root.
			The relative error ranges from 0 to +0.00103.
			Caution:
				Result for -0 is NaN.
				Result for 0 is 1.24e-13.
			For denorms it is either within tolerance or gives a result < 2.1e-13.
			Gives the correct result (inf) for x = inf.
			Gives the correct result (NaN) for x = NaN. */

			union { i32 ix; f32 x; };

			x = input;
			ix = ix / 4 + ix / 16; // Approximate divide by 3.
			ix = ix + ix / 16;
			ix = ix + ix / 256;
			ix = 0x2a5137a0 + ix; // Initial guess.
			x = 0.33333333f * (2.0f * x + input / (x * x)); // Newton step.
			return x;
		}
		else
		{
			/* This is the same as Precision::Medium, but with an additional step
			of the Newton iteration, for increased accuracy.
			The relative error ranges from 0 to +0.00000116. */

			union { i32 ix; f32 x; };

			x = input;
			ix = ix / 4 + ix / 16; // Approximate divide by 3.
			ix = ix + ix / 16;
			ix = ix + ix / 256;
			ix = 0x2a5137a0 + ix; // Initial guess.
			x = 0.33333333f * (2.0f * x + input / (x * x)); // Newton step.
			x = 0.33333333f * (2.0f * x + input / (x * x)); // Newton step again.
			return x;
		}
	}

	template <Precision precision> [[nodiscard]] inline f64 Qurt(f64 input)
	{
		return pow(input, 1.0 / 3.0);
	}
}