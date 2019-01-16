#include "_PreHeader.hpp"

#if !defined(PLATFORM_WINDOWS)

void XNARef()
{}

#else

#include <DirectXMath.h>
//#include <DirectXMathMatrix.inl>
//#include <DirectXPackedVector.h>
#include <MatrixMathTypes.hpp>
#include <MathFunctions.hpp>

using namespace DirectX;
using namespace StdLib;

template <typename T> static void MatrixTestsHelper()
{
	XMMATRIX xmMat;
	auto g = [&xmMat](uiw row, uiw column) -> f32 &
	{
		return xmMat.r[row].m128_f32[column];
	};

	Matrix4x4 values =
	{
		 1,  2,  3,  4,
		 5,  6,  7,  8,
		 9, -1, -2, -3,
		-4, -5, -6,  8
	};

	T m;
	for (uiw row = 0; row < 4; ++row)
	{
		for (uiw column = 0; column < 4; ++column)
		{
			m.StoreValueBoundless(row, column, values[row][column]);
			g(row, column) = m.FetchValueBoundless(row, column);
		}
	}

	f32 customDet = m.Determinant();
	XMVECTOR xmDet = XMMatrixDeterminant(xmMat);
	UTest(true, EqualsWithEpsilon(customDet, xmDet.m128_f32[0]));

	m.Inverse();
	xmMat = XMMatrixInverse(&xmDet, xmMat);
	for (uiw row = 0; row < 4; ++row)
	{
		for (uiw column = 0; column < 4; ++column)
		{
			f32 custom = m.FetchValueBoundless(row, column);
			f32 xms = g(row, column);
			UTest(true, EqualsWithEpsilon(custom, xms));
		}
	}
}

static void TestMatrices()
{
	MatrixTestsHelper<Matrix2x2>();
	MatrixTestsHelper<Matrix2x3>();
	MatrixTestsHelper<Matrix3x2>();
	MatrixTestsHelper<Matrix3x3>();
	MatrixTestsHelper<Matrix3x4>();
	MatrixTestsHelper<Matrix4x3>();
	MatrixTestsHelper<Matrix4x4>();

	auto axisVec = Vector3{4, 1, 3}.GetNormalized();
	auto rotAngle = 0.123f;
	auto m4x3axis = Matrix4x3::CreateRotationAroundAxis(axisVec, rotAngle);
	auto m4x4axis = Matrix4x4::CreateRotationAroundAxis(axisVec, rotAngle);

	XMVECTOR xmAxisVec = XMVectorSet(axisVec.x, axisVec.y, axisVec.z, 1.0f);
	XMMATRIX xmMat = XMMatrixRotationAxis(xmAxisVec, rotAngle);

	auto g = [&xmMat](uiw row, uiw column)
	{
		return xmMat.r[row].m128_f32[column];
	};
	for (uiw row = 0; row < 4; ++row)
	{
		for (uiw column = 0; column < 4; ++column)
		{
			f32 custom = m4x3axis.FetchValueBoundless(row, column);
			f32 xms = g(row, column);
			UTest(true, EqualsWithEpsilon(custom, xms));
			custom = m4x4axis[row][column];
			UTest(true, EqualsWithEpsilon(custom, xms));
		}
	}
}

static void TestQuaternions()
{
	XMVECTOR xmq0 = XMQuaternionIdentity();
	XMVECTOR xmq1 = XMVectorSet(1, 2, 3, 1);
	auto equal = [](XMVECTOR left, Quaternion right)
	{
		for (uiw i = 0; i < 4; ++i)
		{
			UTest(true, EqualsWithEpsilon(left.m128_f32[i], right[i]));
		}
	};
	auto toxmat = [](XMMATRIX &target, const auto &source)
	{
		for (uiw row = 0; row < 4; ++row)
		{
			for (uiw column = 0; column < 4; ++column)
			{
				target.r[row].m128_f32[column] = source.FetchValueBoundless(row, column);
			}
		}
	};

	Quaternion q0, q1 = {1, 2, 3, 1};
	equal(xmq0, q0);

	xmq1 = XMQuaternionNormalize(xmq1);
	q1.Normalize();
	equal(xmq1, q1);

	auto axis = Vector3{1, 2, 3}.GetNormalized();
	f32 angle = 0.245f;

	q0 = Quaternion::FromAxis(axis, angle);
	xmq0 = XMVectorSet(axis.x, axis.y, axis.z, 0);
	xmq1 = XMQuaternionRotationAxis(xmq0, angle);
	equal(xmq1, q0);

	auto matrix = Matrix3x3::CreateRS(Vector3{25.0f, 40.0f, 75.0f}.ForEach(DegToRad)); // positive trace matrix
	XMMATRIX xmatrix;
	toxmat(xmatrix, matrix);
	q0 = Quaternion::FromMatrix(matrix);
	xmq0 = XMQuaternionRotationMatrix(xmatrix);
	equal(xmq0, q0);

	matrix = Matrix3x3::CreateRS(Vector3{25.0f, 50.0f, 256.0f}.ForEach(DegToRad)); // negative trace matrix
	toxmat(xmatrix, matrix);
	q0 = Quaternion::FromMatrix(matrix);
	xmq0 = XMQuaternionRotationMatrix(xmatrix);
	equal(xmq0, q0);

	q0 = Quaternion{2, 3, 4, 1}.GetNormalized();
	q1 = Quaternion{5, 6, 7, 1}.GetNormalized();
	xmq0 = XMVectorSet(2, 3, 4, 1);
	xmq0 = XMQuaternionNormalize(xmq0);
	xmq1 = XMVectorSet(5, 6, 7, 1);
	xmq1 = XMQuaternionNormalize(xmq1);
	q0 *= q1;
	xmq0 = XMQuaternionMultiply(xmq0, xmq1);
	equal(xmq0, q0);

	xmq0 = XMQuaternionInverse(xmq0);
	q0.Inverse();
	equal(xmq0, q0);

	UTest(true, EqualsWithEpsilon(XMQuaternionLength(xmq0).m128_f32[0], q0.Length()));
	UTest(true, EqualsWithEpsilon(XMQuaternionLengthSq(xmq0).m128_f32[0], q0.LengthSquare()));

	auto [qaxis, qangle] = q0.ToAxisAndAngle();
	XMVECTOR xmaxis;
	f32 xmangle;
	XMQuaternionToAxisAngle(&xmaxis, &xmangle, xmq0);
	// axis of XMQuaternionToAxisAngle is weird, it is just a copy of xmq0
	//UTest(true, EqualsWithEpsilon(xmaxis.m128_f32[0], qaxis.x, 0.0001f));
	//UTest(true, EqualsWithEpsilon(xmaxis.m128_f32[1], qaxis.y, 0.0001f));
	//UTest(true, EqualsWithEpsilon(xmaxis.m128_f32[2], qaxis.z, 0.0001f));
	UTest(true, EqualsWithEpsilon(xmangle, qangle, 0.0001f));
}

void XNARef()
{
	TestMatrices();
	TestQuaternions();

	Logger::Message("Finished XNA reference tests\n");
}

#endif