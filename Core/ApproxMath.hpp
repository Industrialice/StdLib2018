#pragma once

namespace StdLib::ApproxMath
{
    enum class Precision { Low, Medium, High };

	// based on https://www.hackersdelight.org/hdcodetxt/rsqrt.c.txt
	// TODO: type punning using unions is UB in C++
    template <Precision precision> [[nodiscard]] inline f32 RSqrt(f32 input)
    {
		ASSUME(input >= 0);

        if constexpr (precision == Precision::Low)
        {
            /* The constant 0x5f37642f balances the relative error at +-0.034213.
            The constant 0x5f30c7f0 makes the relative error range from 0 to
            -0.061322.
            The constant 0x5f400000 makes the relative error range from 0 to
            +0.088662.
			Measured error relative to 1.0f / sqrt on range 
			[0.05; 10000] is [-0.141699; 0.103287]
			MSVC 15.9.12 2500k x64: 389.12kk 2.39x
			Clang 8.0.2 MT6589: 57.60kk 2.46x 
			Clang 8.0.2 MT8735M ARM64: 47.45kk 2.06x */

			ASSUME(input > 0.0f);

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
            The routine can be adapted to IEEE double precision.
			Measured error relative to 1.0f / sqrt on range 
			[0.05; 10000] is [-0.003691; 0.003884]
			MSVC 15.9.12 2500k x64: 342.06kk 2.09x
			Clang 8.0.2 MT6589: 31.39kk 1.34x 
			Clang 8.0.2 MT8735M ARM64: 24.38kk 1.06x */

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
            You can't balance the error by adjusting the constant.
			Measured error relative to 1.0f / sqrt on range 
			[0.05; 10000] is [-0.000020; 0.000001]
			MSVC 15.9.12 2500k x64: 251.82kk 1.54x
			Clang 8.0.2 MT6589: 22.08kk 0.94x 
			Clang 8.0.2 MT8735M ARM64: 17.36kk 0.75x */

			union { i32 i; f32 x; };

            f32 xhalf = 0.5f * input;
            x = input;
            i = 0x5f37599e - (i >> 1); // Initial guess.
            x = x * (1.5f - xhalf * x * x); // Newton step.
            x = x * (1.5f - xhalf * x * x); // Newton step again.
            return x;
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
			The constant 0x1fbb4f2e balances the relative error at +-0.0347474.
			Measured error relative to sqrt on range [0.00001; 2pi] is [-0.07199645; 0.04914033]
			MSVC 15.9.12 2500k x64: 405.29kk 3.88x
			Clang 8.0.2 MT6589: 56.36kk 4.83x 
			Clang 8.0.2 MT8735M ARM64: 47.77kk 3.54x */

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
			Gives the correct result (NaN) for x = NaN.
			Measured error relative to sqrt on range [0.00001; 2pi] is [-0.00000012; 0.00124454]
			MSVC 15.9.12 2500k x64: 320.82kk 3.08x
			Clang 8.0.2 MT6589: 24.18kk 2.07x 
			Clang 8.0.2 MT8735M ARM64: 23.06kk 1.71x */

			union { i32 ix; f32 x; };

			x = input;
			ix = 0x1fbb67a8 + (ix >> 1); // Initial guess.
			x = 0.5f * (x + input / x); // Newton step.
			return x;
		}
		else
		{
			/* This is the same as Precision::Medium, but with an additional step
			of the Newton iteration, for increased accuracy.
			The relative error ranges from 0 to +0.00000023.
			Measured error relative to sqrt on range [0.00001; 2pi] is [-0.00000024; 0.00000048]
			MSVC 15.9.12 2500k x64: 165.93kk 1.58x
			Clang 8.0.2 MT6589: 15.86kk 1.36x 
			Clang 8.0.2 MT8735M ARM64: 15.20kk 1.13x */

			union { i32 ix; f32 x; };

			x = input;
			ix = 0x1fbb67a8 + (ix >> 1); // Initial guess.
			x = 0.5f * (x + input / x); // Newton step.
			x = 0.5f * (x + input / x); // Newton step again.
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
			The constant 0x2a51067f balances the relative error at +-0.0316.
			Measured error relative to pow(value, 1.0f / 3.0f) on range
			[0.00001; pi] is [-0.03258288; 0.04560971]
			MSVC 15.9.12 2500k x64: 356.08kk 5.29x
			Clang 8.0.2 MT6589: 48.03kk 19.31x 
			Clang 8.0.2 MT8735M ARM64: 37.74kk 15.60x */

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
			Gives the correct result (NaN) for x = NaN.
			Measured error relative to pow(value, 1.0f / 3.0f) on range
			[0.00001; pi] is [-0.00000012; 0.00130832]
			MSVC 15.9.12 2500k x64: 187.49kk 2.81x
			Clang 8.0.2 MT6589: 16.91kk 6.80x 
			Clang 8.0.2 MT8735M ARM64: 15.79kk 6.53x */

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
			The relative error ranges from 0 to +0.00000116.
			Measured error relative to pow(value, 1.0f / 3.0f) on range
			[0.00001; pi] is [-0.00000024; 0.00000143]
			MSVC 15.9.12 2500k x64: 127.52kk 1.89x
			Clang 8.0.2 MT6589: 11.47kk 4.61x 
			Clang 8.0.2 MT8735M ARM64: 11.01kk 4.55x */

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

	template <Precision precision> [[nodiscard]] inline f32 Cos(f32 value)
	{
		// based on GLM, relative to std::cos precision on range 
		// [-0.5pi-0.00001;2.5pi+0.00001] is [-0.000007;0.000007]
		// MSVC 15.9.12 2500k x64: 271.64kk 2.48x
		// Clang 8.0.2 MT6589: 19.53kk 1.63x 
		// Clang 8.0.2 MT8735M ARM64: 19.00kk 1.40x

		ASSUME(value >= -MathPiHalf() - 0.00001f && value <= MathPiDouble() + MathPiHalf() + 0.00001f);

        #define COS52S(v) \
            const f32 x = v; \
            const f32 xx = x * x; \
            f32 r = (0.9999932946f + xx * (-0.4999124376f + xx * (0.0414877472f + xx * -0.0012712095f)));

        if (value < MathPiHalf())
        {
            COS52S(value);
            return r;
        }
        if (value < MathPi())
        {
            COS52S(MathPi() - value);
            return -r;
        }
        if (value < (3.0f * MathPiHalf()))
        {
            COS52S(value - MathPi());
            return -r;
        }

        COS52S(MathPiDouble() - value);
        #undef COS52S
		return r;
	}

	template <Precision precision> [[nodiscard]] inline f64 Cos(f64 value)
	{
		return std::cos(value);
	}

	template <Precision precision> [[nodiscard]] inline f32 Sin(f32 value)
	{
		// based on GLM, relative to std::sin precision on range 
		// [-0.00001;3pi+0.00001] is [-0.000007;0.000007]
		// MSVC 15.9.12 2500k x64: 236.08kk 2.19x
		// Clang 8.0.2 MT6589: 17.69kk 1.43x
		// Clang 8.0.2 MT8735M ARM64: 15.79kk 1.18x
		ASSUME(value >= -0.00001f && value <= MathPiDouble() + MathPi() + 0.00001f);
		return Cos<precision>((MathPiDouble() + MathPiHalf()) - value);
	}

	template <Precision precision> [[nodiscard]] inline f64 Sin(f64 value)
	{
		return std::sin(value);
	}
}