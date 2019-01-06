#include "_PreHeader.hpp"
#include <MathFunctions.hpp>

using namespace StdLib;
using namespace Funcs;

void XNARef();

namespace
{
	static constexpr ui32 TestIterations = 10;
}

template <uiw index, typename T> static typename T::ScalarType Get(T vec)
{
	if constexpr (index >= T::dim) return 0;
	return vec[index];
}

template <typename T> void Compare(T v2, T v0, T v1, typename T::ScalarType transform(typename T::ScalarType, typename T::ScalarType))
{
	if constexpr (std::is_floating_point_v<typename T::ScalarType>)
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

template <typename T> static T GenerateVec(bool isAllowZero)
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

template <typename T> static T GenerateMatrix()
{
	T matrix;
	for (uiw row = 0; row < T::rows; ++row)
	{
		for (uiw column = 0; column < T::columns; ++column)
		{
			matrix[row][column] = (rand() % 1000) * 0.01f;
		}
	}
	return matrix;
}

template <typename T> static void BaseVectorTestsHelper()
{
    using st = typename T::ScalarType;
	T v0, v1, v2;

    // making sure everything is properly accessible
    v0.x = 0;
    UTest(Equal, v0[0], 0);
    v0.y = 1;
    UTest(Equal, v0[1], 1);
    if constexpr (T::dim > 2)
    {
        v0.z = 2;
        UTest(Equal, v0[2], 2);
        if constexpr (T::dim > 3)
        {
            v0.w = 3;
            UTest(Equal, v0[3], 3);
        }
    }

    if constexpr (T::dim == 2)
    {
        constexpr T constexprTest = T(0, 1);
    }
    else if constexpr (T::dim == 3)
    {
        constexpr T constexprTest = T(0, 1, 2);
    }
    else
    {
        constexpr T constexprTest = T(0, 1, 2, 3);
    }

	for (ui32 index = 0; index < TestIterations; ++index)
	{
		v0 = GenerateVec<T>(true);
		v1 = GenerateVec<T>(true);

        for (uiw dataIndex = 0; dataIndex < v0.Data().size(); ++dataIndex)
        {
            UTest(Equal, v0.Data()[dataIndex], v0[dataIndex]);
        }

		auto addTransform = [](auto s0, auto s1) { return s0 + s1; };
		auto subTransform = [](auto s0, auto s1) { return s0 - s1; };
		auto mulTransform = [](auto s0, auto s1) { return s0 * s1; };
		auto divTransform = [](auto s0, auto s1) { return s0 / (s1 ? s1 : 1); };
        auto negTransform = [](auto s0, auto s1) { return std::is_signed_v<decltype(s0)> ? s0 * -1 : s0; };

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
        st gotMin = std::numeric_limits<st>::max();
        auto computeMin = [&gotMin](st value) mutable
        {
            gotMin = std::min(gotMin, value);
        };
        v0.ForEach(computeMin);
		UTest(Equal, gotMin, legitMin);

		auto legitMax = v0.x;
		legitMax = std::max(legitMax, v0.y);
		if constexpr (T::dim > 2) legitMax = std::max(legitMax, v0.z);
		if constexpr (T::dim > 3) legitMax = std::max(legitMax, v0.w);
        st gotMax;
        if constexpr (std::is_floating_point_v<st>)
        {
            gotMax = -std::numeric_limits<st>::max();
        }
        else
        {
            gotMax = std::numeric_limits<st>::min();
        }
        auto computeMax = [&gotMax](st value) mutable
        {
            gotMax = std::max(gotMax, value);
        };
        v0.ForEach(computeMax);
		UTest(Equal, gotMax, legitMax);

		auto accum = v0.x + v0.y;
		if constexpr (T::dim > 2) accum += v0.z;
		if constexpr (T::dim > 3) accum += v0.w;
        st accumulated = st(0);
        auto accumulate = [&accumulated](st value) mutable
        {
            accumulated += value;
        };
        v0.ForEach(accumulate);
		UTest(Equal, accumulated, accum);

        if constexpr (std::is_signed_v<T::ScalarType>)
        {
            v2 = v0;
            v2 = -v2;
            Compare(v2, v0, v0, negTransform);
        }

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

template <typename T> static void IntegerVectorTestsHelper()
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

template <typename T> static void FP32VectorTestsHelper()
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
    
    {
        v0 = {1, 2};
        auto[x, y] = v0;
        UTest(Equal, x, 1);
        UTest(Equal, y, 2);
    }

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

    {
        v0 = {1, 2, 3};
        auto[x, y, z] = v0;
        UTest(Equal, x, 1);
        UTest(Equal, y, 2);
        UTest(Equal, z, 3);
    }

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
    Vector4 v0;

    {
        v0 = {1, 2, 3, 4};
        auto[x, y, z, w] = v0;
        UTest(Equal, x, 1);
        UTest(Equal, y, 2);
        UTest(Equal, z, 3);
        UTest(Equal, w, 4);
    }

	for (ui32 index = 0; index < TestIterations; ++index)
	{
		// nothing to test yet
	}
}

template <typename T> static void MathFuncsTests()
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

    UTest(true, EqualsWithEpsilon(RadDistance(zero, MathPiDouble<T>()), zero));
    UTest(true, EqualsWithEpsilon(RadDistance(zero, zero), zero));
    UTest(true, EqualsWithEpsilon(RadDistance(MathPiDouble<T>(), MathPiDouble<T>()), zero));
    UTest(true, EqualsWithEpsilon(RadDistance(MathPi<T>(), MathPiDouble<T>()), MathPi<T>()));
    UTest(true, EqualsWithEpsilon(RadDistance(zero, MathPi<T>()), MathPi<T>()));

    UTest(true, EqualsWithEpsilon(DegDistance(zero, T(360)), zero));
    UTest(true, EqualsWithEpsilon(DegDistance(zero, zero), zero));
    UTest(true, EqualsWithEpsilon(DegDistance(T(360), T(360)), zero));
    UTest(true, EqualsWithEpsilon(DegDistance(T(180), T(360)), T(180)));
    UTest(true, EqualsWithEpsilon(DegDistance(zero, T(180)), T(180)));
    UTest(true, EqualsWithEpsilon(DegDistance(T(90), T(270)), T(180)));

	for (ui32 index = 0; index < TestIterations; ++index)
	{
		T value = (rand() % 1000) * T(0.01);

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

    if constexpr (std::is_same_v<T, f32>)
    {
        f32 horToVerDeg = RadToDeg(HorizontalFOVToVertical(DegToRad(90.0f), 1920.0f / 1080.0f));
        UTest(true, EqualsWithEpsilon(horToVerDeg, 58.7f, 0.25f));

        f32 verToHorDeg = RadToDeg(VerticalFOVToHorizontal(DegToRad(58.7f), 1920.0f / 1080.0f));
        UTest(true, EqualsWithEpsilon(verToHorDeg, 90.0f, 0.25f));
    }
}

static void QuaternionTests()
{
    f32 epsilon = 0.0001f;
    Quaternion q0, q1;
    
    q0 = Quaternion::FromEuler(Vector3{25.0f, 40.0f, 75.0f} * DegToRadF32Const);
    Vector3 toEuler = q0.ToEuler().ForEach(RadToDeg);
    UTest(true, toEuler.EqualsWithEpsilon({25.0f, 40.0f, 75.0f}, epsilon));

    auto mat = Matrix3x3::CreateRS(Vector3{25.0f, 40.0f, 75.0f}.ForEach(DegToRad)); // positive trace matrix
    q0 = Quaternion(mat);
    toEuler = q0.ToEuler().ForEach(RadToDeg);
    UTest(true, toEuler.EqualsWithEpsilon({25.0f, 40.0f, 75.0f}, epsilon));

    mat = Matrix3x3::CreateRS(Vector3{25.0f, 50.0f, 256.0f}.ForEach(DegToRad)); // negative trace matrix
    q0 = Quaternion(mat);
    toEuler = q0.ToEuler().ForEach(RadToDeg);
    UTest(true, toEuler.EqualsWithEpsilon({25.0f, 50.0f, 256.0f}, epsilon));

    for (ui32 index = 0; index < TestIterations; ++index)
    {
        /*auto radDist = [](Vector3 left, Vector3 right)
        {
            f32 x = RadDistance(left.x, right.x);
            f32 y = RadDistance(left.y, right.y);
            f32 z = RadDistance(left.z, right.z);
            return Vector3{x, y, z};
        };

        Vector3 euler = GenerateVec<Vector3>(true).ForEach(RadNormalize);
        q0 = Quaternion::FromEuler(euler);
        UTest(true, q0.IsNormalized());
        toEuler = q0.ToEuler();
        Vector3 dist = radDist(toEuler, euler);
        UTest(true, dist.EqualsWithEpsilon({0, 0, 0}));
        euler = euler;*/
    }
}

template <typename T> static void MatrixTestsHelper()
{
	T m0, m1, m2, m3;
	for (ui32 index = 0; index < TestIterations; ++index)
	{
		m0 = GenerateMatrix<T>();
		m1 = GenerateMatrix<T>();

        for (uiw row = 0; row < T::rows; ++row)
        {
            for (uiw column = 0; column < T::columns; ++column)
            {
                UTest(Equal, m0[row][column], m0.Data()[row * T::columns + column]);
            }
        }

        m2 = m0 + m1;
        m3 = m0 - m1;
        for (uiw elIndex = 0; elIndex < T::numberOfElements; ++elIndex)
        {
            UTest(true, EqualsWithEpsilon(m0.Data()[elIndex] + m1.Data()[elIndex], m2.Data()[elIndex]));
            UTest(true, EqualsWithEpsilon(m0.Data()[elIndex] - m1.Data()[elIndex], m3.Data()[elIndex]));
        }

        m2 = m0;
        m2 += m1;
        m3 = m0;
        m3 -= m1;
        for (uiw elIndex = 0; elIndex < T::numberOfElements; ++elIndex)
        {
            UTest(true, EqualsWithEpsilon(m0.Data()[elIndex] + m1.Data()[elIndex], m2.Data()[elIndex]));
            UTest(true, EqualsWithEpsilon(m0.Data()[elIndex] - m1.Data()[elIndex], m3.Data()[elIndex]));
        }

        auto m0Transposed = m0.GetTransposed();
        for (uiw row = 0; row < T::rows; ++row)
        {
            for (uiw column = 0; column < T::columns; ++column)
            {
                UTest(Equal, m0[row][column], m0Transposed[column][row]);
            }
        }
        UTest(Equal, m0.GetRow(1), m0Transposed.GetColumn(1));

        for (uiw row = 0; row < T::rows; ++row)
        {
            m2.SetRow(row, m0.GetRow(row));
            UTest(Equal, m2.GetRow(row), m0.GetRow(row));
        }
        UTest(Equal, m2.Data(), m0.Data());

        for (uiw column = 0; column < T::columns; ++column)
        {
            m2.SetColumn(column, m0.GetColumn(column));
            UTest(Equal, m2.GetColumn(column), m0.GetColumn(column));
        }
        UTest(Equal, m2.Data(), m0.Data());

        UTest(true, m2.EqualsWithEpsilon(m2, 0));

        Matrix2x2 convertTest = m0.template As<Matrix2x2>();
        UTest(Equal, convertTest[0][0], m0[0][0]);
        UTest(Equal, convertTest[0][1], m0[0][1]);
        UTest(Equal, convertTest[1][0], m0[1][0]);
        UTest(Equal, convertTest[1][1], m0[1][1]);
    }
}

static void Matrix2x2Tests()
{
    constexpr Matrix2x2 constexprTest = Matrix2x2(
        5, 1,
        2, 3);

	auto inversed = constexprTest.GetInversed();
	UTest(NotEqual, inversed, nullopt);
	Vector2 toTransform = {5, 10};
	toTransform *= constexprTest;
	toTransform *= *inversed;
	UTest(true, toTransform.EqualsWithEpsilon({5, 10}, 0.0001f));

	f32 det = constexprTest.Determinant();
	UTest(true, EqualsWithEpsilon(det, 13));
}

static void Matrix2x3Tests()
{
    constexpr Matrix2x3 constexprTest = Matrix2x3(
        7, 1, 2,
        3, 4, 5);

	auto inversed = constexprTest.GetInversed();
	UTest(NotEqual, inversed, nullopt);
	Vector2 toTransform = {5, 15};
	Vector3 transformed = toTransform * constexprTest;
	transformed = transformed.ToVector2() * *inversed;
	UTest(true, transformed.ToVector2().EqualsWithEpsilon({5, 15}, 0.0001f));

	f32 det = constexprTest.Determinant();
	UTest(true, EqualsWithEpsilon(det, 25));
}

static void Matrix3x2Tests()
{
	constexpr Matrix3x2 constexprTest = Matrix3x2(
		9, 1,
		2, 3,
		4, 5);

	auto inversed = constexprTest.GetInversed();
	UTest(NotEqual, inversed, nullopt);
	Vector3 toTransform = {5, 10, 1};
	Vector2 transformed = toTransform * constexprTest;
	transformed = Vector3(transformed, 1) * *inversed;
	UTest(true, transformed.EqualsWithEpsilon({5, 10}, 0.0001f));

	f32 det = constexprTest.Determinant();
	UTest(true, EqualsWithEpsilon(det, 25));
}

static void Matrix3x3Tests()
{
    constexpr Matrix3x3 constexprTest = Matrix3x3(
        5, 1, 2,
        3, 4, 5,
        6, 7, 8);

	auto inversed = constexprTest.GetInversed();
	UTest(NotEqual, inversed, nullopt);
	Vector3 toTransform = {5, 10, 7};
	toTransform *= constexprTest;
	toTransform *= *inversed;
	UTest(true, toTransform.EqualsWithEpsilon({5, 10, 7}, 0.0001f));

	f32 det = constexprTest.Determinant();
	UTest(true, EqualsWithEpsilon(det, -15));
}

static void Matrix3x4Tests()
{
    constexpr Matrix3x4 constexprTest = Matrix3x4(
        7, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 6, 2);

	auto inversed = constexprTest.GetInversed();
	UTest(NotEqual, inversed, nullopt);
	Vector3 toTransform = {5, 15, 75};
	Vector4 transformed = toTransform * constexprTest;
	transformed = transformed.ToVector3() * *inversed;
	UTest(true, transformed.ToVector3().EqualsWithEpsilon({5, 15, 75}, 0.0001f));

	f32 det = constexprTest.Determinant();
	UTest(true, EqualsWithEpsilon(det, -152));
}

static void Matrix4x3Tests()
{
    constexpr Matrix4x3 constexprTest = Matrix4x3(
        4, 2, 3,
        4, 5, 6,
        7, 8, 9,
        1, 2, 3);

    auto checkRTS = [](const Matrix4x3 &m)
    {
        UTest(true, m.GetRow(0).EqualsWithEpsilon({0.0665462837f, 0.248354077f, -0.306417793f}));
        UTest(true, m.GetRow(1).EqualsWithEpsilon({-0.395817429f, 0.273641407f, 0.135826901f}));
        UTest(true, m.GetRow(2).EqualsWithEpsilon({0.352745265f, 0.336740196f, 0.349538058f}));
        UTest(true, m.GetRow(3).EqualsWithEpsilon({10.0f, 20.0f, 30.0f}));
    };

    auto rtsEuler = Matrix4x3::CreateRTS(Vector3{25.0f, 50.0f, 75.0f}.ForEach(DegToRad), Vector3(10, 20, 30), Vector3(0.4f, 0.5f, 0.6f));
    checkRTS(rtsEuler);

    auto rtsQuat = Matrix4x3::CreateRTS(Quaternion::FromEuler(Vector3{25.0f, 50.0f, 75.0f}.ForEach(DegToRad)), Vector3(10, 20, 30), Vector3(0.4f, 0.5f, 0.6f));
    checkRTS(rtsQuat);

    Vector4 toTransform = {1, 2, 3, 1};
    Vector3 transformed = toTransform * rtsQuat;
    auto inversedClipped = rtsQuat.GetInversed();
    UTest(NotEqual, inversedClipped, nullopt);
    Vector3 originalPoint = Vector4(transformed, 1) * *inversedClipped;
    UTest(true, originalPoint.EqualsWithEpsilon({1, 2, 3}, 0.0001f));

	f32 det = constexprTest.Determinant();
	UTest(true, EqualsWithEpsilon(det, -9));
}

static void Matrix4x4Tests()
{
    constexpr Matrix4x4 constexprTest = Matrix4x4(
        5, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 4, 1,
        2, 3, 4, 5);

    auto persp = Matrix4x4::CreatePerspectiveProjection(DegToRad(90.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f, ProjectionTarget::OGL); // TODO: test other targets
    UTest(true, persp.GetRow(0).EqualsWithEpsilon({1.0f, 0.0f, 0.0f, 0.0f}));
    UTest(true, persp.GetRow(1).EqualsWithEpsilon({0.0f, 1.777777f, 0.0f, 0.0f}));
    UTest(true, persp.GetRow(2).EqualsWithEpsilon({0.0f, 0.0f, -1.0002f, -1.0f}));
    UTest(true, persp.GetRow(3).EqualsWithEpsilon({0.0f, 0.0f, -0.20002f, 0.0f}));

    auto orto = Matrix4x4::CreateOrthographicProjection({0, 0, 0.1f}, {1920, 1080, 1000}, ProjectionTarget::OGL); // TODO: test other targets
    UTest(true, orto.GetRow(0).EqualsWithEpsilon({0.00104f, 0.0f, 0.0f, 0.0f}));
    UTest(true, orto.GetRow(1).EqualsWithEpsilon({0.0f, 0.00186f, 0.0f, 0.0f}));
    UTest(true, orto.GetRow(2).EqualsWithEpsilon({0.0f, 0.0f, -0.002f, 0.0f}));
    UTest(true, orto.GetRow(3).EqualsWithEpsilon({-1.0f, -1.0f, -1.0002f, 1.0f}));

    Vector4 toTransform{1, 2, 3, 4};
    toTransform *= persp;
    auto inversed = persp.GetInversed();
    UTest(NotEqual, inversed, nullopt);
    toTransform *= *inversed;
    UTest(true, toTransform.EqualsWithEpsilon({1, 2, 3, 4}, 0.0001f));

	f32 det = constexprTest.Determinant();
	UTest(true, EqualsWithEpsilon(det, -20));
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

    Matrix2x2Tests();
    Matrix2x3Tests();
    Matrix3x2Tests();
    Matrix3x3Tests();
    Matrix3x4Tests();
    Matrix4x3Tests();
    Matrix4x4Tests();
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
        QuaternionTests();
		MatrixTests();

		auto end = TimeMoment::Now();

		auto diff = end - start;

		if (fastest > diff.ToSeconds())
		{
			fastest = diff.ToSeconds();
			it = index;
		}

	}
    PRINTLOG("iteration %i took %.4g\n", it, fastest);

    XNARef();

    PRINTLOG("finished math lib tests\n");
}