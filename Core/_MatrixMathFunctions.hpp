#pragma once

#include "MatrixMathTypes.hpp"
#include "ApproxMath.hpp"

namespace StdLib
{
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
        _ValidateValues(*this, scalar);
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

	template <typename _ScalarType, uiw Dim>	inline bool _VectorBase<_ScalarType, Dim>::operator == (const _VectorBase &other) const
	{
		_ValidateValues(*this, other);
		for (uiw index = 0; index < Dim; ++index)
			if (Data()[index] != other.Data()[index])
				return false;
		return true;
	}

	template <typename _ScalarType, uiw Dim>	inline bool _VectorBase<_ScalarType, Dim>::operator != (const _VectorBase &other) const
	{
		return !operator==(other);
	}

    template <typename ScalarType, uiw Dim> inline std::array<ScalarType, Dim> &_VectorBase<ScalarType, Dim>::Data()
    {
		_ValidateValues(*this);
        return *(std::array<ScalarType, Dim> *)&x;
    }

    template <typename ScalarType, uiw Dim> inline const std::array<ScalarType, Dim> &_VectorBase<ScalarType, Dim>::Data() const
    {
		_ValidateValues(*this);
        return *(std::array<ScalarType, Dim> *)&x;
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

    template <typename _ScalarType, uiw Dim> inline auto _VectorBase<_ScalarType, Dim>::ForEach(ScalarType func(ScalarType element)) -> VectorType &
    {
        for (auto &el : Data())
            el = func(el);
        _ValidateValues(*this);
        return *(VectorType *)this;
    }

    /////////////////
    // Vector2Base //
    /////////////////

    template <typename ScalarType> inline ScalarType &Vector2Base<ScalarType>::operator[](uiw index)
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        ASSUME(index == 1);
        return y;
    }

    template <typename ScalarType> inline const ScalarType &Vector2Base<ScalarType>::operator[](uiw index) const
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        ASSUME(index == 1);
        return y;
    }

    /////////////////
    // Vector3Base //
    /////////////////

    template <typename ScalarType> inline ScalarType &Vector3Base<ScalarType>::operator[](uiw index)
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        if (index == 1) return y;
        ASSUME(index == 2);
        return z;
    }

    template <typename ScalarType> inline const ScalarType &Vector3Base<ScalarType>::operator[](uiw index) const
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        if (index == 1) return y;
        ASSUME(index == 2);
        return z;
    }

    template <typename ScalarType> inline VectorTypeByDimension<ScalarType, 2> Vector3Base<ScalarType>::ToVector2() const
    {
		_ValidateValues(*this);
        return {x, y};
    }

    /////////////////
    // Vector4Base //
    /////////////////

    template <typename ScalarType> inline ScalarType &Vector4Base<ScalarType>::operator[](uiw index)
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;
        ASSUME(index == 3);
        return w;
    }

    template <typename ScalarType> inline const ScalarType &Vector4Base<ScalarType>::operator[](uiw index) const
    {
		_ValidateValues(*this);
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;
        ASSUME(index == 3);
        return w;
    }

    template <typename ScalarType> inline VectorTypeByDimension<ScalarType, 2> Vector4Base<ScalarType>::ToVector2() const
    {
		_ValidateValues(*this);
        return {x, y};
    }

    template <typename ScalarType> inline VectorTypeByDimension<ScalarType, 3> Vector4Base<ScalarType>::ToVector3() const
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

    template <typename Basis> inline auto _VectorFP<Basis>::Normalize() -> VectorType &
    {
        f32 revLength = ApproxMath::RSqrt<ApproxMath::Precision::High>(LengthSquare());
        for (uiw index = 0; index < dim; ++index)
            operator[](index) *= revLength;
		_ValidateValues(*this);
        return *(VectorType *)this;
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

    template <typename Basis> inline bool _VectorFP<Basis>::IsNormalized(f32 epsilon) const
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

    template <typename Basis> inline auto _VectorFP<Basis>::Lerp(const _VectorFP &other, f32 interpolant) -> VectorType &
    {
		_ValidateValues(*this, other, interpolant);
		ASSUME(interpolant >= 0.0f && interpolant <= 1.0f);
        *this += (other - *this) * interpolant;
        return *(VectorType *)this;
    }

    template <typename Basis> inline auto _VectorFP<Basis>::GetLerped(const _VectorFP &other, f32 interpolant) const -> VectorType
    {
		_ValidateValues(*this, other, interpolant);
		ASSUME(interpolant >= 0.0f && interpolant <= 1.0f);
        return *this + (other - *this) * interpolant;
    }

    /////////////
    // _Matrix //
    /////////////

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator + (const _Matrix &other) const -> MatrixType
    {
        MatrixType result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                result[rowIndex][columnIndex] = elements[rowIndex][columnIndex] + other[rowIndex][columnIndex];
        _ValidateValues(*this, other, result);
        return result;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator += (const _Matrix &other) -> MatrixType &
    {
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                elements[rowIndex][columnIndex] += other[rowIndex][columnIndex];
        _ValidateValues(*this, other);
        return *(MatrixType *)this;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator - (const _Matrix &other) const -> MatrixType
    {
        MatrixType result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                result[rowIndex][columnIndex] = elements[rowIndex][columnIndex] - other[rowIndex][columnIndex];
        _ValidateValues(*this, other);
        return result;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator -= (const _Matrix &other) -> MatrixType &
    {
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                elements[rowIndex][columnIndex] -= other[rowIndex][columnIndex];
        _ValidateValues(*this, other);
        return *(MatrixType *)this;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator * (const _Matrix &other) const -> MatrixType
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
        _ValidateValues(*this, other, result);
        return result;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator * (f32 scalar) const -> MatrixType
    {
        MatrixType result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                result[rowIndex][columnIndex] = elements[rowIndex][columnIndex] * scalar;
        _ValidateValues(*this, scalar);
        return result;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator *= (f32 scalar) -> MatrixType &
    {
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                elements[rowIndex][columnIndex] *= scalar;
        _ValidateValues(*this, scalar);
        return *(MatrixType *)this;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator *= (const _Matrix &other) -> MatrixType &
    {
        *this = *this * other;
        _ValidateValues(*this, other);
        return *(MatrixType *)this;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator[](uiw index) -> std::array<f32, Columns> &
    {
        _ValidateValues(*this);
        return elements[index];
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator[](uiw index) const -> const std::array<f32, Columns> &
    {
        _ValidateValues(*this);
        return elements[index];
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::Data() -> std::array<f32, Rows * Columns> &
    {
        _ValidateValues(*this);
        return *(std::array<f32, Rows * Columns> *)elements.data()->data();
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::Data() const -> const std::array<f32, Rows * Columns> &
    {
        _ValidateValues(*this);
        return *(std::array<f32, Rows * Columns> *)elements.data()->data();
    }

    template <uiw Rows, uiw Columns> inline MatrixTypeByDimensions<Columns, Rows> _Matrix<Rows, Columns>::GetTransposed() const
    {
        MatrixTypeByDimensions<Columns, Rows> result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                result[columnIndex][rowIndex] = elements[rowIndex][columnIndex];
        _ValidateValues(result);
        return result;
    }

    template <uiw Rows, uiw Columns> inline VectorTypeByDimension<f32, Columns> _Matrix<Rows, Columns>::GetRow(uiw rowIndex) const
    {
        _ValidateValues(*this);
        VectorTypeByDimension<f32, Columns> result;
        for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
            result[columnIndex] = elements[rowIndex][columnIndex];
        return result;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::SetRow(uiw rowIndex, const VectorTypeByDimension<f32, Columns> &row) -> MatrixType &
    {
        for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
            elements[rowIndex][columnIndex] = row[columnIndex];
        _ValidateValues(*this);
        return *(MatrixType *)this;
    }

    template <uiw Rows, uiw Columns> inline VectorTypeByDimension<f32, Rows> _Matrix<Rows, Columns>::GetColumn(uiw columnIndex) const
    {
        _ValidateValues(*this);
        VectorTypeByDimension<f32, Rows> result;
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            result[rowIndex] = elements[rowIndex][columnIndex];
        return result;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::SetColumn(uiw columnIndex, const VectorTypeByDimension<f32, Rows> &column) -> MatrixType &
    {
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            elements[rowIndex][columnIndex] = column[rowIndex];
        _ValidateValues(*this);
        return *(MatrixType *)this;
    }

    template <uiw Rows, uiw Columns> inline f32 _Matrix<Rows, Columns>::FetchValueBoundless(uiw rowIndex, uiw columnIndex) const
    {
        _ValidateValues(*this);
        if (rowIndex < Rows && columnIndex < Columns)
            return elements[rowIndex][columnIndex];
        if (rowIndex == columnIndex)
            return 1.0f;
        return 0.0f;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::StoreValueBoundless(uiw rowIndex, uiw columnIndex, f32 value) -> MatrixType &
    {
        _ValidateValues(*this, value);
        if (rowIndex >= Rows || columnIndex >= Columns)
            return *(MatrixType *)this;
        elements[rowIndex][columnIndex] = value;
        return *(MatrixType *)this;
    }

    template <uiw Rows, uiw Columns> inline bool _Matrix<Rows, Columns>::EqualsWithEpsilon(const _Matrix &other, f32 epsilon) const
    {
        _ValidateValues(*this, other, epsilon);
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                if (abs(elements[rowIndex][columnIndex] - other[rowIndex][columnIndex]) > epsilon)
                    return false;
        return true;
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