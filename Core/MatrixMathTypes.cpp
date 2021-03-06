#include "_PreHeader.hpp"
#include "MathFunctions.hpp"
#include "ApproxMath.hpp"
#include "_MatrixMathFunctions.hpp"

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

    template struct _VectorArithmeticBase<f32, 2>;
    template struct _VectorArithmeticBase<f32, 3>;
    template struct _VectorArithmeticBase<f32, 4>;

    template struct _VectorArithmeticBase<i32, 2>;
    template struct _VectorArithmeticBase<i32, 3>;
    template struct _VectorArithmeticBase<i32, 4>;

    template struct _VectorArithmeticBase<ui32, 2>;
    template struct _VectorArithmeticBase<ui32, 3>;
    template struct _VectorArithmeticBase<ui32, 4>;

	template struct _VectorBase<bool, 2>;
	template struct _VectorBase<bool, 3>;
	template struct _VectorBase<bool, 4>;

    template struct Vector2Base<_VectorArithmeticBase<f32, 2>>;
    template struct Vector3Base<_VectorArithmeticBase<f32, 3>>;
    template struct Vector4Base<_VectorArithmeticBase<f32, 4>>;

    template struct Vector2Base<_VectorArithmeticBase<i32, 2>>;
    template struct Vector3Base<_VectorArithmeticBase<i32, 3>>;
    template struct Vector4Base<_VectorArithmeticBase<i32, 4>>;

    template struct Vector2Base<_VectorArithmeticBase<ui32, 2>>;
    template struct Vector3Base<_VectorArithmeticBase<ui32, 3>>;
    template struct Vector4Base<_VectorArithmeticBase<ui32, 4>>;

	template struct Vector2Base<_VectorBase<bool, 2>>;
	template struct Vector3Base<_VectorBase<bool, 3>>;
	template struct Vector4Base<_VectorBase<bool, 4>>;

    template struct _VectorFP<Vector2Base<_VectorArithmeticBase<f32, 2>>>;
    template struct _VectorFP<Vector3Base<_VectorArithmeticBase<f32, 3>>>;
    template struct _VectorFP<Vector4Base<_VectorArithmeticBase<f32, 4>>>;

    template struct _Matrix<2, 2>;
    template struct _Matrix<3, 2>;
    template struct _Matrix<2, 3>;
    template struct _Matrix<3, 3>;
    template struct _Matrix<4, 3>;
    template struct _Matrix<3, 4>;
    template struct _Matrix<4, 4>;

    template struct Rectangle<f32>;
	template struct Rectangle<i32>;
	template struct Rectangle<ui32>;
}

template <uiw Rows, uiw Columns> static inline void _TransposeSquareMatrix(_Matrix<Rows, Columns> &matrix)
{
    static_assert(Rows == Columns);

    _ValidateValues(matrix);

    for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
        for (uiw columnIndex = rowIndex + 1; columnIndex < Columns; ++columnIndex)
            std::swap(matrix.elements[rowIndex][columnIndex], matrix.elements[columnIndex][rowIndex]);
}

template <typename MatrixType, bool isAllowTranslation> static inline void _SetTranslationScale(MatrixType &r, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale)
{
    if (scale)
    {
        _ValidateValues(*scale);

        for (uiw row = 0; row < 3; ++row)
        {
            for (uiw column = 0; column < 3; ++column)
            {
                r[row][column] *= scale->operator[](row);
            }
        }
    }

    if (isAllowTranslation && translation)
    {
        _ValidateValues(*translation);

        r[3][0] = translation->x;
        r[3][1] = translation->y;
        r[3][2] = translation->z;
    }
}

template <typename MatrixType, bool isAllowTranslation> static inline MatrixType _CreateRTS(const std::optional<Vector3> &rotation, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale)
{
    static_assert(MatrixType::rows >= (isAllowTranslation ? 4 : 3));
    static_assert(MatrixType::columns >= 3);

    MatrixType r;

    if (rotation)
    {
        _ValidateValues(*rotation);

        f32 cx = cos(rotation->x);
        f32 cy = cos(rotation->y);
        f32 cz = cos(rotation->z);

        f32 sx = sin(rotation->x);
        f32 sy = sin(rotation->y);
        f32 sz = sin(rotation->z);

        r[0][0] = cy * cz;
        r[0][1] = cy * sz;
        r[0][2] = -sy;

        r[1][0] = sx * sy * cz - cx * sz;
        r[1][1] = sx * sy * sz + cx * cz;
        r[1][2] = sx * cy;

        r[2][0] = cx * sy * cz + sx * sz;
        r[2][1] = cx * sy * sz - sx * cz;
        r[2][2] = cx * cy;
    }

    _SetTranslationScale<MatrixType, isAllowTranslation>(r, translation, scale);

    _ValidateValues(r);

    return r;
}

template <typename MatrixType, bool isAllowTranslation> static inline MatrixType _CreateRTS(const std::optional<Quaternion> &rotation, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale)
{
    static_assert(MatrixType::rows >= (isAllowTranslation ? 4 : 3));
    static_assert(MatrixType::columns >= 3);

    MatrixType r;

    if (rotation)
    {
        _ValidateValues(*rotation);

        Matrix3x3 rotationMatrix = rotation->ToMatrix();
        for (uiw row = 0; row < 3; ++row)
        {
            for (uiw column = 0; column < 3; ++column)
            {
                r[row][column] = rotationMatrix[row][column];
            }
        }
    }

    _SetTranslationScale<MatrixType, isAllowTranslation>(r, translation, scale);

    _ValidateValues(r);

    return r;
}

template <typename MatrixType, bool isAllowTranslation> static inline MatrixType _CreateRTS2D(const std::optional<f32> &rotation, const std::optional<Vector2> &translation, const std::optional<Vector2> &scale)
{
	MatrixType result;

	if (rotation)
	{
		_ValidateValues(*rotation);

		f32 cr = cos(*rotation);
		f32 sr = sin(*rotation);

		result[0][0] = cr; result[0][1] = -sr;
		result[1][0] = sr; result[1][1] = cr;
	}

	if (isAllowTranslation && translation)
	{
		_ValidateValues(*translation);
		result[2][0] = translation->x; result[2][1] = translation->y;
	}

	if (scale)
	{
		_ValidateValues(*scale);
		result[0][0] *= scale->x;
		result[0][1] *= scale->x;
		result[1][0] *= scale->y;
		result[1][1] *= scale->y;
	}

	_ValidateValues(result);
	return result;
}

// Inverse code is based on GLM

static inline std::optional<Matrix4x4> _Inverse4x4Matrix(const Matrix4x4 &m)
{
    Matrix4x4 r;

    f32 SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
    f32 SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
    f32 SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    f32 SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
    f32 SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    f32 SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    f32 SubFactor06 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
    f32 SubFactor07 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
    f32 SubFactor08 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
    f32 SubFactor09 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
    f32 SubFactor10 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
    f32 SubFactor11 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
    f32 SubFactor12 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    f32 SubFactor13 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
    f32 SubFactor14 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
    f32 SubFactor15 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
    f32 SubFactor16 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
    f32 SubFactor17 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
    f32 SubFactor18 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

    r[0][0] = +(m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02);
    r[1][0] = -(m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04);
    r[2][0] = +(m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05);
    r[3][0] = -(m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05);

    r[0][1] = -(m[0][1] * SubFactor00 - m[0][2] * SubFactor01 + m[0][3] * SubFactor02);
    r[1][1] = +(m[0][0] * SubFactor00 - m[0][2] * SubFactor03 + m[0][3] * SubFactor04);
    r[2][1] = -(m[0][0] * SubFactor01 - m[0][1] * SubFactor03 + m[0][3] * SubFactor05);
    r[3][1] = +(m[0][0] * SubFactor02 - m[0][1] * SubFactor04 + m[0][2] * SubFactor05);

    r[0][2] = +(m[0][1] * SubFactor06 - m[0][2] * SubFactor07 + m[0][3] * SubFactor08);
    r[1][2] = -(m[0][0] * SubFactor06 - m[0][2] * SubFactor09 + m[0][3] * SubFactor10);
    r[2][2] = +(m[0][0] * SubFactor11 - m[0][1] * SubFactor09 + m[0][3] * SubFactor12);
    r[3][2] = -(m[0][0] * SubFactor08 - m[0][1] * SubFactor10 + m[0][2] * SubFactor12);

    r[0][3] = -(m[0][1] * SubFactor13 - m[0][2] * SubFactor14 + m[0][3] * SubFactor15);
    r[1][3] = +(m[0][0] * SubFactor13 - m[0][2] * SubFactor16 + m[0][3] * SubFactor17);
    r[2][3] = -(m[0][0] * SubFactor14 - m[0][1] * SubFactor16 + m[0][3] * SubFactor18);
    r[3][3] = +(m[0][0] * SubFactor15 - m[0][1] * SubFactor17 + m[0][2] * SubFactor18);

    f32 det = +m[0][0] * r[0][0] + m[0][1] * r[1][0] + m[0][2] * r[2][0] + m[0][3] * r[3][0];
    
    if (Distance(det, 0.0f) < DefaultF32Epsilon)
    {
        return std::nullopt;
    }

    f32 revDet = 1.0f / det;
    r *= revDet;

    return r;
}

static inline std::optional<Matrix4x3> _Inverse4x3Matrix(const Matrix4x3 &m)
{
    Matrix4x3 r;

    f32 SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    f32 SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    f32 SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    f32 SubFactor08 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
    f32 SubFactor10 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
    f32 SubFactor12 = m[1][0] * m[3][1] - m[3][0] * m[1][1];

    r[0][0] = +(m[1][1] * m[2][2] - m[1][2] * m[2][1]);
    r[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]);
    r[2][0] = +(m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    r[3][0] = -(m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05);

    r[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]);
    r[1][1] = +(m[0][0] * m[2][2] - m[0][2] * m[2][0]);
    r[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]);
    r[3][1] = +(m[0][0] * SubFactor02 - m[0][1] * SubFactor04 + m[0][2] * SubFactor05);

    r[0][2] = +(m[0][1] * m[1][2] - m[0][2] * m[1][1]);
    r[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]);
    r[2][2] = +(m[0][0] * m[1][1] - m[0][1] * m[1][0]);
    r[3][2] = -(m[0][0] * SubFactor08 - m[0][1] * SubFactor10 + m[0][2] * SubFactor12);

    f32 det = +m[0][0] * r[0][0] + m[0][1] * r[1][0] + m[0][2] * r[2][0];
    
    if (Distance(det, 0.0f) < DefaultF32Epsilon)
    {
        return std::nullopt;
    }

    f32 revDet = 1.0f / det;
    r *= revDet;

    return r;
}

static inline std::optional<Matrix3x4> _Inverse3x4Matrix(const Matrix3x4 &m)
{
	Matrix3x4 r;

	f32 SubFactor13 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
	f32 SubFactor14 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
	f32 SubFactor15 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
	f32 SubFactor16 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
	f32 SubFactor17 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
	f32 SubFactor18 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

	r[0][0] = +(m[1][1] * m[2][2] - m[1][2] * m[2][1]);
	r[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]);
	r[2][0] = +(m[1][0] * m[2][1] - m[1][1] * m[2][0]);

	r[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]);
	r[1][1] = +(m[0][0] * m[2][2] - m[0][2] * m[2][0]);
	r[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]);

	r[0][2] = +(m[0][1] * m[1][2] - m[0][2] * m[1][1]);
	r[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]);
	r[2][2] = +(m[0][0] * m[1][1] - m[0][1] * m[1][0]);

	r[0][3] = -(m[0][1] * SubFactor13 - m[0][2] * SubFactor14 + m[0][3] * SubFactor15);
	r[1][3] = +(m[0][0] * SubFactor13 - m[0][2] * SubFactor16 + m[0][3] * SubFactor17);
	r[2][3] = -(m[0][0] * SubFactor14 - m[0][1] * SubFactor16 + m[0][3] * SubFactor18);

	f32 det = +m[0][0] * r[0][0] + m[0][1] * r[1][0] + m[0][2] * r[2][0];

	if (Distance(det, 0.0f) < DefaultF32Epsilon)
	{
		return std::nullopt;
	}

	f32 revDet = 1.0f / det;
	r *= revDet;

	return r;
}

static inline std::optional<Matrix2x2> _Inverse2x2Matrix(const Matrix2x2 &m)
{
    Matrix2x2 r;

    r[0][0] = +m[1][1];
    r[1][0] = -m[1][0];

    r[0][1] = -m[0][1];
    r[1][1] = +m[0][0];

    f32 det = m[0][0] * r[0][0] + m[0][1] * r[1][0];

    if (Distance(det, 0.0f) < DefaultF32Epsilon)
    {
        return std::nullopt;
    }

    f32 revDet = 1.0f / det;
    r *= revDet;

    return r;
}

static inline std::optional<Matrix3x2> _Inverse3x2Matrix(const Matrix3x2 &m)
{
	Matrix3x2 r;

	r[0][0] = +m[1][1];
	r[1][0] = -m[1][0];
	r[2][0] = +(m[1][0] * m[2][1] - m[1][1] * m[2][0]);

	r[0][1] = -m[0][1];
	r[1][1] = +m[0][0];
	r[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]);

	f32 det = +m[0][0] * r[0][0] + m[0][1] * r[1][0];

	if (Distance(det, 0.0f) < DefaultF32Epsilon)
	{
		return std::nullopt;
	}

	f32 revDet = 1.0f / det;
	r *= revDet;

	return r;
}

static inline std::optional<Matrix2x3> _Inverse2x3Matrix(const Matrix2x3 &m)
{
	Matrix2x3 r;

	r[0][0] = +m[1][1];
	r[1][0] = -m[1][0];

	r[0][1] = -m[0][1];
	r[1][1] = +m[0][0];

	r[0][2] = +(m[0][1] * m[1][2] - m[0][2] * m[1][1]);
	r[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]);

	f32 det = +m[0][0] * r[0][0] + m[0][1] * r[1][0];

	if (Distance(det, 0.0f) < DefaultF32Epsilon)
	{
		return std::nullopt;
	}

	f32 revDet = 1.0f / det;
	r *= revDet;

	return r;
}

static inline std::optional<Matrix3x3> _Inverse3x3Matrix(const Matrix3x3 &m)
{
	Matrix3x3 r;

	r[0][0] = +(m[1][1] * m[2][2] - m[1][2] * m[2][1]);
	r[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]);
	r[2][0] = +(m[1][0] * m[2][1] - m[1][1] * m[2][0]);

	r[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]);
	r[1][1] = +(m[0][0] * m[2][2] - m[0][2] * m[2][0]);
	r[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]);

	r[0][2] = +(m[0][1] * m[1][2] - m[0][2] * m[1][1]);
	r[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]);
	r[2][2] = +(m[0][0] * m[1][1] - m[0][1] * m[1][0]);

	f32 det = +m[0][0] * r[0][0] + m[0][1] * r[1][0] + m[0][2] * r[2][0];

	if (Distance(det, 0.0f) < DefaultF32Epsilon)
	{
		return std::nullopt;
	}

	f32 revDet = 1.0f / det;
	r *= revDet;

	return r;
}

template <typename MatrixType> static inline MatrixType _CreateRotationAroundAxis(const Vector3 &axis, f32 angle)
{
    MatrixType r;
    angle = -angle;

    f32 ca = cos(angle);
    f32 sa = sin(angle);

    r[0][0] = axis.x * axis.x + (1 - axis.x * axis.x) * ca;
    r[0][1] = axis.x * axis.y * (1 - ca) - axis.z * sa;
    r[0][2] = axis.x * axis.z * (1 - ca) + axis.y * sa;

    r[1][0] = axis.x * axis.y * (1 - ca) + axis.z * sa;
    r[1][1] = axis.y * axis.y + (1 - axis.y * axis.y) * ca;
    r[1][2] = axis.y * axis.z * (1 - ca) - axis.x * sa;

    r[2][0] = axis.x * axis.z * (1 - ca) - axis.y * sa;
    r[2][1] = axis.y * axis.z * (1 - ca) + axis.x * sa;
    r[2][2] = axis.z * axis.z + (1 - axis.z * axis.z) * ca;

    _ValidateValues(axis, angle, r);

    return r;
}

template <typename T> static inline T _CreateOrthographicMarix(const Vector3 &min, const Vector3 &max, ProjectionTarget target)
{
	T m;
    m[0][0] = 2.0f / (max.x - min.x);
    m[3][0] = -((max.x + min.x) / (max.x - min.x));
    m[1][1] = 2.0f / (max.y - min.y);
    m[3][1] = -((max.y + min.y) / (max.y - min.y));
    if (target == ProjectionTarget::OGL)
    {
        m[2][2] = -2.0f / (max.z - min.z);
        m[3][2] = -((max.z + min.z) / (max.z - min.z));
    }
    else if (target == ProjectionTarget::D3DAndMetal)
    {
        m[2][2] = 1.0f / (max.z - min.z);
        m[3][2] = min.z / (min.z - max.z);
    }
    else
    {
        ASSUME(target == ProjectionTarget::Vulkan);
        NOIMPL;
    }
	_ValidateValues(min, max, m);
    return m;
}

template <typename T, typename E> static inline void _DecomposeMatrix(const T &m, E *rotation, Vector3 *translation, Vector3 *scale)
{
	_ValidateValues(m);

	f32 sx = m.GetRow(0).Length();
	f32 sy = m.GetRow(1).Length();
	f32 sz = m.GetRow(2).Length();

	if (rotation)
	{
		f32 rsx = 1.0f / sx;
		f32 rsy = 1.0f / sy;
		f32 rsz = 1.0f / sz;
		Matrix3x3 rm = Matrix3x3(
			m.GetRow(0).ToVector3() * rsx,
			m.GetRow(1).ToVector3() * rsy,
			m.GetRow(2).ToVector3() * rsz);

		if constexpr (std::is_same_v<E, Matrix3x3>)
		{
			*rotation = rm;
		}
		else if constexpr (std::is_same_v<E, Vector3>)
		{
			float singy = sqrt(rm[0][0] * rm[0][0] + rm[0][1] * rm[0][1]);

			bool singular = singy < 0.0001f;

			f32 x, y, z;
			if (!singular)
			{
				x = atan2(rm[1][2], rm[2][2]);
				y = atan2(-rm[0][2], singy);
				z = atan2(rm[0][1], rm[0][0]);
			}
			else
			{
				x = atan2(-rm[2][1], rm[1][1]);
				y = atan2(-rm[0][2], singy);
				z = 0;
			}

			*rotation = {x, y, z};
		}
		else
		{
			*rotation = Quaternion::FromMatrix(rm);
		}

		_ValidateValues(*rotation);
	}

	if (translation)
	{
		*translation = m.GetRow(3).ToVector3();
		_ValidateValues(*translation);
	}

	if (scale)
	{
		*scale = {sx, sy, sz};
		_ValidateValues(*scale);
	}
}

template <typename T> static inline void _Decompose2DMatrix(const T &m, f32 *rotation, Vector2 *translation, Vector2 *scale)
{
	f32 sx = m.GetRow(0).ToVector2().Length();
	f32 sy = m.GetRow(1).ToVector2().Length();

	if (rotation)
	{
		f32 unscaled = m.elements[0][0] / sx;
		*rotation = acos(unscaled);
	}

	if (translation)
	{
		*translation = m.GetRow(2).ToVector2();
	}

	if (scale)
	{
		*scale = {sx, sy};
	}
}

/////////////
// Vector2 //
/////////////

Vector2 Vector2::GetLeftNormal() const
{
	_ValidateValues(*this);
	return {-y, x};
}

Vector2 Vector2::GetRightNormal() const
{
	_ValidateValues(*this);
	return {y, -x};
}

/////////////
// Vector3 //
/////////////

Vector3 &Vector3::Cross(const Vector3 &other)
{
	_ValidateValues(*this, other);

	f32 nx = this->y * other.z - this->z * other.y;
    f32 ny = this->z * other.x - this->x * other.z;
    f32 nz = this->x * other.y - this->y * other.x;

    x = nx;
    y = ny;
    z = nz;

    return *this;
}

Vector3 Vector3::GetCrossed(const Vector3 &other) const
{
	_ValidateValues(*this, other);

	f32 nx = this->y * other.z - this->z * other.y;
    f32 ny = this->z * other.x - this->x * other.z;
    f32 nz = this->x * other.y - this->y * other.x;

    return Vector3(nx, ny, nz);
}

///////////////
// Matrix3x2 //
///////////////

std::optional<Matrix3x2> Matrix3x2::GetInversed() const
{
	_ValidateValues(*this);
	return _Inverse3x2Matrix(*this);
}

f32 Matrix3x2::Determinant() const
{
	_ValidateValues(*this);
	auto &m = *this;
	return +m[0][0] * m[1][1] + m[0][1] * -m[1][0];
}

void Matrix3x2::Decompose(f32 *rotation, Vector2 *translation, Vector2 *scale) const
{
	_Decompose2DMatrix(*this, rotation, translation, scale);
}

Matrix3x2 Matrix3x2::CreateRTS(const std::optional<f32> &rotation, const std::optional<Vector2> &translation, const std::optional<Vector2> &scale)
{
	return _CreateRTS2D<Matrix3x2, true>(rotation, translation, scale);
}

///////////////
// Matrix4x3 //
///////////////

std::optional<Matrix4x3> Matrix4x3::GetInversed() const
{
	_ValidateValues(*this);
	return _Inverse4x3Matrix(*this);
}

f32 Matrix4x3::Determinant() const
{
	_ValidateValues(*this);
	auto &m = *this;

	f32 fac0 = +(m[1][1] * m[2][2] - m[1][2] * m[2][1]);
	f32 fac1 = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]);
	f32 fac2 = +(m[1][0] * m[2][1] - m[1][1] * m[2][0]);

	return +m[0][0] * fac0 + m[0][1] * fac1 + m[0][2] * fac2;
}

void Matrix4x3::Decompose(Matrix3x3 *rotation, Vector3 *translation, Vector3 *scale) const
{
	_DecomposeMatrix(*this, rotation, translation, scale);
}

void Matrix4x3::Decompose(Vector3 *rotation, Vector3 *translation, Vector3 *scale) const
{
	_DecomposeMatrix(*this, rotation, translation, scale);
}

void Matrix4x3::Decompose(Quaternion *rotation, Vector3 *translation, Vector3 *scale) const
{
	_DecomposeMatrix(*this, rotation, translation, scale);
}

Matrix4x4 Matrix4x3::operator * (const Matrix4x4 &other) const
{
    Matrix4x4 r;
    for (uiw row = 0; row < 4; ++row)
    {
        for (uiw column = 0; column < 4; ++column)
        {
            r[row][column] =
                elements[row][0] * other[0][column] +
                elements[row][1] * other[1][column] +
                elements[row][2] * other[2][column];
        }
    }
	r[3][0] += other[3][0];
	r[3][1] += other[3][1];
	r[3][2] += other[3][2];
	r[3][3] += other[3][3];
	_ValidateValues(*this, other, r);
	return r;
}

Matrix4x3 Matrix4x3::CreateRotationAroundAxis(const Vector3 &axis, f32 angle)
{
    return _CreateRotationAroundAxis<Matrix4x3>(axis, angle);
}

Matrix4x3 Matrix4x3::CreateRTS(const std::optional<Vector3> &rotation, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale)
{
    return _CreateRTS<Matrix4x3, true>(rotation, translation, scale);
}

Matrix4x3 Matrix4x3::CreateRTS(const std::optional<Quaternion> &rotation, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale)
{
    return _CreateRTS<Matrix4x3, true>(rotation, translation, scale);
}

Matrix4x3 Matrix4x3::CreateOrthographicProjection(const Vector3 &min, const Vector3 &max, ProjectionTarget target)
{
    return _CreateOrthographicMarix<Matrix4x3>(min, max, target);
}

///////////////
// Matrix3x4 //
///////////////

std::optional<Matrix3x4> Matrix3x4::GetInversed() const
{
	_ValidateValues(*this);
	return _Inverse3x4Matrix(*this);
}

f32 Matrix3x4::Determinant() const
{
	_ValidateValues(*this);
	auto &m = *this;

	f32 fac0 = +(m[1][1] * m[2][2] - m[1][2] * m[2][1]);
	f32 fac1 = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]);
	f32 fac2 = +(m[1][0] * m[2][1] - m[1][1] * m[2][0]);

	return +m[0][0] * fac0 + m[0][1] * fac1 + m[0][2] * fac2;
}

Matrix3x4 Matrix3x4::CreateRotationAroundAxis(const Vector3 &axis, f32 angle)
{
    return _CreateRotationAroundAxis<Matrix3x4>(axis, angle);
}

Matrix3x4 Matrix3x4::CreateRS(const std::optional<Vector3> &rotation, const std::optional<Vector3> &scale)
{
    return _CreateRTS<Matrix3x4, false>(rotation, std::nullopt, scale);
}

Matrix3x4 Matrix3x4::CreateRS(const std::optional<Quaternion> &rotation, const std::optional<Vector3> &scale)
{
    return _CreateRTS<Matrix3x4, false>(rotation, std::nullopt, scale);
}

///////////////
// Matrix4x4 //
///////////////

Matrix4x4 &Matrix4x4::Transpose()
{
	_TransposeSquareMatrix(*this);
    return *this;
}

std::optional<Matrix4x4> Matrix4x4::GetInversed() const
{
	_ValidateValues(*this);
	return _Inverse4x4Matrix(*this);
}

f32 Matrix4x4::Determinant() const
{
	_ValidateValues(*this);
	auto &m = *this;

	f32 SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
	f32 SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	f32 SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	f32 SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	f32 SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	f32 SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

	f32 fac0 = +(m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02);
	f32 fac1 = -(m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04);
	f32 fac2 = +(m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05);
	f32 fac3 = -(m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05);

	return +m[0][0] * fac0 + m[0][1] * fac1 + m[0][2] * fac2 + m[0][3] * fac3;
}

void Matrix4x4::Decompose(Matrix3x3 *rotation, Vector3 *translation, Vector3 *scale) const
{
	_DecomposeMatrix(*this, rotation, translation, scale);
}

void Matrix4x4::Decompose(Vector3 *rotation, Vector3 *translation, Vector3 *scale) const
{
	_DecomposeMatrix(*this, rotation, translation, scale);
}

void Matrix4x4::Decompose(Quaternion *rotation, Vector3 *translation, Vector3 *scale) const
{
	_DecomposeMatrix(*this, rotation, translation, scale);
}

Matrix4x4 Matrix4x4::CreateRotationAroundAxis(const Vector3 &axis, f32 angle)
{
    return _CreateRotationAroundAxis<Matrix4x4>(axis, angle);
}

Matrix4x4 Matrix4x4::CreateRTS(const std::optional<Vector3> &rotation, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale)
{
    return _CreateRTS<Matrix4x4, true>(rotation, translation, scale);
}

Matrix4x4 Matrix4x4::CreateRTS(const std::optional<Quaternion> &rotation, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale)
{
    return _CreateRTS<Matrix4x4, true>(rotation, translation, scale);
}

Matrix4x4 Matrix4x4::CreatePerspectiveProjection(f32 horizontalFOVRad, f32 aspectRatio, f32 nearPlane, f32 farPlane, ProjectionTarget target)
{
	ASSUME(horizontalFOVRad > DefaultF32Epsilon && horizontalFOVRad < MathPi() && aspectRatio > DefaultF32Epsilon && nearPlane > DefaultF32Epsilon && farPlane > DefaultF32Epsilon);

    Matrix4x4 r;

    f32 fovH = horizontalFOVRad;
    f32 tanOfHalfFovH = tan(fovH * 0.5f);
    f32 fovV = 2.0f * atan(tanOfHalfFovH / aspectRatio);
    f32 q = farPlane / (farPlane - nearPlane);
    f32 w = 1.0f / tanOfHalfFovH;
    f32 h = 1.0f / tan(fovV * 0.5f);
    f32 l = -q * nearPlane;
    f32 d = 1;

    if (target == ProjectionTarget::Vulkan)
    {
        h = -h;
    }
    else if (target == ProjectionTarget::OGL)
    {
        q = -(farPlane + nearPlane) / (farPlane - nearPlane);
        l = (-2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        d = -1;
    }

    r[0][0] = w; r[0][1] = 0; r[0][2] = 0; r[0][3] = 0;
    r[1][0] = 0; r[1][1] = h; r[1][2] = 0; r[1][3] = 0;
    r[2][0] = 0; r[2][1] = 0; r[2][2] = q; r[2][3] = d;
    r[3][0] = 0; r[3][1] = 0; r[3][2] = l; r[3][3] = 0;

	_ValidateValues(r);
    return r;
}

Matrix4x4 Matrix4x4::CreateOrthographicProjection(const Vector3 &min, const Vector3 &max, ProjectionTarget target)
{
    auto m = _CreateOrthographicMarix<Matrix4x4>(min, max, target);
    m[3][3] = 1.0f;
    return m;
}

///////////////
// Matrix2x3 //
///////////////

[[nodiscard]] std::optional<Matrix2x3> Matrix2x3::GetInversed() const
{
	_ValidateValues(*this);
	return _Inverse2x3Matrix(*this);
}

f32 Matrix2x3::Determinant() const
{
	_ValidateValues(*this);
	auto &m = *this;
	return +m[0][0] * m[1][1] + m[0][1] * -m[1][0];
}

///////////////
// Matrix2x2 //
///////////////

Matrix2x2 &Matrix2x2::Transpose()
{
	_TransposeSquareMatrix(*this);
    return *this;
}

void Matrix2x2::Decompose(f32 *rotation, Vector2 *scale) const
{
	_Decompose2DMatrix(*this, rotation, nullptr, scale);
}

std::optional<Matrix2x2> Matrix2x2::GetInversed() const
{
	_ValidateValues(*this);
	return _Inverse2x2Matrix(*this);
}

f32 Matrix2x2::Determinant() const
{
	_ValidateValues(*this);
	auto &m = *this;
	return m[0][0] * m[1][1] + m[0][1] * -m[1][0];
}

Matrix2x2 Matrix2x2::CreateRS(const std::optional<f32> &rotation, const std::optional<Vector2> &scale)
{
	return _CreateRTS2D<Matrix2x2, false>(rotation, std::nullopt, scale);
}

///////////////
// Matrix3x3 //
///////////////

Matrix3x3 &Matrix3x3::Transpose()
{
	_TransposeSquareMatrix(*this);
    return *this;
}

std::optional<Matrix3x3> Matrix3x3::GetInversed() const
{
	_ValidateValues(*this);
	return _Inverse3x3Matrix(*this);
}

f32 Matrix3x3::Determinant() const
{
	_ValidateValues(*this);

	auto &m = *this;

	f32 fac0 = +(m[1][1] * m[2][2] - m[1][2] * m[2][1]);
	f32 fac1 = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]);
	f32 fac2 = +(m[1][0] * m[2][1] - m[1][1] * m[2][0]);

	return +m[0][0] * fac0 + m[0][1] * fac1 + m[0][2] * fac2;
}

void Matrix3x3::Decompose(Matrix3x3 *rotation, Vector3 *scale) const
{
	_DecomposeMatrix(*this, rotation, nullptr, scale);
}

void Matrix3x3::Decompose(Vector3 *rotation, Vector3 *scale) const
{
	_DecomposeMatrix(*this, rotation, nullptr, scale);
}

void Matrix3x3::Decompose(Quaternion *rotation, Vector3 *scale) const
{
	_DecomposeMatrix(*this, rotation, nullptr, scale);
}

void Matrix3x3::Decompose(f32 *rotation, Vector2 *translation, Vector2 *scale) const
{
	_Decompose2DMatrix(*this, rotation, translation, scale);
}

Matrix3x3 Matrix3x3::CreateRotationAroundAxis(const Vector3 &axis, f32 angle)
{
    return _CreateRotationAroundAxis<Matrix3x3>(axis, angle);
}

Matrix3x3 Matrix3x3::CreateRS(const std::optional<Vector3> &rotation, const std::optional<Vector3> &scale)
{
    return _CreateRTS<Matrix3x3, false>(rotation, std::nullopt, scale);
}

Matrix3x3 Matrix3x3::CreateRS(const std::optional<Quaternion> &rotation, const std::optional<Vector3> &scale)
{
    return _CreateRTS<Matrix3x3, false>(rotation, std::nullopt, scale);
}

Matrix3x3 Matrix3x3::CreateRTS(const std::optional<f32> &rotation, const std::optional<Vector2> &translation, const std::optional<Vector2> &scale)
{
	return _CreateRTS2D<Matrix3x3, true>(rotation, translation, scale);
}

////////////////
// Quaternion //
////////////////

// based on https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
Quaternion Quaternion::FromEuler(const Vector3 &source)
{
    f32 cx = cos(source.x * 0.5f);
    f32 cy = cos(source.y * 0.5f);
    f32 cz = cos(source.z * 0.5f);

    f32 sx = sin(source.x * 0.5f);
    f32 sy = sin(source.y * 0.5f);
    f32 sz = sin(source.z * 0.5f);

    f32 w = cx * cy * cz + sx * sy * sz;
    f32 x = sx * cy * cz - cx * sy * sz;
    f32 y = cx * sy * cz + sx * cy * sz;
    f32 z = cx * cy * sz - sx * sy * cz;

	Quaternion result = {x, y, z, w};

	_ValidateValues(source, result);
	return result;
}

Quaternion Quaternion::FromMatrix(const Matrix3x3 &matrix)
{
#ifdef DEBUG
	Vector3 debugScale;
	matrix.Decompose(static_cast<Vector3 *>(nullptr), &debugScale);
	ASSUME(debugScale.EqualsWithEpsilon({1, 1, 1}));
#endif

	Quaternion q;

	const f32 trace = matrix[0][0] + matrix[1][1] + matrix[2][2];

	if (trace > 0.0f)
	{
		f32 s = sqrt(trace + 1.0f);
		q.w = s * 0.5f;
		s = 0.5f / s;

		q.x = (matrix[1][2] - matrix[2][1]) * s;
		q.y = (matrix[2][0] - matrix[0][2]) * s;
		q.z = (matrix[0][1] - matrix[1][0]) * s;
	}
	else
	{
		static constexpr uiw nxt[3] = { 1, 2, 0 };
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

		q[i] = s * 0.5f;
		s = 0.5f / s;

		q[3] = (matrix[j][k] - matrix[k][j]) * s;
		q[j] = (matrix[i][j] + matrix[j][i]) * s;
		q[k] = (matrix[i][k] + matrix[k][i]) * s;
	}

	_ValidateValues(matrix, q);
	return q;
}

Quaternion Quaternion::FromAxis(const Vector3 &axis, f32 angleRad)
{
	ASSUME(axis.IsNormalized());

	const f32 halfAngle = 0.5f * angleRad;
	const f32 s = sin(halfAngle);

	Quaternion result =
	{
		s * axis.x,
		s * axis.y,
		s * axis.z,
		cos(halfAngle)
	};

	_ValidateValues(axis, angleRad, result);
	return result;
}

Quaternion::Quaternion(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w)
{
	_ValidateValues(*this);
}

Quaternion Quaternion::operator + (const Quaternion &other) const
{
	Quaternion added = {x + other.x, y + other.y, z + other.z, w + other.w};
	_ValidateValues(*this, other, added);
	return added;
}

Quaternion &Quaternion::operator += (const Quaternion &other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
	_ValidateValues(*this, other);
    return *this;
}

Quaternion Quaternion::operator - (const Quaternion &other) const
{
	Quaternion subbed = {x - other.x, y - other.y, z - other.z, w - other.w};
	_ValidateValues(*this, other, subbed);
	return subbed;
}

Quaternion &Quaternion::operator -= (const Quaternion &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
	_ValidateValues(*this, other);
    return *this;
}

static inline Quaternion QuaternionMul(const Quaternion &left, const Quaternion &right)
{
	_ValidateValues(left, right);
	f32 x = right.w * left.x + right.x * left.w + right.y * left.z - right.z * left.y;
	f32 y = right.w * left.y + right.y * left.w + right.z * left.x - right.x * left.z;
	f32 z = right.w * left.z + right.z * left.w + right.x * left.y - right.y * left.x;
	f32 w = right.w * left.w - right.x * left.x - right.y * left.y - right.z * left.z;
	_ValidateValues(x, y, z, w);
	return {x, y, z, w};
}

Quaternion Quaternion::operator * (const Quaternion &other) const
{
    return QuaternionMul(*this, other);
}

Quaternion &Quaternion::operator *= (const Quaternion &other)
{
	*this = QuaternionMul(*this, other);
    return *this;
}

Quaternion Quaternion::operator * (f32 scale) const
{
    Quaternion scaled = {x * scale, y * scale, z * scale, w * scale};
	_ValidateValues(*this, scale, scaled);
	return scaled;
}

Quaternion &Quaternion::operator *= (f32 scale)
{
    x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;
	_ValidateValues(*this, scale);
    return *this;
}

Quaternion Quaternion::operator / (f32 scale) const
{
    f32 r = 1.0f / scale;
    Quaternion scaled = {x * r, y * r, z * r, w * r};
	_ValidateValues(*this, scale, scaled);
	return *this;
}

Quaternion &Quaternion::operator /= (f32 scale)
{
    f32 r = 1.0f / scale;
    x *= r;
    y *= r;
    z *= r;
    w *= r;
	_ValidateValues(*this, scale);
    return *this;
}

f32 &Quaternion::operator[](uiw index)
{
	ASSUME(index < 4);
	_ValidateValues(*this);
	return (&x)[index];
}

const f32 &Quaternion::operator[](uiw index) const
{
	ASSUME(index < 4);
	_ValidateValues(*this);
	return (&x)[index];
}

std::array<f32, 4> &Quaternion::Data()
{
	_ValidateValues(*this);
    return *reinterpret_cast<std::array<f32, 4> *>(&x);
}

const std::array<f32, 4> &Quaternion::Data() const
{
	_ValidateValues(*this);
    return *reinterpret_cast<const std::array<f32, 4> *>(&x);
}

Vector3 Quaternion::RotateVector(const Vector3 &source) const
{
    Vector3 u{x, y, z};
    Vector3 v = source;
    Vector3 uv = u.GetCrossed(v);
    Vector3 uuv = u.GetCrossed(uv);
    Vector3 rotated = v + ((uv * w) + uuv) * 2.0f;
	_ValidateValues(*this, source, rotated);
	return rotated;
}

Quaternion &Quaternion::Normalize()
{
    f32 r = ApproxMath::RSqrt<ApproxMath::Precision::High>(LengthSquare());
    x *= r;
    y *= r;
    z *= r;
    w *= r;
	_ValidateValues(*this);
    return *this;
}

Quaternion Quaternion::GetNormalized() const
{
    f32 r = ApproxMath::RSqrt<ApproxMath::Precision::High>(LengthSquare());
    Quaternion normalized = {x * r, y * r, z * r, w * r};
	_ValidateValues(*this, normalized);
	return normalized;
}

bool Quaternion::IsNormalized() const
{
	return StdLib::EqualsWithEpsilon(LengthSquare(), 1.0f);
}

f32 Quaternion::Length() const
{
	f32 len = sqrt(LengthSquare());
	_ValidateValues(*this, len);
	return len;
}

f32 Quaternion::LengthSquare() const
{
	f32 len = x * x + y * y + z * z + w * w;
	_ValidateValues(*this, len);
	return len;
}

Quaternion &Quaternion::Inverse()
{
    x = -x;
    y = -y;
    z = -z;
	_ValidateValues(*this);
    return *this;
}

Quaternion Quaternion::GetInversed() const
{
	_ValidateValues(*this);
    return {-x, -y, -z, w};
}

Vector3 Quaternion::ToEuler() const
{
    // based on https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    const f32 sqx = x * x;
    const f32 sqy = y * y;
    const f32 sqz = z * z;

    f32 sinr_cosp = 2.0f * (w * x + y * z);
    f32 cosr_cosp = 1.0f - 2.0f * (sqx + sqy);
    f32 eulerX = atan2(sinr_cosp, cosr_cosp);

    f32 eulerY;
    f32 sinp = 2.0f * (w * y - z * x);
    if (abs(sinp) >= 1)
    {
        eulerY = copysign(MathPiHalf(), sinp); // use 90 degrees if out of range
    }
    else
    {
        eulerY = asin(sinp);
    }

    f32 siny_cosp = 2.0f * (w * z + x * y);
    f32 cosy_cosp = 1.0f - 2.0f * (sqy + sqz);
    f32 eulerZ = atan2(siny_cosp, cosy_cosp);

    if (eulerX < 0) eulerX += MathPiDouble();
    if (eulerY < 0) eulerY += MathPiDouble();
    if (eulerZ < 0) eulerZ += MathPiDouble();

    ASSUME(eulerX < MathPiDouble() + DefaultF32Epsilon);
    ASSUME(eulerY < MathPiDouble() + DefaultF32Epsilon);
    ASSUME(eulerZ < MathPiDouble() + DefaultF32Epsilon);

    Vector3 euler = {eulerX, eulerY, eulerZ};
        
    // based on https://math.stackexchange.com/questions/1477926/quaternion-to-euler-with-some-properties
    //const f32 sqx = x * x;
    //const f32 sqy = y * y;
    //const f32 sqz = z * z;

    //f32 eulerX, eulerY, eulerZ;

    //f32 singularity = 2.0f * (x * z - y * w);
    //if (singularity > 0.99995f)
    //{
    //    eulerX = atan2(x, w);
    //    eulerY = -MathPiHalf();
    //    eulerZ = atan2(z, w);
    //}
    //else if (singularity < -0.99995f)
    //{
    //    eulerX = atan2(x, w);
    //    eulerY = MathPiHalf();
    //    eulerZ = atan2(z, w);
    //}
    //else
    //{
    //    eulerX = atan2(y * z + x * w, 0.5f - sqy - sqx);
    //    eulerY = -asin(singularity);
    //    eulerZ = atan2(y * x + z * w, 0.5f - sqy - sqz);
    //}

    //if (eulerX < 0) eulerX += MathPiDouble();
    //if (eulerY < 0) eulerY += MathPiDouble();
    //if (eulerZ < 0) eulerZ += MathPiDouble();

    //ASSUME(eulerX < MathPiDouble() + DefaultF32Epsilon);
    //ASSUME(eulerY < MathPiDouble() + DefaultF32Epsilon);
    //ASSUME(eulerZ < MathPiDouble() + DefaultF32Epsilon);

	// Vector3 euler = {eulerX, eulerY, eulerZ};

    // based on https://www.gamedev.net/forums/topic/166424-quaternion-to-euler/
    //f32 sqw = w * w;
    //f32 sqx = x * x;
    //f32 sqy = y * y;
    //f32 sqz = z * z;

    //f32 rotxrad = atan2(2.0f * (y * z + x * w), (-sqx - sqy + sqz + sqw));
    //f32 rotyrad = asin(-2.0f * (x * z - y * w));
    //f32 rotzrad = atan2(2.0f * (x * y + z * w), (sqx - sqy - sqz + sqw));

    //if (eulerX < 0) eulerX += MathPiDouble();
    //if (eulerY < 0) eulerY += MathPiDouble();
    //if (eulerZ < 0) eulerZ += MathPiDouble();

    //ASSUME(eulerX < MathPiDouble() + DefaultF32Epsilon);
    //ASSUME(eulerY < MathPiDouble() + DefaultF32Epsilon);
    //ASSUME(eulerZ < MathPiDouble() + DefaultF32Epsilon);

	//Vector3 euler = {rotxrad, rotyrad, rotzrad};

	_ValidateValues(*this, euler);
	return euler;
}

std::tuple<Vector3, f32> Quaternion::ToAxisAndAngle() const
{
	ASSUME(IsNormalized());
	Vector3 axis;
	f32 r = ApproxMath::RSqrt<ApproxMath::Precision::High>(1.0f - (w * w));
	if (r < 0.0001f)
	{
		axis = {x, y, z};
	}
	else
	{
		axis = {x * r, y * r, z * r};
	}
	f32 angle = 2.0f * acos(w);
	_ValidateValues(*this, axis, angle);
	return {axis, angle};
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

	_ValidateValues(*this, result);
    return result;
}

bool Quaternion::EqualsWithEpsilon(const Quaternion &other, f32 epsilon) const
{
	_ValidateValues(*this, other, epsilon);
	return StdLib::EqualsWithEpsilon({x, y, z, w}, {other.x, other.y, other.z, other.w}, epsilon);
}

uiw Quaternion::Hash() const
{
	_ValidateValues(*this);
	uiw hash = 0x811c9dc5u;
	for (uiw index = 0; index < 4; ++index)
	{
		hash ^= std::hash<f32>()(this->operator[](index));
		hash *= 16777619u;
	}
	return hash;
}