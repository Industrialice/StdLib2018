#pragma once

namespace StdLib::ApproxMath
{
    enum class Precision { Low, Medium, High };

	// http://nnp.ucsd.edu/PHY120B/student_dirs/shared/gnarly/misc_docs/trig_approximations.pdf
	template <Precision precision> [[nodiscard]] inline f32 Cos(f32 value)
	{
		/* High precision:
		During [-1.5708;7.8540]
		Absolute [-0.00000693;+0.00000700] At [4.71240;7.73048] Average absolute [-0.00000649;+0.00000007]
		Relative [-0.06675720;+0.06675720] At [0.00010;-0.00010]
		MSVC 15.9.12 2500k x64: 271.64kk 2.48x
		MSVC  16.1.1 LX800 x86: 1.79kk 1.27x
		Clang 8.0.2 MT6589: 19.53kk 1.63x
		Clang 8.0.2 MT8735M ARM64: 19.00kk 1.40x

		Medium precision:
		During [-1.5708;7.8540]
		Absolute [-0.00059701;+0.00059722] At [4.71239;7.85399] Average absolute [-0.00058156;+0.00000623]
		Relative [-5.96940489;+5.96940489] At [0.00010;-0.00010]
		MSVC 15.9.12 2500k x64: 297.52kk 2.78x */

		ASSUME(value >= -MathPiHalf() - 0.00001f && value <= MathPiDouble() + MathPiHalf() + 0.00001f);

		struct Compute
		{
			static FORCEINLINE constexpr f32 Get(f32 x) // operates on [0;pi/2]
			{
				const f32 xx = x * x;
				if constexpr (precision == Precision::High)
				{
					return (0.9999932946f + xx * (-0.4999124376f + xx * (0.0414877472f + xx * -0.0012712095f)));
				}
				else
				{
					return (0.99940307f + xx * (-0.49558072f + 0.03679168f * xx));
				}
			}
		};

		if (value < MathPiHalf())
		{
			return Compute::Get(value);
		}
		if (value < MathPi())
		{
			return -Compute::Get(MathPi() - value);
		}
		if (value < (3.0f * MathPiHalf()))
		{
			return -Compute::Get(value - MathPi());
		}

		return Compute::Get(MathPiDouble() - value);
	}

	template <Precision precision> [[nodiscard]] inline f64 Cos(f64 value)
	{
		return std::cos(value);
	}

	template <Precision precision> [[nodiscard]] inline f32 Sin(f32 value)
	{
		/* High precision:
		During [-0.0000;9.4248]
		Absolute [-0.00000715;+0.00000724] At [-0.00001;3.14173] Average absolute [-0.00000658;+0.00000009]
		Relative [-0.07033090;+0.00007286] At [0.00010;0.07230]
		MSVC 15.9.12 2500k x64: 236.08kk 2.19x
		MSVC  16.1.1 LX800 x86: 1.57kk 0.86x
		Clang 8.0.2 MT6589: 17.69kk 1.43x
		Clang 8.0.2 MT8735M ARM64: 15.79kk 1.18x

		Medium precision:
		During [-0.0000;9.4248]
		Absolute [-0.00059721;+0.00059738] At [0.21272;9.42479] Average absolute [-0.00000757;+0.00057822]
		Relative [-0.00351660;+5.95526567] At [0.13283;0.00010]
		MSVC 15.9.12 2500k x64: 259.79kk 2.31x */
		ASSUME(value >= -0.00001f && value <= MathPiDouble() + MathPi() + 0.00001f);
		return Cos<precision>((MathPiDouble() + MathPiHalf()) - value);
	}

	template <Precision precision> [[nodiscard]] inline f64 Sin(f64 value)
	{
		return std::sin(value);
	}

	// based on https://www.hackersdelight.org/hdcodetxt/asqrt.c.txt
    template <Precision precision> [[nodiscard]] inline f32 Sqrt(f32 input)
    {
		union { i32 ix; f32 x; };
		x = input;

		if constexpr (precision == Precision::Low)
		{
			/* This is a very approximate but very fast version of asqrt. It is just
			two integer instructions (shift right and add), plus instructions
			to load the constant.
			The constant 0x1fbb4f2e balances the relative error at +-0.0347474.
			During [0.0000;6.2832]
			Absolute [-0.07199645;+0.04914033] At [4.29317;2.00000] Average absolute [-0.00467490;+0.00164097]
			Relative [-2.14656018;+3.14498176] At [0.00026;0.00012]
			MSVC  16.1.1 2500k x64: 407.92kk 1.23x
			MSVC  16.1.1 LX800 x86: 4.25kk
			Clang 8.0.2 MT6589: 56.36kk
			Clang 8.0.2 MT8735M ARM64: 47.77kk */

			ix = 0x1fbb4f2e + (ix >> 1); // Initial guess.
		}
		else
		{
			/* This is a novel and fast routine for the approximate square root of
			an IEEE float (single precision). It is derived from a similar program
			for the approximate reciprocal square root.
			Caution:
				Result for -0 is -1.35039e+19 (unreasonable).
				Result for 0 is 3.96845e-20.
			For denorms it is either within tolerance or gives a result < 1.0e-19.
			Gives the correct result (inf) for x = inf.
			Gives the correct result (NaN) for x = NaN.

			Medium precision:
			The relative error ranges from 0 to +0.0006011.
			During [0.0000;6.2832]
			Absolute [-0.00000012;+0.00124454] At [1.60514;4.28715] Average absolute [-0.00000000;+0.00006772]
			Relative [-0.00000582;+0.05439809] At [0.00016;0.00012]
			MSVC  16.1.1 2500k x64: 323.82kk 0.97x
			MSVC  16.1.1 LX800 x86: 2.08kk
			Clang 8.0.2 MT6589: 24.18kk
			Clang 8.0.2 MT8735M ARM64: 23.06kk
			
			High precision:
			The relative error ranges from 0 to +0.00000023.
			During [0.0000;6.2832]
			Absolute [-0.00000024;+0.00000048] At [4.00237;4.18436] Average absolute [-0.00000000;+0.00000003]
			Relative [-0.00000931;+0.00002310] At [0.00010;0.00012]
			MSVC  16.1.1 2500k x64: 165.93kk 0.50x
			MSVC  16.1.1 LX800 x86: 1.76kk
			Clang 8.0.2 MT6589: 15.86kk
			Clang 8.0.2 MT8735M ARM64: 15.20kk */

			ix = 0x1fbb67a8 + (ix >> 1); // Initial guess.
			x = 0.5f * (x + input / x); // Newton step.

			if constexpr (precision == Precision::High)
			{
				x = 0.5f * (x + input / x); // Newton step again.
			}
		}

		return x;
    }

    template <Precision precision> [[nodiscard]] inline f64 Sqrt(f64 input)
    {
        return sqrt(input);
    }

	// based on https://www.hackersdelight.org/hdcodetxt/rsqrt.c.txt
	// TODO: type punning using unions is UB in C++
	template <Precision precision> [[nodiscard]] inline f32 RSqrt(f32 input)
	{
		ASSUME(input >= 0);

		union { i32 ix; f32 x; };
		x = input;

		if constexpr (precision == Precision::Low)
		{
			/* The constant 0x5f37642f balances the relative error at +-0.034213.
			The constant 0x5f30c7f0 makes the relative error range from 0 to
			-0.061322.
			The constant 0x5f400000 makes the relative error range from 0 to
			+0.088662.
			During [0.0500;10000.0000]
			Absolute [-0.14169931;+0.10328674] At [0.05830;0.09917] Average absolute [-0.00465273;+0.01198563]
			Relative [-2.43066365;+1.17687224] At [0.05830;0.05000]
			MSVC 15.9.12 2500k x64: 389.12kk 2.39x
			MSVC  16.1.1 LX800 x86: 4.20kk 1.69x
			Clang 8.0.2 MT6589: 57.60kk 2.46x
			Clang 8.0.2 MT8735M ARM64: 47.45kk 2.06x */

			ASSUME(input > 0.0f);

			ix = 0x5f37642f - (ix >> 1); // Initial guess.
		}
		else
		{
			/* Notes: For more accuracy, repeat the Newton step (just duplicate the
			line). The routine always gets the result too low.

			Medium precision:
			According to Chris
			Lomont, the relative error is at most -0.00175228.
			Therefore, to cut its relative error in half, making it approximately
			plus or minus 0.000876, change the 1.5f in the Newton step to 1.500876f
			(1.5008908 works best for me, rel err is +-0.0008911).
			Chris says that changing the hex constant to 0x5f375a86 reduces the
			maximum relative error slightly, to 0.00175124. (I get 0.00175128. But
			the best I can do is use 5f375a82, which gives rel err = 0 to
			-0.00175123). However, using that value seems to usually give a slightly
			larger relative error, according to Chris.
			During [0.0500;10000.0000]
			Absolute [-0.00369120;+0.00388432] At [0.05828;0.05256] Average absolute [-0.00020234;+0.00019093]
			Relative [-0.06333765;+0.07403230] At [0.05828;0.05236]
			MSVC 15.9.12 2500k x64: 342.06kk 2.09x
			MSVC  16.1.1 LX800 x86: 2.64kk 1.06x
			Clang 8.0.2 MT6589: 31.39kk 1.34x
			Clang 8.0.2 MT8735M ARM64: 24.38kk 1.06x

			High precision:
			The constant 0x5f37599e makes the relative error range from 0 to -0.00000463.
			You can't balance the error by adjusting the constant.
			During [0.0500;10000.0000]
			Absolute [-0.00000572;+0.00000072] At [0.05196;0.06472] Average absolute [-0.00000039;+0.00000001]
			Relative [-0.00011013;+0.00001105] At [0.05196;0.06472]
			MSVC 15.9.12 2500k x64: 251.82kk 1.54x
			MSVC  16.1.1 LX800 x86: 1.93kk 0.78x
			Clang 8.0.2 MT6589: 22.08kk 0.94x
			Clang 8.0.2 MT8735M ARM64: 17.36kk 0.75x */

			f32 xhalf = 0.5f * x;
			ix = 0x5f375a82 - (ix >> 1); // Initial guess.
			x = x * (1.5008908f - xhalf * x * x); // Newton step.

			if constexpr (precision == Precision::High)
			{
				x = x * (1.5f - xhalf * x * x); // Newton step again.
			}
		}

		return x;
	}

	template <Precision precision> [[nodiscard]] inline f64 RSqrt(f64 input)
	{
		return 1.0 / sqrt(input);
	}

	// qube root, based on https://www.hackersdelight.org/hdcodetxt/acbrt.c.txt
	template <Precision precision> [[nodiscard]] inline f32 Qurt(f32 input)
	{
		ASSUME(input >= 0);

		union { i32 ix; f32 x; };
		x = input;

		if constexpr (precision == Precision::Low)
		{
			/* This is a very approximate but very fast version of acbrt. It is just
			two integer instructions (shift right and divide), plus instructions
			to load the constant.
			The constant 0x2a51067f balances the relative error at +-0.0316.
			During [0.0000;6.2832]
			Absolute [-0.03258288;+0.04560971] At [1.10097;4.00000] Average absolute [-0.00231086;+0.00445631]
			Relative [-7.57623056;+11.67612112] At [0.00027;0.00012]
			MSVC 15.9.12 2500k x64: 356.08kk 5.29x
			MSVC  16.1.1 LX800 x86: 3.97kk 6.19x
			Clang 8.0.2 MT6589: 48.03kk 19.31x
			Clang 8.0.2 MT8735M ARM64: 37.74kk 15.60x */

			ix = 0x2a51067f + ix / 3; // Initial guess.
		}
		else
		{
			/* This is a novel and fast routine for the approximate cube root of
			an IEEE float (single precision). It is derived from a similar program
			for the approximate square root.
			Caution:
				Result for -0 is NaN.
				Result for 0 is 1.24e-13.
			For denorms it is either within tolerance or gives a result < 2.1e-13.
			Gives the correct result (inf) for x = inf.
			Gives the correct result (NaN) for x = NaN.

			Medium precision:
			The relative error ranges from 0 to +0.00103.
			During [0.0000;6.2832]
			Absolute [-0.00000012;+0.00130832] At [1.62507;3.99997] Average absolute [-0.00000000;+0.00013839]
			Relative [-0.00000036;+0.33606151] At [0.08197;0.00012]
			MSVC 15.9.12 2500k x64: 187.49kk 2.81x
			MSVC  16.1.1 LX800 x86: 2.03kk 3.16x
			Clang 8.0.2 MT6589: 16.91kk 6.80x
			Clang 8.0.2 MT8735M ARM64: 15.79kk 6.53x

			High precision:
			The relative error ranges from 0 to +0.00000116.
			During [0.0000;6.2832]
			Absolute [-0.00000024;+0.00000143] At [4.75436;1.99903] Average absolute [-0.00000000;+0.00000009]
			Relative [-0.00000510;+0.00036859] At [0.00292;0.00012]
			MSVC 15.9.12 2500k x64: 127.52kk 1.89x
			MSVC  16.1.1 LX800 x86: 1.38kk 2.16x
			Clang 8.0.2 MT6589: 11.47kk 4.61x
			Clang 8.0.2 MT8735M ARM64: 11.01kk 4.55x */

			ix = ix / 4 + ix / 16; // Approximate divide by 3.
			ix = ix + ix / 16;
			ix = ix + ix / 256;
			ix = 0x2a5137a0 + ix; // Initial guess.
			x = 0.33333333f * (2.0f * x + input / (x * x)); // Newton step.

			if constexpr (precision == Precision::High)
			{
				x = 0.33333333f * (2.0f * x + input / (x * x)); // Newton step again.
			}
		}

		return x;
	}
}