#include "_PreHeader.hpp"
#include "MatrixMathTypes.hpp"
#include "_MatrixMathFunctions.hpp"
#include "MathFunctions.hpp"
#include "ApproxMath.hpp"

using namespace StdLib;

namespace StdLib
{
    template struct _VectorBase<f32, 2>;
    template struct _VectorBase<f32, 3>;
    template struct _VectorBase<f32, 4>;

    template struct _VectorBase<i32, 2>;
    template struct _VectorBase<i32, 3>;
    template struct _VectorBase<i32, 4>;

    template struct _VectorBase<ui32, 2>;
    template struct _VectorBase<ui32, 3>;
    template struct _VectorBase<ui32, 4>;

    template struct Vector2Base<f32>;
    template struct Vector3Base<f32>;
    template struct Vector4Base<f32>;

    template struct Vector2Base<i32>;
    template struct Vector3Base<i32>;
    template struct Vector4Base<i32>;

    template struct Vector2Base<ui32>;
    template struct Vector3Base<ui32>;
    template struct Vector4Base<ui32>;

    template struct _VectorFP<Vector2Base<f32>>;
    template struct _VectorFP<Vector3Base<f32>>;
    template struct _VectorFP<Vector4Base<f32>>;

    template struct _Matrix<2, 2>;
    template struct _Matrix<3, 2>;
    template struct _Matrix<2, 3>;
    template struct _Matrix<3, 3>;
    template struct _Matrix<4, 3>;
    template struct _Matrix<3, 4>;
    template struct _Matrix<4, 4>;

    template struct Rectangle<f32>;
    template struct Rectangle<bool>;
    template struct Rectangle<i32>;
}

template <typename T> T CreateOrthographicMarix(const Vector3 &min, const Vector3 &max)
{
    T m;
    m[0][0] = 2.0f / (max.x - min.x);
    m[3][0] = -((max.x + min.x) / (max.x - min.x));
    m[1][1] = 2.0f / (max.y - min.y);
    m[3][1] = -((max.y + min.y) / (max.y - min.y));
    m[2][2] = 2.0f / (max.z - min.z);
    m[3][2] = -((max.z + min.z) / (max.z - min.z));
    return m;
}

/////////////
// Vector2 //
/////////////

Vector2 Vector2::GetLeftNormal() const
{
    return {-y, x};
}

Vector2 Vector2::GetRightNormal() const
{
    return {y, -x};
}

/////////////
// Vector3 //
/////////////

void Vector3::Cross(const Vector3 &other)
{
    f32 nx = this->y * other.z - this->z * other.y;
    f32 ny = this->z * other.x - this->x * other.z;
    f32 nz = this->x * other.y - this->y * other.x;

    x = nx;
    y = ny;
    z = nz;
}

Vector3 Vector3::GetCrossed(const Vector3 &other) const
{
    f32 nx = this->y * other.z - this->z * other.y;
    f32 ny = this->z * other.x - this->x * other.z;
    f32 nz = this->x * other.y - this->y * other.x;

    return Vector3(nx, ny, nz);
}

///////////////
// Matrix3x2 //
///////////////

Matrix3x3 Matrix3x2::GetInversed() const
{
    Matrix3x3 r;

    r[0][0] = elements[1][1];
    r[1][0] = -elements[1][0];
    r[2][0] = elements[1][0] * elements[2][1] - elements[1][1] * elements[2][0];

    r[0][1] = -elements[0][1];
    r[1][1] = elements[0][0];
    r[2][1] = -elements[0][0] * elements[2][1] - elements[0][1] * elements[2][0];

    r[0][2] = 0;
    r[1][2] = 0;
    r[2][2] = +(elements[0][0] * elements[1][1] - elements[0][1] * elements[1][0]);

    f32 det = elements[0][0] * r[0][0] + elements[0][1] * r[1][0];
    f32 revDet = 1.f / det;

    return r * revDet;
}

Matrix3x2 Matrix3x2::GetInversedClipped() const
{
    Matrix3x2 r;

    r[0][0] = elements[1][1];
    r[1][0] = -elements[1][0];
    r[2][0] = elements[1][0] * elements[2][1] - elements[1][1] * elements[2][0];

    r[0][1] = -elements[0][1];
    r[1][1] = elements[0][0];
    r[2][1] = -elements[0][0] * elements[2][1] - elements[0][1] * elements[2][0];

    f32 det = elements[0][0] * r[0][0] + elements[0][1] * r[1][0];
    f32 revDet = 1.f / det;

    return r * revDet;
}

Matrix3x2 Matrix3x2::CreateRTS(const optional<f32> &rotation, const optional<Vector2> &translation, const optional<Vector2> &scale)
{
    Matrix3x2 result;

    if (rotation)
    {
        f32 cr = cos(*rotation);
        f32 sr = sin(*rotation);

        result[0][0] = cr; result[0][1] = -sr;
        result[1][0] = sr; result[1][1] = cr;
    }

    if (translation)
    {
        result[2][0] = translation->x; result[2][1] = translation->y;
    }

    if (scale)
    {
        result[0][0] *= scale->x;
        result[0][1] *= scale->x;
        result[1][0] *= scale->y;
        result[1][1] *= scale->y;
    }

    return result;
}

///////////////
// Matrix4x3 //
///////////////

Matrix4x4 Matrix4x3::operator*(const Matrix4x4 &other) const
{
    Matrix4x4 result;
    for (uiw rowIndex = 0; rowIndex < 4; ++rowIndex)
    {
        for (uiw columnIndex = 0; columnIndex < 4; ++columnIndex)
        {
            result[rowIndex][columnIndex] =
                elements[rowIndex][0] * other[0][columnIndex] +
                elements[rowIndex][1] * other[1][columnIndex] +
                elements[rowIndex][2] * other[2][columnIndex];
            if (rowIndex == 3)
            {
                result[rowIndex][columnIndex] += other[3][columnIndex];
            }
        }
    }
    return result;
}

Matrix4x3 Matrix4x3::CreateRotationAroundAxis(const Vector3 &axis, f32 angle)
{
    return StdLib::CreateRotationAroundAxis<Matrix4x3>(axis, angle);
}

Matrix4x3 Matrix4x3::CreateRTS(const optional<Vector3> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale)
{
    return StdLib::CreateRTS<Matrix4x3, true>(rotation, translation, scale);
}

Matrix4x3 Matrix4x3::CreateRTS(const optional<Quaternion> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale)
{
    return StdLib::CreateRTS<Matrix4x3, true>(rotation, translation, scale);
}

Matrix4x3 Matrix4x3::CreateOrthographicProjection(const Vector3 &min, const Vector3 &max)
{
    return CreateOrthographicMarix<Matrix4x3>(min, max);
}

///////////////
// Matrix3x4 //
///////////////

Matrix3x4 Matrix3x4::CreateRotationAroundAxis(const Vector3 &axis, f32 angle)
{
    return StdLib::CreateRotationAroundAxis<Matrix3x4>(axis, angle);
}

Matrix3x4 Matrix3x4::CreateRS(const optional<Vector3> &rotation, const optional<Vector3> &scale)
{
    return StdLib::CreateRTS<Matrix3x4, false>(rotation, nullopt, scale);
}

Matrix3x4 Matrix3x4::CreateRS(const optional<Quaternion> &rotation, const optional<Vector3> &scale)
{
    return StdLib::CreateRTS<Matrix3x4, false>(rotation, nullopt, scale);
}

///////////////
// Matrix4x4 //
///////////////

void Matrix4x4::Transpose()
{
	TransposeSquareMatrix(*this);
}

Matrix4x4 Matrix4x4::CreateRotationAroundAxis(const Vector3 &axis, f32 angle)
{
    return StdLib::CreateRotationAroundAxis<Matrix4x4>(axis, angle);
}

Matrix4x4 Matrix4x4::CreateRTS(const optional<Vector3> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale)
{
    return StdLib::CreateRTS<Matrix4x4, true>(rotation, translation, scale);
}

Matrix4x4 Matrix4x4::CreateRTS(const optional<Quaternion> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale)
{
    return StdLib::CreateRTS<Matrix4x4, true>(rotation, translation, scale);
}

Matrix4x4 Matrix4x4::CreatePerspectiveProjection(f32 horizontalFOV, f32 aspectRatio, f32 nearPlane, f32 farPlane)
{
    Matrix4x4 result;

    f32 fovH = horizontalFOV;
    f32 tanOfHalfFovH = tan(fovH * 0.5f);
    f32 fovV = 2.0f * atan(tanOfHalfFovH / aspectRatio);
    f32 q = farPlane / (farPlane - nearPlane);
    f32 w = 1.0f / tanOfHalfFovH;
    f32 h = 1.0f / tan(fovV * 0.5f);
    f32 l = -q * nearPlane;

    result[0][0] = w; result[0][1] = 0; result[0][2] = 0; result[0][3] = 0;
    result[1][0] = 0; result[1][1] = h; result[1][2] = 0; result[1][3] = 0;
    result[2][0] = 0; result[2][1] = 0; result[2][2] = q; result[2][3] = 1;
    result[3][0] = 0; result[3][1] = 0; result[3][2] = l; result[3][3] = 0;

    return result;
}

Matrix4x4 Matrix4x4::CreateOrthographicProjection(const Vector3 &min, const Vector3 &max)
{
    auto m = CreateOrthographicMarix<Matrix4x4>(min, max);
    m[3][3] = 1.0f;
    return m;
}

///////////////
// Matrix2x2 //
///////////////

void Matrix2x2::Transpose()
{
	TransposeSquareMatrix(*this);
}

///////////////
// Matrix3x3 //
///////////////

void Matrix3x3::Transpose()
{
	TransposeSquareMatrix(*this);
}

Matrix3x3 Matrix3x3::CreateRotationAroundAxis(const Vector3 &axis, f32 angle)
{
    return StdLib::CreateRotationAroundAxis<Matrix3x3>(axis, angle);
}

Matrix3x3 Matrix3x3::CreateRS(const optional<Vector3> &rotation, const optional<Vector3> &scale)
{
    return StdLib::CreateRTS<Matrix3x3, false>(rotation, nullopt, scale);
}

Matrix3x3 Matrix3x3::CreateRS(const optional<Quaternion> &rotation, const optional<Vector3> &scale)
{
    return StdLib::CreateRTS<Matrix3x3, false>(rotation, nullopt, scale);
}

////////////////
// Quaternion //
////////////////

Quaternion Quaternion::FromEuler(const Vector3 &source)
{
    f32 cx = cos(source.x);
    f32 cy = cos(source.y);
    f32 cz = cos(source.z);

    f32 sx = sin(source.x);
    f32 sy = sin(source.y);
    f32 sz = sin(source.z);

    f32 w = cx * cy * cz + sx * sy * sz;
    f32 x = sx * cy * cz - cx * sy * sz;
    f32 y = cx * sy * cz + sx * cy * sz;
    f32 z = cx * cy * sz - sx * sy * cz;

    return {x, y, z, w};
}

Quaternion::Quaternion(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w)
{}

// TODO: make sure that the rotation matrix is unit length (doesn't contain scale)
Quaternion::Quaternion(const Matrix3x3 &matrix)
{
    const f32 trace = matrix[0][0] + matrix[1][1] + matrix[2][2];

    if (trace > 0.0f)
    {
        f32 s = sqrt(trace + 1.0f);
        w = s * 0.5f;
        s = 0.5f / s;

        x = (matrix[1][2] - matrix[2][1]) * s;
        y = (matrix[2][0] - matrix[0][2]) * s;
        z = (matrix[0][1] - matrix[1][0]) * s;
    }
    else
    {
        static constexpr uiw nxt[3] = {1, 2, 0};
        uiw i = 0;

        if (matrix[1][1] > matrix[0][0])
        {
            i = 1;
        }
        if (matrix[2][2] > matrix[i][i])
        {
            i = 2;
        }

        uiw j = nxt[i];
        uiw k = nxt[j];
        
        f32 s = sqrt((matrix[i][i] - (matrix[j][j] + matrix[k][k])) + 1.0f);

        operator[](i) = s * 0.5f;
        s = 0.5f / s;

        operator[](3) = (matrix[j][k] - matrix[k][j]) * s;
        operator[](j) = (matrix[i][j] + matrix[j][i]) * s;
        operator[](k) = (matrix[i][k] + matrix[k][i]) * s;
    }
}

Quaternion::Quaternion(const Vector3 &axis, f32 angle)
{
    ASSUME(axis.IsNormalized());

    const f32 halfAngle = 0.5f * angle;
    const f32 s = sin(halfAngle);

    x = s * axis.x;
    y = s * axis.y;
    z = s * axis.z;
    w = cos(halfAngle);
}

Quaternion Quaternion::operator + (const Quaternion &other) const
{
    return {x + other.x, y + other.y, z + other.z, w + other.w};
}

Quaternion &Quaternion::operator += (const Quaternion &other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Quaternion Quaternion::operator - (const Quaternion &other) const
{
    return {x - other.x, y - other.y, z - other.z, w - other.w};
}

Quaternion &Quaternion::operator -= (const Quaternion &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Quaternion Quaternion::operator * (const Quaternion &other) const
{
    f32 nx = w * other.x + x * other.w + y * other.z - z * other.y;
    f32 ny = w * other.y + y * other.w + z * other.x - x * other.z;
    f32 nz = w * other.z + z * other.w + x * other.y - y * other.x;
    f32 nw = w * other.w - x * other.x - y * other.y - z * other.z;
    return {nx, ny, nz, nw};
}

Quaternion &Quaternion::operator *= (const Quaternion &other)
{
    f32 nx = w * other.x + x * other.w + y * other.z - z * other.y;
    f32 ny = w * other.y + y * other.w + z * other.x - x * other.z;
    f32 nz = w * other.z + z * other.w + x * other.y - y * other.x;
    f32 nw = w * other.w - x * other.x - y * other.y - z * other.z;
    x = nx;
    y = ny;
    z = nz;
    w = nw;
    return *this;
}

Quaternion Quaternion::operator * (f32 scale) const
{
    return {x * scale, y * scale, z * scale, w * scale};
}

Quaternion &Quaternion::operator *= (f32 scale)
{
    x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;
    return *this;
}

Quaternion Quaternion::operator / (f32 scale) const
{
    f32 r = 1.0f / scale;
    return {x * r, y * r, z * r, w * r};
}

Quaternion &Quaternion::operator /= (f32 scale)
{
    f32 r = 1.0f / scale;
    x *= r;
    y *= r;
    z *= r;
    w *= r;
    return *this;
}

f32 &Quaternion::operator[](uiw index)
{
    if (index == 0) return x;
    if (index == 1) return y;
    if (index == 2) return z;
    ASSUME(index == 3);
    return w;
}

const f32 &Quaternion::operator[](uiw index) const
{
    if (index == 0) return x;
    if (index == 1) return y;
    if (index == 2) return z;
    ASSUME(index == 3);
    return w;
}

f32 *Quaternion::Data()
{
    return &x;
}

const f32 *Quaternion::Data() const
{
    return &x;
}

Vector3 Quaternion::RotateVector(const Vector3 &source) const
{
    Vector3 u{x, y, z};
    Vector3 v = source;
    Vector3 uv = u.GetCrossed(v);
    Vector3 uuv = u.GetCrossed(uv);
    return v + ((uv * w) + uuv) * 2.0f;
}

void Quaternion::Normalize()
{
    f32 lengthSquare = x * x + y * y + z * z + w * w;
    f32 r = ApproxMath::RSqrt<ApproxMath::Precision::High>(lengthSquare);
    x *= r;
    y *= r;
    z *= r;
    w *= r;
}

Quaternion Quaternion::GetNormalized() const
{
    f32 lengthSquare = x * x + y * y + z * z + w * w;
    f32 r = ApproxMath::RSqrt<ApproxMath::Precision::High>(lengthSquare);
    return {x * r, y * r, z * r, w * r};
}

bool Quaternion::IsNormalized(f32 epsilon) const
{
    f32 length = sqrt(x * x + y * y + z * z + w * w);
    return (length >= 1.0f - epsilon) && (length <= 1.0f + epsilon);
}

void Quaternion::Inverse()
{
    x = -x;
    y = -y;
    z = -z;
}

Quaternion Quaternion::GetInversed() const
{
    return {-x, -y, -z, w};
}

f32 Quaternion::GetAngle() const
{
    return 2.0f * acos(w);
}

Vector3 Quaternion::GetRotationAxis() const
{
    f32 r = ApproxMath::RSqrt<ApproxMath::Precision::High>(std::max(1.0f - (w * w), DefaultF32Epsilon));
    return {x * r, y * r, z * r};
}

// based on https://math.stackexchange.com/questions/1477926/quaternion-to-euler-with-some-properties
Vector3 Quaternion::ToEuler() const
{
    const f32 sqx = x * x;
    const f32 sqy = y * y;
    const f32 sqz = z * z;

    f32 eulerX, eulerY, eulerZ;

    f32 singularity = x * z - y * w;
    if (singularity > 0.4999995f)
    {
        eulerX = atan2(x, w);
        eulerY = MathPiHalf<f32>();
        eulerZ = atan2(z, w);
    }
    else if (singularity < -0.4999995f)
    {
        eulerX = atan2(x, w);
        eulerY = -MathPiHalf<f32>();
        eulerZ = atan2(z, w);
    }
    else
    {
        eulerX = atan2(y * z + x * w, 0.5f - sqy - sqx);
        eulerY = -asin(singularity * 2.0f);
        eulerZ = atan2(y * x + z * w, 0.5f - sqy - sqz);
    }

    return {eulerX, eulerY, eulerZ};
}

std::tuple<Vector3, f32> Quaternion::ToAxisAndAngle() const
{
    return {GetRotationAxis(), GetAngle()};
}

Matrix3x3 Quaternion::ToMatrix() const
{
    Matrix3x3 result;

    f32 sqx = x * x;
    f32 sqy = y * y;
    f32 sqz = z * z;

    f32 doubleWX = 2.0f * w * x;
    f32 doubleWY = 2.0f * w * y;
    f32 doubleWZ = 2.0f * w * z;

    f32 doubleXY = 2.0f * x * y;
    f32 doubleXZ = 2.0f * x * z;
    f32 doubleYZ = 2.0f * y * z;

    f32 doubleSQX = 2.0f * sqx;
    f32 doubleSQY = 2.0f * sqy;
    f32 doubleSQZ = 2.0f * sqz;

    result[0][0] = 1.0f - doubleSQY - doubleSQZ;
    result[0][1] = doubleXY + doubleWZ;
    result[0][2] = doubleXZ - doubleWY;

    result[1][0] = doubleXY - doubleWZ;
    result[1][1] = 1.0f - doubleSQX - doubleSQZ;
    result[1][2] = doubleYZ + doubleWX;

    result[2][0] = doubleXZ + doubleWY;
    result[2][1] = doubleYZ - doubleWX;
    result[2][2] = 1.0f - doubleSQX - doubleSQY;

    return result;
}

bool Quaternion::EqualsWithEpsilon(const Quaternion &other, f32 epsilon) const
{
    if (abs(x - other.x) > epsilon)
    {
        return false;
    }
    if (abs(y - other.y) > epsilon)
    {
        return false;
    }
    if (abs(z - other.z) > epsilon)
    {
        return false;
    }
    if (abs(w - other.w) > epsilon)
    {
        return false;
    }
    return true;
}
