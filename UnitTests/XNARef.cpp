#include "_PreHeader.hpp"

#ifndef PLATFORM_WINDOWS

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

void XNARef()
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

#endif