#pragma once

#include "MatrixMathTypes.hpp"
#include "ApproxMath.hpp"

namespace StdLib
{
	template <uiw count> void __ValidateValuesArray(const f32 *values)
	{
		for (uiw index = 0; index < count; ++index)
		{
			int c = std::fpclassify(values[index]);
			ASSUME(c != FP_INFINITE);
			ASSUME(c != FP_NAN);
			ASSUME(c != FP_SUBNORMAL);
		}
	}

	template <typename ScalarType, uiw Dim> constexpr bool __IsVector(_VectorBase<ScalarType, Dim>) { return true; }
	template <typename ScalarType = void, uiw Dim = 0> constexpr bool __IsVector(f32) { return false; }

	template <typename T> void __ValidateValues(const T &v)
	{
		if constexpr (__IsVector(T()))
		{
			if constexpr (std::is_same_v<T::ScalarType, f32>)
			{
				__ValidateValuesArray<T::dim>(&v.x);
			}
		}
		else if constexpr (std::is_same_v<T, f32>)
		{
			__ValidateValuesArray<1>(&v);
		}
	}

	template <typename T0, typename T1 = i32, typename T2 = i32> void _ValidateValues(const T0 &v0, const T1 &v1 = {}, const T2 &v2 = {})
	{
		__ValidateValues(v0);
		__ValidateValues(v1);
		__ValidateValues(v2);
	}

    /////////////////
    // _VectorBase //
    /////////////////

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator + (const _VectorBase &other) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            result.Data()[index] = Data()[index] + other.Data()[index];
		_ValidateValues(*this, other, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator + (ScalarType scalar) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            result.Data()[index] = Data()[index] + scalar;
		_ValidateValues(*this, scalar, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator += (const _VectorBase &other) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            Data()[index] += other.Data()[index];
		_ValidateValues(*this, other);
		return *(VectorType *)this;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator += (ScalarType scalar) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            Data()[index] += scalar;
        return *(VectorType *)this;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator - (const _VectorBase &other) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            result.Data()[index] = Data()[index] - other.Data()[index];
		_ValidateValues(*this, other, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator - (ScalarType scalar) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            result.Data()[index] = Data()[index] - scalar;
		_ValidateValues(*this, scalar, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator -= (const _VectorBase &other) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            Data()[index] -= other.Data()[index];
		_ValidateValues(*this, other);
        return *(VectorType *)this;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator -= (ScalarType scalar) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            Data()[index] -= scalar;
		_ValidateValues(*this, scalar);
        return *(VectorType *)this;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator * (const _VectorBase &other) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            result.Data()[index] = Data()[index] * other.Data()[index];
		_ValidateValues(*this, other, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator * (ScalarType scalar) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            result.Data()[index] = Data()[index] * scalar;
		_ValidateValues(*this, scalar, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator *= (const _VectorBase &other) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            Data()[index] *= other.Data()[index];
		_ValidateValues(*this, other);
        return *(VectorType *)this;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator *= (ScalarType scalar) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            Data()[index] *= scalar;
		_ValidateValues(*this, scalar);
        return *(VectorType *)this;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator / (const _VectorBase &other) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            result.Data()[index] = Data()[index] / other.Data()[index];
		_ValidateValues(*this, other, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator / (ScalarType scalar) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            result.Data()[index] = Data()[index] / scalar;
		_ValidateValues(*this, scalar, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator /= (const _VectorBase &other) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            Data()[index] /= other.Data()[index];
		_ValidateValues(*this, other);
        return *(VectorType *)this;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorBase<ScalarType, Dim>::operator /= (ScalarType scalar) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            Data()[index] /= scalar;
		_ValidateValues(*this, scalar);
        return *(VectorType *)this;
    }

	template<typename _ScalarType, uiw Dim>	inline bool _VectorBase<_ScalarType, Dim>::operator == (const _VectorBase &other) const
	{
		_ValidateValues(*this, other);
		for (uiw index = 0; index < Dim; ++index)
			if (Data()[index] != other.Data()[index])
				return false;
		return true;
	}

	template<typename _ScalarType, uiw Dim>	inline bool _VectorBase<_ScalarType, Dim>::operator != (const _VectorBase &other) const
	{
		return !operator==(other);
	}

    template<typename ScalarType, uiw Dim> inline ScalarType *_VectorBase<ScalarType, Dim>::Data()
    {
		_ValidateValues(*this);
        return &x;
    }

    template<typename ScalarType, uiw Dim> inline const ScalarType *_VectorBase<ScalarType, Dim>::Data() const
    {
		_ValidateValues(*this);
        return &x;
    }

    template <typename ScalarType, uiw Dim> inline ScalarType _VectorBase<ScalarType, Dim>::Accumulate() const
    {
        ScalarType sum = x;
        for (uiw index = 1; index < dim; ++index)
            sum += Data()[index];
		_ValidateValues(*this, sum);
        return sum;
    }

    template <typename ScalarType, uiw Dim> inline ScalarType _VectorBase<ScalarType, Dim>::Max() const
    {
        ScalarType max = x;
        for (uiw index = 1; index < dim; ++index)
            if (Data()[index] > max)
                max = Data()[index];
		_ValidateValues(*this, max);
        return max;
    }

    template <typename ScalarType, uiw Dim> inline ScalarType _VectorBase<ScalarType, Dim>::Min() const
    {
        ScalarType min = x;
        for (uiw index = 1; index < dim; ++index)
            if (Data()[index] < min)
                min = Data()[index];
		_ValidateValues(*this, min);
        return min;
    }

    /////////////////
    // Vector2Base //
    /////////////////

    template<typename ScalarType> inline ScalarType &Vector2Base<ScalarType>::operator[](uiw index)
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        ASSUME(index == 1);
        return y;
    }

    template<typename ScalarType> inline const ScalarType &Vector2Base<ScalarType>::operator[](uiw index) const
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        ASSUME(index == 1);
        return y;
    }

    /////////////////
    // Vector3Base //
    /////////////////

    template<typename ScalarType> inline ScalarType &Vector3Base<ScalarType>::operator[](uiw index)
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        if (index == 1) return y;
        ASSUME(index == 2);
        return z;
    }

    template<typename ScalarType> inline const ScalarType &Vector3Base<ScalarType>::operator[](uiw index) const
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        if (index == 1) return y;
        ASSUME(index == 2);
        return z;
    }

    template<typename ScalarType> inline VectorTypeByDimension<ScalarType, 2> Vector3Base<ScalarType>::ToVector2() const
    {
		_ValidateValues(*this);
        return {x, y};
    }

    /////////////////
    // Vector4Base //
    /////////////////

    template<typename ScalarType> inline ScalarType &Vector4Base<ScalarType>::operator[](uiw index)
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;
        ASSUME(index == 3);
        return w;
    }

    template<typename ScalarType> inline const ScalarType &Vector4Base<ScalarType>::operator[](uiw index) const
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;
        ASSUME(index == 3);
        return w;
    }

    template<typename ScalarType> inline VectorTypeByDimension<ScalarType, 2> Vector4Base<ScalarType>::ToVector2() const
    {
		_ValidateValues(*this);
        return {x, y};
    }

    template<typename ScalarType> inline VectorTypeByDimension<ScalarType, 3> Vector4Base<ScalarType>::ToVector3() const
    {
		_ValidateValues(*this);
        return {x, y, z};
    }

    /////////////
    // _Vector //
    /////////////
        
    template <typename Basis> inline f32 _VectorFP<Basis>::Length() const
    {
        f32 len = sqrt(LengthSquare());
		_ValidateValues(len);
		return len;
    }

    template <typename Basis> inline f32 _VectorFP<Basis>::LengthSquare() const
    {
        f32 sum = x * x;
        for (uiw index = 1; index < dim; ++index)
            sum += operator[](index) * operator[](index);
		_ValidateValues(*this, sum);
        return sum;
    }

    template <typename Basis> inline f32 _VectorFP<Basis>::Distance(const _VectorFP &other) const
    {
        f32 dist = sqrt(DistanceSquare(other));
		_ValidateValues(dist);
		return dist;
    }

    template <typename Basis> inline f32 _VectorFP<Basis>::DistanceSquare(const _VectorFP &other) const
    {
        f32 dist = x - other.x;
        f32 sum = dist * dist;
        for (uiw index = 1; index < dim; ++index)
        {
            dist = operator[](index) - other[index];
            sum += dist * dist;
        }
		_ValidateValues(*this, sum);
        return sum;
    }

    template <typename Basis> inline f32 _VectorFP<Basis>::Dot(const _VectorFP &other) const
    {
        f32 sum = x * other.x;
        for (uiw index = 1; index < dim; ++index)
            sum += operator[](index) * other[index];
		_ValidateValues(*this, sum);
        return sum;
    }

    template <typename Basis> inline f32 _VectorFP<Basis>::Average() const
    {
        f32 avg = Accumulate() / dim;
		_ValidateValues(avg);
		return avg;
    }

    template <typename Basis> inline void _VectorFP<Basis>::Normalize()
    {
        f32 revLength = ApproxMath::RSqrt<ApproxMath::Precision::High>(LengthSquare());
        for (uiw index = 0; index < dim; ++index)
            operator[](index) *= revLength;
		_ValidateValues(*this);
    }

    template <typename Basis> inline auto _VectorFP<Basis>::GetNormalized() const -> VectorType
    {
        VectorType result;
        f32 revLength = ApproxMath::RSqrt<ApproxMath::Precision::High>(LengthSquare());
        for (uiw index = 0; index < dim; ++index)
            result[index] = operator[](index) * revLength;
		_ValidateValues(*this, result);
        return result;
    }

    template<typename Basis> inline bool _VectorFP<Basis>::IsNormalized(f32 epsilon) const
    {
        f32 length = LengthSquare();
        return (length >= 1.0f - epsilon) && (length <= 1.0f + epsilon);
    }

    template <typename Basis> inline bool _VectorFP<Basis>::EqualsWithEpsilon(const _VectorFP &other, f32 epsilon) const
    {
		_ValidateValues(*this, other);
        for (uiw index = 0; index < dim; ++index)
            if (abs(operator[](index) - other[index]) > epsilon)
                return false;
        return true;
    }

    template <typename Basis> inline void _VectorFP<Basis>::Lerp(const _VectorFP &other, f32 interpolant)
    {
		_ValidateValues(*this, other, interpolant);
		ASSUME(interpolant >= 0.0f && interpolant <= 1.0f);
        *this += interpolant * (other - *this);
    }

    template <typename Basis> inline auto _VectorFP<Basis>::GetLerped(const _VectorFP &other, f32 interpolant) const -> VectorType
    {
		_ValidateValues(*this, other, interpolant);
		ASSUME(interpolant >= 0.0f && interpolant <= 1.0f);
        return *this + interpolant * (other - *this);
    }

    /////////////
    // _Matrix //
    /////////////

    template<uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator+(const _Matrix &other) const -> MatrixType
    {
        MatrixType result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                result[rowIndex][columnIndex] = elements[rowIndex][columnIndex] + other[rowIndex][columnIndex];
        return result;
    }

    template<uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator+=(const _Matrix &other) -> MatrixType &
    {
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                elements[rowIndex][columnIndex] += other[rowIndex][columnIndex];
        return *(MatrixType *)this;
    }

    template<uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator-(const _Matrix &other) const -> MatrixType
    {
        MatrixType result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                result[rowIndex][columnIndex] = elements[rowIndex][columnIndex] - other[rowIndex][columnIndex];
        return result;
    }

    template<uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator-=(const _Matrix &other) -> MatrixType &
    {
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                elements[rowIndex][columnIndex] -= other[rowIndex][columnIndex];
        return *(MatrixType *)this;
    }

    template<uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator*(const _Matrix &other) const -> MatrixType
    {
        MatrixType result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
        {
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
            {
                f32 value = 0.0f;

                for (uiw offset = 0; offset < std::max<uiw>(Rows, Columns); ++offset)
                {
                    value += FetchValueBoundless(rowIndex, offset) * other.FetchValueBoundless(offset, columnIndex);
                }

                result.StoreValueBoundless(rowIndex, columnIndex, value);
            }
        }
        return result;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator*(f32 scalar) const -> MatrixType
    {
        MatrixType result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                result[rowIndex][columnIndex] = elements[rowIndex][columnIndex] * scalar;
        return result;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator*=(f32 scalar) -> MatrixType &
    {
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                elements[rowIndex][columnIndex] *= scalar;
        return *(MatrixType *)this;
    }

    template<uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator*=(const _Matrix &other) -> MatrixType &
    {
        *this = *this * other;
        return *(MatrixType *)this;
    }

    template<uiw Rows, uiw Columns> inline std::array<f32, Columns> &_Matrix<Rows, Columns>::operator[](uiw index)
    {
        return elements[index];
    }

    template<uiw Rows, uiw Columns> inline const std::array<f32, Columns> &_Matrix<Rows, Columns>::operator[](uiw index) const
    {
        return elements[index];
    }

    template<uiw Rows, uiw Columns> inline f32 *_Matrix<Rows, Columns>::Data()
    {
        return elements.data()->data();
    }

    template<uiw Rows, uiw Columns> inline const f32 *_Matrix<Rows, Columns>::Data() const
    { 
        return elements.data()->data();
    }

    template<uiw Rows, uiw Columns> inline MatrixTypeByDimensions<Columns, Rows> _Matrix<Rows, Columns>::GetTransposed() const
    {
        MatrixTypeByDimensions<Columns, Rows> result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                result[columnIndex][rowIndex] = elements[rowIndex][columnIndex];
        return result;
    }

    template<uiw Rows, uiw Columns> inline VectorTypeByDimension<f32, Columns> _Matrix<Rows, Columns>::GetRow(uiw rowIndex) const
    {
        VectorTypeByDimension<f32, Columns> result;
        for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
            result[columnIndex] = elements[rowIndex][columnIndex];
        return result;
    }

    template<uiw Rows, uiw Columns> inline void _Matrix<Rows, Columns>::SetRow(uiw rowIndex, const VectorTypeByDimension<f32, Columns> &row)
    {
        for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
            elements[rowIndex][columnIndex] = row[columnIndex];
    }

    template<uiw Rows, uiw Columns> inline VectorTypeByDimension<f32, Rows> _Matrix<Rows, Columns>::GetColumn(uiw columnIndex) const
    {
        VectorTypeByDimension<f32, Rows> result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            result[rowIndex] = elements[rowIndex][columnIndex];
        return result;
    }

    template<uiw Rows, uiw Columns> inline void _Matrix<Rows, Columns>::SetColumn(uiw columnIndex, const VectorTypeByDimension<f32, Rows> &column)
    {
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            elements[rowIndex][columnIndex] = column[rowIndex];
    }

    template<uiw Rows, uiw Columns> inline f32 _Matrix<Rows, Columns>::FetchValueBoundless(uiw rowIndex, uiw columnIndex) const
    {
        if (rowIndex < Rows && columnIndex < Columns)
            return elements[rowIndex][columnIndex];
        if (rowIndex == columnIndex)
            return 1.0f;
        return 0.0f;
    }

    template<uiw Rows, uiw Columns> inline void _Matrix<Rows, Columns>::StoreValueBoundless(uiw rowIndex, uiw columnIndex, f32 value)
    {
        if (rowIndex >= Rows || columnIndex >= Columns)
            return;
        elements[rowIndex][columnIndex] = value;
    }

    template<uiw Rows, uiw Columns> inline bool _Matrix<Rows, Columns>::EqualsWithEpsilon(const _Matrix &other, f32 epsilon) const
    {
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                if (abs(elements[rowIndex][columnIndex] - other[rowIndex][columnIndex]) > epsilon)
                    return false;
        return true;
    }

	template<uiw Rows, uiw Columns> inline void TransposeSquareMatrix(_Matrix<Rows, Columns> &matrix)
	{
		static_assert(Rows == Columns, "Only a matrix with equal number of rows and columns can be transposed, use GetTransposed() instead");
		for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
			for (uiw columnIndex = rowIndex + 1; columnIndex < Columns; ++columnIndex)
				std::swap(matrix.elements[rowIndex][columnIndex], matrix.elements[columnIndex][rowIndex]);
	}

    template <typename MatrixType, bool isAllowTranslation> inline MatrixType CreateRTS(const optional<Vector3> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale = nullopt)
    {
        static_assert(MatrixType::rows >= (isAllowTranslation ? 4 : 3));
        static_assert(MatrixType::columns >= 3);

        MatrixType result;

        if (rotation)
        {
            f32 cx = cos(rotation->x);
            f32 cy = cos(rotation->y);
            f32 cz = cos(rotation->z);

            f32 sx = sin(rotation->x);
            f32 sy = sin(rotation->y);
            f32 sz = sin(rotation->z);

            result[0][0] = cy * cz;
            result[0][1] = cy * sz;
            result[0][2] = -sy;

            result[1][0] = sx * sy * cz - cx * sz;
            result[1][1] = sx * sy * sz + cx * cz;
            result[1][2] = sx * cy;

            result[2][0] = cx * sy * cz + sx * sz;
            result[2][1] = cx * sy * sz - sx * cz;
            result[2][2] = cx * cy;
        }

        if (scale)
        {
            for (uiw rowIndex = 0; rowIndex < 3; ++rowIndex)
            {
                for (uiw columnIndex = 0; columnIndex < 3; ++columnIndex)
                {
                    result[rowIndex][columnIndex] *= scale->operator[](rowIndex);
                }
            }
        }

        if (isAllowTranslation && translation)
        {
            result[3][0] = translation->x;
            result[3][1] = translation->y;
            result[3][2] = translation->z;
        }

        return result;
    }

    template <typename MatrixType, bool isAllowTranslation> inline MatrixType CreateRTS(const optional<Quaternion> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale = nullopt)
    {
        static_assert(MatrixType::rows >= (isAllowTranslation ? 4 : 3));
        static_assert(MatrixType::columns >= 3);

        MatrixType result;

        if (rotation)
        {
            Matrix3x3 rotationMatrix = rotation->ToMatrix();
            for (uiw rowIndex = 0; rowIndex < 3; ++rowIndex)
            {
                for (uiw columnIndex = 0; columnIndex < 3; ++columnIndex)
                {
                    result[rowIndex][columnIndex] = rotationMatrix[rowIndex][columnIndex];
                }
            }
        }

        if (scale)
        {
            for (uiw rowIndex = 0; rowIndex < 3; ++rowIndex)
            {
                for (uiw columnIndex = 0; columnIndex < 3; ++columnIndex)
                {
                    result[rowIndex][columnIndex] *= scale->operator[](rowIndex);
                }
            }
        }

        if (isAllowTranslation && translation)
        {
            result[3][0] = translation->x;
            result[3][1] = translation->y;
            result[3][2] = translation->z;
        }

        return result;
    }

    template <typename MatrixType> inline MatrixType CreateRotationAroundAxis(const Vector3 &axis, f32 angle)
    {
        MatrixType result;
        angle = -angle;

        f32 ca = cos(angle);
        f32 sa = sin(angle);

        result[0][0] = axis.x * axis.x + (1 - axis.x * axis.x) * ca;
        result[0][1] = axis.x * axis.y * (1 - ca) - axis.z * sa;
        result[0][2] = axis.x * axis.z * (1 - ca) + axis.y * sa;

        result[1][0] = axis.x * axis.y * (1 - ca) + axis.z * sa;
        result[1][1] = axis.y * axis.y + (1 - axis.y * axis.y) * ca;
        result[1][2] = axis.y * axis.z * (1 - ca) - axis.x * sa;

        result[2][0] = axis.x * axis.z * (1 - ca) - axis.y * sa;
        result[2][1] = axis.y * axis.z * (1 - ca) + axis.x * sa;
        result[2][2] = axis.z * axis.z + (1 - axis.z * axis.z) * ca;

        return result;
    }

    ///////////////
    // Rectangle //
    ///////////////

    template <typename T, bool isTopLessThanBottom> inline bool Rectangle<T, isTopLessThanBottom>::IsIntersected(const Rectangle &other) const
    {
        ASSUME(left <= right);
        ASSUME(isTopLessThanBottom ? (top <= bottom) : (bottom <= top));

        if (right < other.left || left > other.right)
            return false;

        if (isTopLessThanBottom)
            return bottom >= other.top && top <= other.bottom;

        return top >= other.bottom && bottom <= other.top;
    }

    template <typename T, bool isTopLessThanBottom> inline T Rectangle<T, isTopLessThanBottom>::Distance(T x, T y) const
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            return left != x || right != x || top != y || bottom != y;
        }
        else
        {
            T centreX = (left + right);
            T centreY = (top + bottom);
            T halfWidth = Width();
            T halfHeight = Height();

            if constexpr (std::is_floating_point_v<T>)
            {
                centreX *= T(0.5);
                centreY *= T(0.5);
                halfWidth *= T(0.5);
                halfHeight *= T(0.5);
            }
            else
            {
                centreX /= 2;
                centreY /= 2;
                halfWidth /= 2;
                halfHeight /= 2;
            }

            T dx = std::max(std::abs(x - centreX) - halfWidth, T(0));
            T dy = std::max(std::abs(y - centreY) - halfHeight, T(0));

            return dx * dx + dy * dy;
        }
    }

    template <typename T, bool isTopLessThanBottom> inline auto Rectangle<T, isTopLessThanBottom>::Expand(const Rectangle &other) -> Rectangle &
    {
        *this = GetExpanded(other);
        return *this;
    }

    template <typename T, bool isTopLessThanBottom> inline auto Rectangle<T, isTopLessThanBottom>::Expand(T x, T y) -> Rectangle &
    {
        *this = GetExpanded(x, y);
        return *this;
    }

    template <typename T, bool isTopLessThanBottom> inline auto Rectangle<T, isTopLessThanBottom>::GetExpanded(const Rectangle &other) const -> Rectangle
    {
        if (IsDefined())
        {
            if (other.IsDefined())
            {
                T l = std::min(left, other.left);
                T r = std::max(right, other.right);
                T t, b;

                if constexpr(isTopLessThanBottom)
                {
                    t = std::min(top, other.top);
                    b = std::max(bottom, other.bottom);
                }
                else
                {
                    t = std::max(top, other.top);
                    b = std::min(bottom, other.bottom);
                }
            }
            return *this;
        }
        else
        {
            if (other.IsDefined())
            {
                return other;
            }
            return {};
        }
    }

    template <typename T, bool isTopLessThanBottom> inline auto Rectangle<T, isTopLessThanBottom>::GetExpanded(T x, T y) const -> Rectangle
    {
        if (IsDefined())
        {
            T l = std::min(left, x);
            T r = std::max(right, x);
            T t, b;

            if constexpr(isTopLessThanBottom)
            {
                t = std::min(top, y);
                b = std::max(bottom, y);
            }
            else
            {
                t = std::max(top, y);
                b = std::min(bottom, y);
            }

            return {l, r, t, b};
        }
        return Rectangle::FromPoint(x, y);
    }

    template <typename T, bool isTopLessThanBottom> inline bool Rectangle<T, isTopLessThanBottom>::IsDefined() const
    {
        ASSUME(left <= right);
        ASSUME(isTopLessThanBottom ? (top <= bottom) : (bottom <= top));

        // suffice to check just one side, right side definitely mustn't be T::min in a defined rectangle
        return right != std::numeric_limits<T>::min();
    }

    template <typename T, bool isTopLessThanBottom> inline T Rectangle<T, isTopLessThanBottom>::Width() const
    {
        ASSUME(left <= right);
        ASSUME(isTopLessThanBottom ? (top <= bottom) : (bottom <= top));

        return right - left;
    }

    template <typename T, bool isTopLessThanBottom> inline T Rectangle<T, isTopLessThanBottom>::Height() const
    {
        ASSUME(left <= right);
        ASSUME(isTopLessThanBottom ? (top <= bottom) : (bottom <= top));

        return isTopLessThanBottom ? (bottom - top) : (top - bottom);
    }

    template <typename T, bool isTopLessThanBottom> inline bool Rectangle<T, isTopLessThanBottom>::operator == (const Rectangle &other) const
    {
        return left == other.left && right == other.right && top == other.top && bottom == other.bottom;
    }

    template <typename T, bool isTopLessThanBottom> inline bool Rectangle<T, isTopLessThanBottom>::operator != (const Rectangle &other) const
    {
        return !(this->operator==(other));
    }
}