#pragma once

#include "MatrixMathTypes.hpp"
#include "ApproxMath.hpp"

namespace StdLib
{
	/////////////////
	// _VectorBase //
	/////////////////

	#define VECDATA(v, index) ((&(v).x)[index])

	template <typename _ScalarType, uiw Dim> inline bool _VectorBase<_ScalarType, Dim>::operator == (const _VectorBase &other) const
	{
		_ValidateValues(*this, other);
		for (uiw index = 0; index < Dim; ++index)
			if (VECDATA(*this, index) != VECDATA(other, index))
				return false;
		return true;
	}

	template <typename _ScalarType, uiw Dim> inline bool _VectorBase<_ScalarType, Dim>::operator != (const _VectorBase &other) const
	{
		return !operator==(other);
	}

	template <typename ScalarType, uiw Dim> inline std::array<ScalarType, Dim> &_VectorBase<ScalarType, Dim>::Data()
	{
		_ValidateValues(*this);
		return *reinterpret_cast<std::array<ScalarType, Dim> *>(&x);
	}

	template <typename ScalarType, uiw Dim> inline const std::array<ScalarType, Dim> &_VectorBase<ScalarType, Dim>::Data() const
	{
		_ValidateValues(*this);
		return *reinterpret_cast<const std::array<ScalarType, Dim> *>(&x);
	}

	template <typename ScalarType, uiw Dim> inline ScalarType &_VectorBase<ScalarType, Dim>::operator[](uiw index)
	{
		_ValidateValues(*this);
		ASSUME(index < Dim);
		return VECDATA(*this, index);
	}

	template <typename ScalarType, uiw Dim> inline const ScalarType &_VectorBase<ScalarType, Dim>::operator[](uiw index) const
	{
		_ValidateValues(*this);
		ASSUME(index < Dim);
		return VECDATA(*this, index);
	}

	template <typename ScalarType, uiw Dim> inline uiw _VectorBase<ScalarType, Dim>::Hash() const
	{
		_ValidateValues(*this);
		uiw hash = 0x811c9dc5u;
		for (uiw index = 0; index < Dim; ++index)
		{
			hash ^= std::hash<ScalarType>()(VECDATA(*this, index));
			if (index + 1 < Dim)
				hash *= 16777619u;
		}
		return hash;
	}

    ///////////////////////////
    // _VectorArithmeticBase //
    ///////////////////////////

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator + (const _VectorArithmeticBase &other) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(result, index) = VECDATA(*this, index) + VECDATA(other, index);
		_ValidateValues(*this, other, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator + (ScalarType scalar) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(result, index) = VECDATA(*this, index) + scalar;
		_ValidateValues(*this, scalar, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator += (const _VectorArithmeticBase &other) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(*this, index) += VECDATA(other, index);
		_ValidateValues(*this, other);
		return *static_cast<VectorType *>(this);
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator += (ScalarType scalar) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(*this, index) += scalar;
        _ValidateValues(*this, scalar);
		return *static_cast<VectorType *>(this);
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator - (const _VectorArithmeticBase &other) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(result, index) = VECDATA(*this, index) - VECDATA(other, index);
		_ValidateValues(*this, other, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator - (ScalarType scalar) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(result, index) = VECDATA(*this, index) - scalar;
		_ValidateValues(*this, scalar, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator -= (const _VectorArithmeticBase &other) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(*this, index) -= VECDATA(other, index);
		_ValidateValues(*this, other);
		return *static_cast<VectorType *>(this);
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator -= (ScalarType scalar) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(*this, index) -= scalar;
		_ValidateValues(*this, scalar);
		return *static_cast<VectorType *>(this);
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator * (const _VectorArithmeticBase &other) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(result, index) = VECDATA(*this, index) * VECDATA(other, index);
		_ValidateValues(*this, other, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator * (ScalarType scalar) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(result, index) = VECDATA(*this, index) * scalar;
		_ValidateValues(*this, scalar, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator *= (const _VectorArithmeticBase &other) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(*this, index) *= VECDATA(other, index);
		_ValidateValues(*this, other);
		return *static_cast<VectorType *>(this);
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator *= (ScalarType scalar) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(*this, index) *= scalar;
		_ValidateValues(*this, scalar);
		return *static_cast<VectorType *>(this);
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator / (const _VectorArithmeticBase &other) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(result, index) = VECDATA(*this, index) / VECDATA(other, index);
		_ValidateValues(*this, other, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator / (ScalarType scalar) const -> VectorType
    {
        VectorType result;
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(result, index) = VECDATA(*this, index) / scalar;
		_ValidateValues(*this, scalar, result);
        return result;
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator /= (const _VectorArithmeticBase &other) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(*this, index) /= VECDATA(other, index);
		_ValidateValues(*this, other);
		return *static_cast<VectorType *>(this);
    }

    template <typename ScalarType, uiw Dim> inline auto _VectorArithmeticBase<ScalarType, Dim>::operator /= (ScalarType scalar) -> VectorType &
    {
        for (uiw index = 0; index < Dim; ++index)
            VECDATA(*this, index) /= scalar;
		_ValidateValues(*this, scalar);
        return *static_cast<VectorType *>(this);
    }

    template <typename _ScalarType, uiw Dim> inline auto _VectorArithmeticBase<_ScalarType, Dim>::operator - () const -> VectorType
    {
        VectorType result;
        if constexpr (std::is_signed_v<_ScalarType>)
        {
            for (uiw index = 0; index < Dim; ++index)
                VECDATA(result, index) = -VECDATA(*this, index);
        }
        else
        {
            HARDBREAK; // trying to negate an unsigned type
            result = *static_cast<const VectorType *>(this);
        }
        _ValidateValues(*this, result);
        return result;
    }

    /////////////////
    // Vector2Base //
    /////////////////

	template<typename BaseType> inline VectorTypeByDimension<typename BaseType::ScalarType, 2> Vector2Base<BaseType>::ToVector2() const
	{
		_ValidateValues(*this);
		return {x, y};
	}

    /////////////////
    // Vector3Base //
    /////////////////

    template <typename BaseType> inline VectorTypeByDimension<typename BaseType::ScalarType, 2> Vector3Base<BaseType>::ToVector2() const
    {
		_ValidateValues(*this);
        return {x, y};
    }

	template<typename BaseType> inline VectorTypeByDimension<typename BaseType::ScalarType, 3> Vector3Base<BaseType>::ToVector3() const
	{
		_ValidateValues(*this);
		return {x, y, this->z};
	}

    /////////////////
    // Vector4Base //
    /////////////////

    template <typename BaseType> inline VectorTypeByDimension<typename BaseType::ScalarType, 2> Vector4Base<BaseType>::ToVector2() const
    {
		_ValidateValues(*this);
        return {x, y};
    }

    template <typename BaseType> inline VectorTypeByDimension<typename BaseType::ScalarType, 3> Vector4Base<BaseType>::ToVector3() const
    {
		_ValidateValues(*this);
        return {x, y, this->z};
    }

	template<typename BaseType> inline VectorTypeByDimension<typename BaseType::ScalarType, 4> Vector4Base<BaseType>::ToVector4() const
	{
		_ValidateValues(*this);
		return {x, y, this->z, this->w};
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
            sum += VECDATA(*this, index) * VECDATA(*this, index);
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
            dist = VECDATA(*this, index) - VECDATA(other, index);
            sum += dist * dist;
        }
		_ValidateValues(*this, sum);
        return sum;
    }

    template <typename Basis> inline f32 _VectorFP<Basis>::Dot(const _VectorFP &other) const
    {
        f32 sum = x * other.x;
        for (uiw index = 1; index < dim; ++index)
            sum += VECDATA(*this, index) * VECDATA(other, index);
		_ValidateValues(*this, sum);
        return sum;
    }

    template <typename Basis> inline auto _VectorFP<Basis>::Normalize() -> VectorType &
    {
        f32 revLength = ApproxMath::RSqrt<ApproxMath::Precision::High>(LengthSquare());
        for (uiw index = 0; index < dim; ++index)
            VECDATA(*this, index) *= revLength;
		_ValidateValues(*this);
        return *static_cast<VectorType *>(this);
    }

    template <typename Basis> inline auto _VectorFP<Basis>::GetNormalized() const -> VectorType
    {
        VectorType result;
        f32 revLength = ApproxMath::RSqrt<ApproxMath::Precision::High>(LengthSquare());
        for (uiw index = 0; index < dim; ++index)
            VECDATA(result, index) = VECDATA(*this, index) * revLength;
		_ValidateValues(*this, result);
        return result;
    }

	template<typename Basis> inline bool _VectorFP<Basis>::IsNormalized() const
	{
		return StdLib::EqualsWithEpsilon(LengthSquare(), 1.0f);
	}

    template <typename Basis> inline bool _VectorFP<Basis>::EqualsWithEpsilon(const _VectorFP &other, f32 epsilon) const
    {
		ASSUME(epsilon >= 0);
		_ValidateValues(*this, other);
        for (uiw index = 0; index < dim; ++index)
            if (abs(VECDATA(*this, index) - VECDATA(other, index)) > epsilon)
                return false;
        return true;
    }

    template <typename Basis> inline auto _VectorFP<Basis>::Lerp(const _VectorFP &other, f32 interpolant) -> VectorType &
    {
		_ValidateValues(*this, other, interpolant);
		ASSUME(interpolant >= 0.0f && interpolant <= 1.0f);
        *this += (other - *this) * interpolant;
        return *static_cast<VectorType *>(this);
    }

    template <typename Basis> inline auto _VectorFP<Basis>::GetLerped(const _VectorFP &other, f32 interpolant) const -> VectorType
    {
		_ValidateValues(*this, other, interpolant);
		ASSUME(interpolant >= 0.0f && interpolant <= 1.0f);
        return *this + (other - *this) * interpolant;
    }

	#undef VECDATA

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
        return *static_cast<MatrixType *>(this);
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
        return *static_cast<MatrixType *>(this);
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
        return *static_cast<MatrixType *>(this);
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::operator *= (const _Matrix &other) -> MatrixType &
    {
        *this = *this * other;
        _ValidateValues(*this, other);
        return *static_cast<MatrixType *>(this);
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
        return *reinterpret_cast<std::array<f32, Rows * Columns> *>(elements.data()->data());
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::Data() const -> const std::array<f32, Rows * Columns> &
    {
        _ValidateValues(*this);
        return *reinterpret_cast<const std::array<f32, Rows * Columns> *>(elements.data()->data());
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
        return *static_cast<MatrixType *>(this);
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
        return *static_cast<MatrixType *>(this);
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
            return *static_cast<MatrixType *>(this);
        elements[rowIndex][columnIndex] = value;
        return *static_cast<MatrixType *>(this);
    }

    template <uiw Rows, uiw Columns> inline bool _Matrix<Rows, Columns>::EqualsWithEpsilon(const _Matrix &other, f32 epsilon) const
    {
		ASSUME(epsilon >= 0);
        _ValidateValues(*this, other, epsilon);
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
                if (abs(elements[rowIndex][columnIndex] - other[rowIndex][columnIndex]) > epsilon)
                    return false;
        return true;
    }

    template <uiw Rows, uiw Columns> inline auto _Matrix<Rows, Columns>::Inverse() -> MatrixType &
    {
        auto inversed = static_cast<MatrixType *>(this)->GetInversed();
        if (inversed)
            *static_cast<MatrixType *>(this) = *inversed;
        else
            SOFTBREAK;
        return *static_cast<MatrixType *>(this);
    }

	template <uiw Rows, uiw Columns> inline uiw _Matrix<Rows, Columns>::Hash() const
	{
		_ValidateValues(*this);
		size_t hash = 0x811c9dc5u;
		for (size_t row = 0; row < Rows; ++row)
			for (size_t column = 0; column < Columns; ++column)
			{
				hash ^= std::hash<f32>()(elements[row][column]);
				hash *= 16777619u;
			}
		return hash;
	}

    ///////////////
    // Rectangle //
    ///////////////

	// TODO: validating values

    template <typename T, bool IsTopLessThanBottom> inline bool Rectangle<T, IsTopLessThanBottom>::IsIntersected(const Rectangle &other) const
    {
        ASSUME(left <= right);
        ASSUME(IsTopLessThanBottom ? (top <= bottom) : (bottom <= top));

        if (right < other.left || left > other.right)
            return false;

        if constexpr (IsTopLessThanBottom)
            return bottom >= other.top && top <= other.bottom;
		else
			return top >= other.bottom && bottom <= other.top;
    }

	template <typename T, bool IsTopLessThanBottom> inline bool Rectangle<T, IsTopLessThanBottom>::Contains(T x, T y) const
	{
		ASSUME(left <= right);
		ASSUME(IsTopLessThanBottom ? (top <= bottom) : (bottom <= top));

		if (x > right || x < left)
			return false;

		if constexpr (IsTopLessThanBottom)
			return y <= bottom && y >= top;
		else
			return y <= top && y >= bottom;
	}

  //  template <typename T, bool IsTopLessThanBottom> inline T Rectangle<T, IsTopLessThanBottom>::SquareDistance(T x, T y) const
  //  {
		//ASSUME(left <= right);
		//ASSUME(IsTopLessThanBottom ? (top <= bottom) : (bottom <= top));

  //      T centreX = (left + right);
  //      T centreY = (top + bottom);
  //      T halfWidth = Width();
  //      T halfHeight = Height();

  //      if constexpr (std::is_floating_point_v<T>)
  //      {
  //          centreX *= T(0.5);
  //          centreY *= T(0.5);
  //          halfWidth *= T(0.5);
  //          halfHeight *= T(0.5);
  //      }
  //      else
  //      {
  //          centreX /= 2;
  //          centreY /= 2;
  //          halfWidth /= 2;
  //          halfHeight /= 2;
  //      }

		//T xDiff, yDiff;

		//if constexpr (std::is_signed_v<T>)
		//{
		//	xDiff = std::abs(x - centreX);
		//	yDiff = std::abs(y - centreY);
		//}
		//else
		//{
		//	xDiff = x > centreX ? x - centreX : centreX - x;
		//	yDiff = y > centreY ? y - centreY : centreY - y;
		//}

  //      T dx = std::max(xDiff - halfWidth, T(0));
  //      T dy = std::max(yDiff - halfHeight, T(0));

  //      return dx * dx + dy * dy;
  //  }

    template <typename T, bool IsTopLessThanBottom> inline auto Rectangle<T, IsTopLessThanBottom>::Expand(const Rectangle &other) -> Rectangle &
    {
        *this = GetExpanded(other);
        return *this;
    }

    template <typename T, bool IsTopLessThanBottom> inline auto Rectangle<T, IsTopLessThanBottom>::Expand(T x, T y) -> Rectangle &
    {
        *this = GetExpanded(x, y);
        return *this;
    }

    template <typename T, bool IsTopLessThanBottom> inline auto Rectangle<T, IsTopLessThanBottom>::GetExpanded(const Rectangle &other) const -> Rectangle
    {
        if (IsDefined())
        {
            if (other.IsDefined())
            {
                T l = std::min(left, other.left);
                T r = std::max(right, other.right);
                T t, b;

                if constexpr(IsTopLessThanBottom)
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

    template <typename T, bool IsTopLessThanBottom> inline auto Rectangle<T, IsTopLessThanBottom>::GetExpanded(T x, T y) const -> Rectangle
    {
        if (IsDefined())
        {
            T l = std::min(left, x);
            T r = std::max(right, x);
            T t, b;

            if constexpr(IsTopLessThanBottom)
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

    template <typename T, bool IsTopLessThanBottom> inline bool Rectangle<T, IsTopLessThanBottom>::IsDefined() const
    {
        ASSUME(left <= right);
        ASSUME(IsTopLessThanBottom ? (top <= bottom) : (bottom <= top));

        // suffice to check just one side, right side definitely mustn't be T::min in a defined rectangle
        return right != undefinedValue;
    }

    template <typename T, bool IsTopLessThanBottom> inline T Rectangle<T, IsTopLessThanBottom>::Width() const
    {
        ASSUME(left <= right);
        ASSUME(IsTopLessThanBottom ? (top <= bottom) : (bottom <= top));

        return right - left;
    }

    template <typename T, bool IsTopLessThanBottom> inline T Rectangle<T, IsTopLessThanBottom>::Height() const
    {
        ASSUME(left <= right);
        ASSUME(IsTopLessThanBottom ? (top <= bottom) : (bottom <= top));

        return IsTopLessThanBottom ? (bottom - top) : (top - bottom);
    }

    template <typename T, bool IsTopLessThanBottom> inline bool Rectangle<T, IsTopLessThanBottom>::operator == (const Rectangle &other) const
    {
        return left == other.left && right == other.right && top == other.top && bottom == other.bottom;
    }

    template <typename T, bool IsTopLessThanBottom> inline bool Rectangle<T, IsTopLessThanBottom>::operator != (const Rectangle &other) const
    {
        return !(this->operator==(other));
    }

	template <typename T, bool IsTopLessThanBottom> inline uiw Rectangle<T, IsTopLessThanBottom>::Hash() const
	{
		uiw hash = 0x811c9dc5u;

		hash ^= std::hash<T>()(left);
		hash *= 16777619u;

		hash ^= std::hash<T>()(right);
		hash *= 16777619u;

		hash ^= std::hash<T>()(top);
		hash *= 16777619u;

		hash ^= std::hash<T>()(bottom);

		return hash;
	}
}