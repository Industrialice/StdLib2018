#include "_PreHeader.hpp"
#include <MathFunctions.hpp>

using namespace StdLib;
using namespace Funcs;

namespace
{
	static constexpr ui32 TestIterations = 10'00;
}

template <uiw index, typename T> typename T::ScalarType Get(T vec)
{
	if constexpr (index >= T::dim) return 0;
	return vec[index];
}

template <typename T> void Compare(T v2, T v0, T v1, typename T::ScalarType transform(typename T::ScalarType, typename T::ScalarType))
{
	if constexpr (std::is_floating_point_v<T::ScalarType>)
	{
		UTest(true, EqualsWithEpsilon(Get<0>(v2), transform(Get<0>(v0), Get<0>(v1))));
		UTest(true, EqualsWithEpsilon(Get<1>(v2), transform(Get<1>(v0), Get<1>(v1))));
		UTest(true, EqualsWithEpsilon(Get<2>(v2), transform(Get<2>(v0), Get<2>(v1))));
		UTest(true, EqualsWithEpsilon(Get<3>(v2), transform(Get<3>(v0), Get<3>(v1))));
	}
	else
	{
		UTest(Equal, Get<0>(v2), transform(Get<0>(v0), Get<0>(v1)));
		UTest(Equal, Get<1>(v2), transform(Get<1>(v0), Get<1>(v1)));
		UTest(Equal, Get<2>(v2), transform(Get<2>(v0), Get<2>(v1)));
		UTest(Equal, Get<3>(v2), transform(Get<3>(v0), Get<3>(v1)));
	}
}

template <typename T> T GenerateVec(bool isAllowZero)
{
	using st = typename T::ScalarType;
	st add = isAllowZero ? st(0) : st(1);
	T v;
	v.x = st(rand() % 10) + add;
	v.y = st(rand() % 10) + add;
	if constexpr (T::dim > 2)
	{
		v.z = st(rand() % 10) + add;
		if constexpr (T::dim > 3)
		{
			v.w = st(rand() % 10) + add;
		}
	}
	return v;
}

template <typename T> void BaseVectorTestsHelper()
{
	T v0, v1, v2;
	for (ui32 index = 0; index < TestIterations; ++index)
	{
		v0 = GenerateVec<T>(true);
		v1 = GenerateVec<T>(true);

		auto addTransform = [](auto s0, auto s1) { return s0 + s1; };
		auto subTransform = [](auto s0, auto s1) { return s0 - s1; };
		auto mulTransform = [](auto s0, auto s1) { return s0 * s1; };
		auto divTransform = [](auto s0, auto s1) { return s0 / (s1 ? s1 : 1); };

		v2 = v0 + v1;
		Compare(v2, v0, v1, addTransform);
		v2 = v0;
		v2 += v1;
		Compare(v2, v0, v1, addTransform);

		v2 = v0 - v1;
		Compare(v2, v0, v1, subTransform);
		v2 = v0;
		v2 -= v1;
		Compare(v2, v0, v1, subTransform);
		v2 = v1 - v0;
		Compare(v2, v1, v0, subTransform);
		v2 = v1;
		v2 -= v0;
		Compare(v2, v1, v0, subTransform);

		v2 = v0 * v1;
		Compare(v2, v0, v1, mulTransform);
		v2 = v0;
		v2 *= v1;
		Compare(v2, v0, v1, mulTransform);

		auto legitMin = v0.x;
		legitMin = std::min(legitMin, v0.y);
		if constexpr (T::dim > 2) legitMin = std::min(legitMin, v0.z);
		if constexpr (T::dim > 3) legitMin = std::min(legitMin, v0.w);
		UTest(Equal, v0.Min(), legitMin);

		auto legitMax = v0.x;
		legitMax = std::max(legitMax, v0.y);
		if constexpr (T::dim > 2) legitMax = std::max(legitMax, v0.z);
		if constexpr (T::dim > 3) legitMax = std::max(legitMax, v0.w);
		UTest(Equal, v0.Max(), legitMax);

		auto accum = v0.x + v0.y;
		if constexpr (T::dim > 2) accum += v0.z;
		if constexpr (T::dim > 3) accum += v0.w;
		UTest(Equal, v0.Accumulate(), accum);

		T v0p = GenerateVec<T>(false);
		T v1p = GenerateVec<T>(false);
		v2 = v0 / v1p;
		Compare(v2, v0, v1p, divTransform);
		v2 = v0;
		v2 /= v1p;
		Compare(v2, v0, v1p, divTransform);
		v2 = v1 / v0p;
		Compare(v2, v1, v0p, divTransform);
		v2 = v1;
		v2 /= v0p;
		Compare(v2, v1, v0p, divTransform);
	}
}

template <typename T> void IntegerVectorTestsHelper()
{
	T v0, v1;
	for (ui32 index = 0; index < TestIterations; ++index)
	{
		v0 = GenerateVec<T>(true);
		v1 = GenerateVec<T>(true);

		bool equals = (Get<0>(v0) == Get<0>(v1)) && (Get<1>(v0) == Get<1>(v1)) && (Get<2>(v0) == Get<2>(v1)) && (Get<3>(v0) == Get<3>(v1));
		UTest(Equal, v0 == v1, equals);
		UTest(NotEqual, v0 != v1, equals);
		UTest(Equal, v0, v0);
		UTest(false, v0 != v0);
	}
}

template <typename T> void FP32VectorTestsHelper()
{
	using st = typename T::ScalarType;
	T v2, v0, v1;
	for (ui32 index = 0; index < TestIterations; ++index)
	{
		v0 = GenerateVec<T>(true);
		v1 = GenerateVec<T>(true);

		st lenSquareComp = Get<0>(v0) * Get<0>(v0) + Get<1>(v0) * Get<1>(v0) + Get<2>(v0) * Get<2>(v0) + Get<3>(v0) * Get<3>(v0);
		UTest(true, EqualsWithEpsilon(v0.LengthSquare(), lenSquareComp));
		UTest(true, EqualsWithEpsilon(v0.Length(), sqrt(lenSquareComp)));
		UTest(Equal, EqualsWithEpsilon(v0.Length(), 1.0f), v0.IsNormalized());
		UTest(Equal, EqualsWithEpsilon(v0.LengthSquare(), 1.0f), v0.IsNormalized());
		v2 = v0 + 1;
		UTest(true, v2.GetNormalized().IsNormalized());
		v2.Normalize();
		UTest(true, v2.IsNormalized());

		v2 = v0 - v1;
		st distSquareComp = Get<0>(v2) * Get<0>(v2) + Get<1>(v2) * Get<1>(v2) + Get<2>(v2) * Get<2>(v2) + Get<3>(v2) * Get<3>(v2);
		UTest(true, EqualsWithEpsilon(v0.DistanceSquare(v1), distSquareComp));
		UTest(true, EqualsWithEpsilon(v0.Distance(v1), sqrt(distSquareComp)));

		st avgComp = (Get<0>(v0) + Get<1>(v0) + Get<2>(v0) + Get<3>(v0)) / T::dim;
		UTest(true, EqualsWithEpsilon(v0.Average(), avgComp));

		st dotComp = Get<0>(v0) * Get<0>(v1) + Get<1>(v0) * Get<1>(v1) + Get<2>(v0) * Get<2>(v1) + Get<3>(v0) * Get<3>(v1);
		UTest(true, EqualsWithEpsilon(v0.Dot(v1), dotComp));

		auto equalsWithEpsilon = [v0](T v1, st epsilon)
		{
			bool equalsComp = EqualsWithEpsilon(Get<0>(v0), Get<0>(v1), epsilon);
			equalsComp &= EqualsWithEpsilon(Get<1>(v0), Get<1>(v1), epsilon);
			equalsComp &= EqualsWithEpsilon(Get<2>(v0), Get<2>(v1), epsilon);
			equalsComp &= EqualsWithEpsilon(Get<3>(v0), Get<3>(v1), epsilon);
			UTest(Equal, v0.EqualsWithEpsilon(v1, epsilon), equalsComp);
		};
		equalsWithEpsilon(v1, DefaultF32Epsilon);
		equalsWithEpsilon(v0, DefaultF32Epsilon);
		equalsWithEpsilon(v1, 0.001f);
		equalsWithEpsilon(v0, 0.001f);
		equalsWithEpsilon(v1, 0.01f);
		equalsWithEpsilon(v0, 0.01f);
		equalsWithEpsilon(v0, 0.0f);

		v2 = v0 + (v1 - v0) * 0.45f;
		UTest(true, v2.EqualsWithEpsilon(v0.GetLerped(v1, 0.45f)));
		v2 = v0;
		v2.Lerp(v1, 0.32f);
		UTest(Equal, v2, v0.GetLerped(v1, 0.32f));
	}
}

static void Vector2Tests()
{
	Vector2 v0;
	for (ui32 index = 0; index < TestIterations; ++index)
	{
		v0 = GenerateVec<Vector2>(true);
		UTest(true, v0.GetLeftNormal().EqualsWithEpsilon({-v0.y, v0.x}));
		UTest(true, v0.GetRightNormal().EqualsWithEpsilon({v0.y, -v0.x}));
	}
}

static void Vector3Tests()
{
	Vector3 v0, v1, v2;
	for (ui32 index = 0; index < TestIterations; ++index)
	{
		v0 = GenerateVec<Vector3>(true);
		v1 = GenerateVec<Vector3>(true);

		f32 x = v0.y * v1.z - v0.z * v1.y;
		f32 y = v0.z * v1.x - v0.x * v1.z;
		f32 z = v0.x * v1.y - v0.y * v1.x;

		v2 = v0.GetCrossed(v1);
		UTest(true, EqualsWithEpsilon(v2.x, x));
		UTest(true, EqualsWithEpsilon(v2.y, y));
		UTest(true, EqualsWithEpsilon(v2.z, z));

		v2 = v0;
		v2.Cross(v1);
		UTest(true, v2.EqualsWithEpsilon(v0.GetCrossed(v1)));
	}
}

static void Vector4Tests()
{
	for (ui32 index = 0; index < TestIterations; ++index)
	{
		// nothing to test yet
	}
}

template <typename T> void MathFuncsTests()
{
	T epsilon = (T)DefaultF32Epsilon;
	static constexpr T zero = T(0);

	UTest(true, EqualsWithEpsilon(zero, zero, zero));
	UTest(true, EqualsWithEpsilon(zero, zero, epsilon));
	UTest(true, EqualsWithEpsilon(std::numeric_limits<T>::min(), std::numeric_limits<T>::min(), zero));
	UTest(true, EqualsWithEpsilon(-std::numeric_limits<T>::min(), -std::numeric_limits<T>::min(), zero));
	UTest(true, EqualsWithEpsilon(std::numeric_limits<T>::min(), std::numeric_limits<T>::min(), epsilon));
	UTest(true, EqualsWithEpsilon(-std::numeric_limits<T>::min(), -std::numeric_limits<T>::min(), epsilon));
	UTest(true, EqualsWithEpsilon(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), zero));
	UTest(true, EqualsWithEpsilon(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max(), zero));
	UTest(true, EqualsWithEpsilon(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), epsilon));
	UTest(true, EqualsWithEpsilon(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max(), epsilon));

	constexpr T pi = MathPi<T>();
	constexpr T pihalf = MathPiHalf<T>();
	constexpr T piQuarter = MathPiQuarter<T>();
	constexpr T piDouble = MathPiDouble<T>();

	constexpr T piFromDeg = DegToRad(T(180));
	UTest(true, EqualsWithEpsilon(piFromDeg, pi));
	constexpr T degFromPi = RadToDeg(pi);
	UTest(true, EqualsWithEpsilon(degFromPi, T(180)));

	constexpr T piFromNormalized = RadNormalize(piDouble + pi);
	UTest(true, EqualsWithEpsilon(piFromNormalized, pi));
	constexpr T piFromNormalizedClose = RadNormalizeClose(piDouble + pi);
	UTest(true, EqualsWithEpsilon(piFromNormalizedClose, pi));

	for (ui32 index = 0; index < TestIterations; ++index)
	{
		T value = rand() * T(0.01);

		UTest(true, EqualsWithEpsilon(value, value, zero));
		UTest(true, EqualsWithEpsilon(value, value, epsilon));

		UTest(true, EqualsWithEpsilon(DegToRad(T(180.0)), MathPi<T>()));
		UTest(true, EqualsWithEpsilon(DegToRad(T(90.0)), MathPiHalf<T>()));
		UTest(true, EqualsWithEpsilon(DegToRad(T(45.0f)), MathPiQuarter<T>()));

		UTest(true, EqualsWithEpsilon(RadToDeg(MathPi<T>()), T(180.0)));
		UTest(true, EqualsWithEpsilon(RadToDeg(MathPiHalf<T>()), T(90.0)));
		UTest(true, EqualsWithEpsilon(RadToDeg(MathPiQuarter<T>()), T(45.0)));

		static constexpr T radEpsilon = T(0.00001);

		UTest(true, EqualsWithEpsilon(RadNormalize(MathPi<T>() + MathPiDouble<T>() * 10), MathPi<T>(), radEpsilon));
		UTest(true, EqualsWithEpsilon(RadNormalize(MathPi<T>()), MathPi<T>(), radEpsilon));
		UTest(true, EqualsWithEpsilon(RadNormalize(-MathPi<T>()), MathPi<T>(), radEpsilon));
		UTest(true, EqualsWithEpsilon(RadNormalize(zero), zero, radEpsilon));
		UTest(true, EqualsWithEpsilon(RadNormalize(MathPiDouble<T>()), zero, radEpsilon));
		UTest(true, EqualsWithEpsilon(RadNormalize(-MathPi<T>() - MathPiDouble<T>() * 10), MathPi<T>(), radEpsilon));

		UTest(true, EqualsWithEpsilon(RadNormalizeClose(MathPi<T>() + MathPiDouble<T>()), MathPi<T>(), radEpsilon));
		UTest(true, EqualsWithEpsilon(RadNormalizeClose(MathPi<T>()), MathPi<T>(), radEpsilon));
		UTest(true, EqualsWithEpsilon(RadNormalizeClose(-MathPi<T>()), MathPi<T>(), radEpsilon));
		UTest(true, EqualsWithEpsilon(RadNormalizeClose(zero), zero, radEpsilon));
		UTest(true, EqualsWithEpsilon(RadNormalizeClose(MathPiDouble<T>()), zero, radEpsilon));

		T value2 = value + 1;
		T lerpEpsilon = T(0.0001);
		UTest(true, EqualsWithEpsilon(Lerp(value, value2, T(0.5)), value + T(0.5), lerpEpsilon));
		UTest(true, EqualsWithEpsilon(Lerp(value, value2, zero), value, lerpEpsilon));
		UTest(true, EqualsWithEpsilon(Lerp(value, value2, T(1)), value2, lerpEpsilon));
	}
}

template <typename T> void MatrixTestsHelper()
{
	for (ui32 index = 0; index < TestIterations; ++index)
	{
	}
}

static void BaseVectorTests()
{
	BaseVectorTestsHelper<i32Vector2>();
	BaseVectorTestsHelper<i32Vector3>();
	BaseVectorTestsHelper<i32Vector4>();
	BaseVectorTestsHelper<ui32Vector2>();
	BaseVectorTestsHelper<ui32Vector3>();
	BaseVectorTestsHelper<ui32Vector4>();
	BaseVectorTestsHelper<Vector2>();
	BaseVectorTestsHelper<Vector3>();
	BaseVectorTestsHelper<Vector4>();

	IntegerVectorTestsHelper<i32Vector2>();
	IntegerVectorTestsHelper<i32Vector3>();
	IntegerVectorTestsHelper<i32Vector4>();
	IntegerVectorTestsHelper<ui32Vector2>();
	IntegerVectorTestsHelper<ui32Vector3>();
	IntegerVectorTestsHelper<ui32Vector4>();
}

static void FP32VectorTests()
{
	FP32VectorTestsHelper<Vector2>();
	FP32VectorTestsHelper<Vector3>();
	FP32VectorTestsHelper<Vector4>();

	Vector2Tests();
	Vector3Tests();
	Vector4Tests();
}

static void MatrixTests()
{
	MatrixTestsHelper<Matrix2x2>();
	MatrixTestsHelper<Matrix2x3>();
	MatrixTestsHelper<Matrix3x2>();
	MatrixTestsHelper<Matrix3x3>();
	MatrixTestsHelper<Matrix3x4>();
	MatrixTestsHelper<Matrix4x3>();
	MatrixTestsHelper<Matrix4x4>();
}

void MathLibTests()
{
	f32 fastest = FLT_MAX;
	i32 it = 0;
	for (i32 index = 0; index < 10; ++index)
	{
		auto start = TimeMoment::Now();

		MathFuncsTests<f32>();
		MathFuncsTests<f64>();
		BaseVectorTests();
		FP32VectorTests();
		MatrixTests();

		auto end = TimeMoment::Now();

		auto diff = end - start;

		if (fastest > diff.ToSeconds())
		{
			fastest = diff.ToSeconds();
			it = index;
		}

	}
	printf("iteration %i took %.4g\n", it, fastest);

    Vector3 rotTest(1.0f, 2.0f, 3.0f);
    auto rotMatrix = Matrix3x3::CreateRS(Vector3{1.0f, 2.0f, 3.0f});
    rotTest *= rotMatrix;
    rotMatrix.Transpose();
    rotTest *= rotMatrix;
    UTest(true, EqualsWithEpsilon(rotTest.x, 1.0f) && EqualsWithEpsilon(rotTest.y, 2.0f) && EqualsWithEpsilon(rotTest.z, 3.0f));

    PRINTLOG("finished math lib tests\n");
}