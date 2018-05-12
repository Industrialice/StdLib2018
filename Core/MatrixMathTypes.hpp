#pragma once

#include "CoreHeader.hpp"

// TODO: inverse matrix
// TODO: determinant
// TODO: matrix conversions (3x3 to 4x4, 4x4 to 3x3 etc.)
// TODO: Rectangle3D

namespace StdLib
{
    template <typename ScalarType> struct Vector2Base;
    template <typename ScalarType> struct Vector3Base;
    template <typename ScalarType> struct Vector4Base;

    struct Vector2;
    struct Vector3;
    struct Vector4;

    template <uiw Rows, uiw Columns> struct _Matrix;

    struct Matrix4x3;
    struct Matrix3x4;
    struct Matrix4x4;
    struct Matrix3x2;
    struct Matrix2x3;
    struct Matrix3x3;
    struct Matrix2x2;

    struct Quaternion;

    template <typename ScalarType, uiw Dim> struct _ChooseVectorType;

    template <> struct _ChooseVectorType<f32, 2> { using Type = Vector2; };
    template <> struct _ChooseVectorType<f32, 3> { using Type = Vector3; };
    template <> struct _ChooseVectorType<f32, 4> { using Type = Vector4; };

    template <typename ScalarType> struct _ChooseVectorType<ScalarType, 2> { using Type = Vector2Base<ScalarType>; };
    template <typename ScalarType> struct _ChooseVectorType<ScalarType, 3> { using Type = Vector3Base<ScalarType>; };
    template <typename ScalarType> struct _ChooseVectorType<ScalarType, 4> { using Type = Vector4Base<ScalarType>; };
    
    template <typename ScalarType, uiw Dim> using VectorTypeByDimension = typename _ChooseVectorType<ScalarType, Dim>::Type;

    template <uiw Rows, uiw Columns> struct _ChooseMatrixType;
    template <> struct _ChooseMatrixType<2, 2> { using Type = Matrix2x2; };
    template <> struct _ChooseMatrixType<2, 3> { using Type = Matrix2x3; };
    template <> struct _ChooseMatrixType<3, 2> { using Type = Matrix3x2; };
    template <> struct _ChooseMatrixType<3, 3> { using Type = Matrix3x3; };
    template <> struct _ChooseMatrixType<4, 3> { using Type = Matrix4x3; };
    template <> struct _ChooseMatrixType<3, 4> { using Type = Matrix3x4; };
    template <> struct _ChooseMatrixType<4, 4> { using Type = Matrix4x4; };

    template <uiw Rows, uiw Columns> using MatrixTypeByDimensions = typename _ChooseMatrixType<Rows, Columns>::Type;

    template <typename _ScalarType, uiw Dim> struct _VectorBase
    {
        static constexpr uiw dim = Dim;

        static_assert(dim > 1);

        using ScalarType = _ScalarType;
        using VectorType = VectorTypeByDimension<ScalarType, dim>;

        ScalarType x = 0, y = 0;

        constexpr _VectorBase() = default;
        _VectorBase(ScalarType x, ScalarType y);

        [[nodiscard]] VectorType operator + (const _VectorBase &other) const;
        [[nodiscard]] VectorType operator + (ScalarType scalar) const;
        [[nodiscard]] friend VectorType operator + (ScalarType scalar, const _VectorBase &vector) { return vector + scalar; }

        VectorType &operator += (const _VectorBase &other);
        VectorType &operator += (ScalarType scalar);

        [[nodiscard]] VectorType operator - (const _VectorBase &other) const;
        [[nodiscard]] VectorType operator - (ScalarType scalar) const;

        VectorType &operator -= (const _VectorBase &other);
        VectorType &operator -= (ScalarType scalar);

        [[nodiscard]] VectorType operator * (const _VectorBase &other) const;
        [[nodiscard]] VectorType operator * (ScalarType scalar) const;
        [[nodiscard]]friend VectorType operator * (ScalarType scalar, const _VectorBase &vector) { return vector * scalar; } // TODO: do we need this?

        VectorType &operator *= (const _VectorBase &other);
        VectorType &operator *= (ScalarType scalar);

        [[nodiscard]] VectorType operator / (const _VectorBase &other) const;
        [[nodiscard]] VectorType operator / (ScalarType scalar) const;
        [[nodiscard]] friend VectorType operator / (ScalarType scalar, const _VectorBase &vector) { return vector / scalar; } // TODO: do we need this?

        VectorType &operator /= (const _VectorBase &other);
        VectorType &operator /= (ScalarType scalar);

        [[nodiscard]] ScalarType *Data();
        [[nodiscard]] const ScalarType *Data() const;
        
        [[nodiscard]] bool Equals(const _VectorBase &other) const;

        [[nodiscard]] ScalarType Accumulate() const;
        [[nodiscard]] ScalarType Max() const;
        [[nodiscard]] ScalarType Min() const;
    };

    template <typename ScalarType> struct Vector2Base : _VectorBase<ScalarType, 2>
    {
        using _VectorBase<ScalarType, 2>::x;
        using _VectorBase<ScalarType, 2>::y;

        Vector2Base() = default;
        Vector2Base(ScalarType x, ScalarType y);
        [[nodiscard]] ScalarType &operator [] (uiw index);
        [[nodiscard]] const ScalarType &operator [] (uiw index) const;
    };

    template <typename ScalarType> struct Vector3Base : _VectorBase<ScalarType, 3>
    {
        using _VectorBase<ScalarType, 3>::x;
        using _VectorBase<ScalarType, 3>::y;
        ScalarType z = 0;

        Vector3Base() = default;
        Vector3Base(ScalarType x, ScalarType y, ScalarType z);
        Vector3Base(const VectorTypeByDimension<ScalarType, 2> &vec, ScalarType z);
        [[nodiscard]] ScalarType &operator [] (uiw index);
        [[nodiscard]] const ScalarType &operator [] (uiw index) const;
        [[nodiscard]] VectorTypeByDimension<ScalarType, 2> ToVector2() const;
    };

    template <typename ScalarType> struct Vector4Base : _VectorBase<ScalarType, 4>
    {
        using _VectorBase<ScalarType, 4>::x;
        using _VectorBase<ScalarType, 4>::y;
        ScalarType z = 0;
        ScalarType w = 0;

        Vector4Base() = default;
        Vector4Base(ScalarType x, ScalarType y, ScalarType z, ScalarType w);
        Vector4Base(const VectorTypeByDimension<ScalarType, 2> &vec, ScalarType z, ScalarType w);
        Vector4Base(const VectorTypeByDimension<ScalarType, 3> &vec, ScalarType w);
        Vector4Base(const VectorTypeByDimension<ScalarType, 2> &v0, const VectorTypeByDimension<ScalarType, 2> &v1);
        [[nodiscard]] ScalarType &operator [] (uiw index);
        [[nodiscard]] const ScalarType &operator [] (uiw index) const;
        [[nodiscard]] VectorTypeByDimension<ScalarType, 2> ToVector2() const;
        [[nodiscard]] VectorTypeByDimension<ScalarType, 3> ToVector3() const;
    };

    template <typename Basis> struct _Vector : Basis
    {
        using Basis::Basis;
        using Basis::operator[];
        using typename Basis::VectorType;
        using Basis::dim;
        using Basis::operator+;
        using Basis::operator+=;
        using Basis::operator-;
        using Basis::operator-=;
        using Basis::operator*;
        using Basis::operator*=;
        using Basis::operator/;
        using Basis::operator/=;
        using Basis::Data;
        using Basis::Accumulate;
        using Basis::Max;
        using Basis::Min;
        using Basis::x;
        using Basis::y;

        [[nodiscard]] f32 Length() const;
        [[nodiscard]] f32 LengthSquare() const;

        [[nodiscard]] f32 Distance(const _Vector &other) const;
        [[nodiscard]] f32 DistanceSquare(const _Vector &other) const;

        [[nodiscard]] f32 Dot(const _Vector &other) const;

        [[nodiscard]] f32 Average() const;

        void Normalize();
        [[nodiscard]] VectorType GetNormalized() const;
        [[nodiscard]] bool IsNormalized(f32 epsilon = DefaultF32Epsilon) const;

        [[nodiscard]] bool EqualsWithEpsilon(const _Vector &other, f32 epsilon = DefaultF32Epsilon) const;

        void Lerp(const _Vector &other, f32 interpolant);
        [[nodiscard]] VectorType GetLerped(const _Vector &other, f32 interpolant) const;

        template <uiw Rows, uiw Columns> [[nodiscard]] VectorTypeByDimension<f32, Columns> operator * (const _Matrix<Rows, Columns> &matrix) const;
        template <uiw Rows, uiw Columns> VectorType &operator *= (const _Matrix<Rows, Columns> &matrix);
    };

    struct Vector2 : _Vector<Vector2Base<f32>>
    {
        using _Vector::_Vector;

        [[nodiscard]] Vector2 GetLeftNormal() const; // no normalization
        [[nodiscard]] Vector2 GetRightNormal() const; // no normalization
    };

    struct Vector3 : _Vector<Vector3Base<f32>>
    {
        using _Vector::_Vector;

        void Cross(const Vector3 &other);
        [[nodiscard]] Vector3 GetCrossed(const Vector3 &other) const;
    };

    struct Vector4 : _Vector<Vector4Base<f32>>
    {
        using _Vector::_Vector;
    };

    template <uiw Rows, uiw Columns> struct _Matrix
    {
        using MatrixType = MatrixTypeByDimensions<Rows, Columns>;

        static_assert(Rows > 1);
        static_assert(Columns > 1);

        static constexpr uiw rows = Rows;
        static constexpr uiw columns = Columns;
        static constexpr uiw numberOfElements = Rows * Columns;
        std::array<std::array<f32, Columns>, Rows> elements;

        [[nodiscard]] MatrixType operator + (const _Matrix &other) const;
        MatrixType &operator += (const _Matrix &other);

        [[nodiscard]] MatrixType operator - (const _Matrix &other) const;
        MatrixType &operator -= (const _Matrix &other);

        [[nodiscard]] MatrixType operator * (const _Matrix &other) const;
        MatrixType &operator *= (const _Matrix &other);

        [[nodiscard]] MatrixType operator * (f32 scalar) const;
        MatrixType &operator *= (f32 scalar);

        [[nodiscard]] std::array<f32, Columns> &operator [] (uiw index);
        [[nodiscard]] const std::array<f32, Columns> &operator [] (uiw index) const;

        [[nodiscard]] f32 *Data();
        [[nodiscard]] const f32 *Data() const;

        void Transpose();
        [[nodiscard]] MatrixTypeByDimensions<Columns, Rows> GetTransposed() const;

        [[nodiscard]] VectorTypeByDimension<f32, Columns> GetRow(uiw rowIndex) const;
        void SetRow(uiw rowIndex, const VectorTypeByDimension<f32, Columns> &row);

        [[nodiscard]] VectorTypeByDimension<f32, Rows> GetColumn(uiw columnIndex) const;
        void SetColumn(uiw columnIndex, const VectorTypeByDimension<f32, Rows> &column);

        [[nodiscard]] f32 FetchValueBoundless(uiw rowIndex, uiw columnIndex) const;
        void StoreValueBoundless(uiw rowIndex, uiw columnIndex, f32 value);

        [[nodiscard]] bool Equals(const _Matrix &other) const;
        [[nodiscard]] bool EqualsWithEpsilon(const _Matrix &other, f32 epsilon = DefaultF32Epsilon) const;

    protected:
        _Matrix(); // will create identity matrix
        template <typename... Args> _Matrix(Args &&... args) : elements{args...} {}
    };

    struct Matrix4x3 : _Matrix<4, 3>
    {
        Matrix4x3() = default;

        Matrix4x3(f32 e00, f32 e01, f32 e02,
            f32 e10, f32 e11, f32 e12,
            f32 e20, f32 e21, f32 e22,
            f32 e30, f32 e31, f32 e32);

        Matrix4x3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2, const Vector3 &row3);

        [[nodiscard]] Matrix4x4 operator * (const Matrix4x4 &other) const;

        [[nodiscard]] static Matrix4x3 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix4x3 CreateRTS(const optional<Vector3> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale = nullopt);
        [[nodiscard]] static Matrix4x3 CreateRTS(const optional<Quaternion> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale = nullopt);
    };

    struct Matrix3x4 : _Matrix<3, 4>
    {
        Matrix3x4() = default;

        Matrix3x4(f32 e00, f32 e01, f32 e02, f32 e03,
            f32 e10, f32 e11, f32 e12, f32 e13,
            f32 e20, f32 e21, f32 e22, f32 e23);

        Matrix3x4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2);

        [[nodiscard]] static Matrix3x4 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix3x4 CreateRS(const optional<Vector3> &rotation, const optional<Vector3> &scale = nullopt);
        [[nodiscard]] static Matrix3x4 CreateRS(const optional<Quaternion> &rotation, const optional<Vector3> &scale = nullopt);
    };

    struct Matrix4x4 : _Matrix<4, 4>
    {
        Matrix4x4() = default;

        Matrix4x4(f32 e00, f32 e01, f32 e02, f32 e03,
            f32 e10, f32 e11, f32 e12, f32 e13,
            f32 e20, f32 e21, f32 e22, f32 e23,
            f32 e30, f32 e31, f32 e32, f32 e33);

        Matrix4x4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2, const Vector4 &row3);

        [[nodiscard]] static Matrix4x4 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix4x4 CreateRTS(const optional<Vector3> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale = nullopt);
        [[nodiscard]] static Matrix4x4 CreateRTS(const optional<Quaternion> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale = nullopt);
        [[nodiscard]] static Matrix4x4 CreatePerspectiveProjection(f32 horizontalFOV, f32 aspectRatio, f32 nearPlane, f32 farPlane);
        // TODO: orthogonal projection
    };

    struct Matrix3x2 : _Matrix<3, 2>
    {
        Matrix3x2() = default;

        Matrix3x2(f32 e00, f32 e01,
            f32 e10, f32 e11,
            f32 e20, f32 e21);

        Matrix3x2(const Vector2 &row0, const Vector2 &row1, const Vector2 &row2);

        [[nodiscard]] Matrix3x3 GetInversed() const;
        [[nodiscard]] Matrix3x2 GetInversedClipped() const;

        // clockwise rotation around Z axis
        [[nodiscard]] static Matrix3x2 CreateRTS(const optional<f32> &rotation = nullopt, const optional<Vector2> &translation = nullopt, const optional<Vector2> &scale = nullopt);
    };

    struct Matrix2x3 : _Matrix<2, 3>
    {
        Matrix2x3() = default;

        Matrix2x3(f32 e00, f32 e01, f32 e02,
            f32 e10, f32 e11, f32 e12);

        Matrix2x3(const Vector3 &row0, const Vector3 &row1);
    };

    struct Matrix2x2 : _Matrix<2, 2>
    {
        Matrix2x2() = default;

        Matrix2x2(f32 e00, f32 e01,
            f32 e10, f32 e11);

        Matrix2x2(const Vector2 &row0, const Vector2 &row1);
    };

    struct Matrix3x3 : _Matrix<3, 3>
    {
        Matrix3x3() = default;

        Matrix3x3(f32 e00, f32 e01, f32 e02,
            f32 e10, f32 e11, f32 e12,
            f32 e20, f32 e21, f32 e22);

        Matrix3x3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2);

        [[nodiscard]] static Matrix3x3 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix3x3 CreateRS(const optional<Vector3> &rotation, const optional<Vector3> &scale = nullopt);
        [[nodiscard]] static Matrix3x3 CreateRS(const optional<Quaternion> &rotation, const optional<Vector3> &scale = nullopt);
    };

    // Addition and subtraction are a component-wise operation; composing quaternions should be done via multiplication.
    // Order matters when composing quaternions : C = A * B will yield a quaternion C that logically
    // first applies B then A to any subsequent transformation(right first, then left).
    struct Quaternion
    {
        f32 x = 0, y = 0, z = 0, w = 1;

        static Quaternion FromEuler(const Vector3 &source);

        Quaternion() = default;
        Quaternion(f32 x, f32 y, f32 z, f32 w);
        explicit Quaternion(const Matrix3x3 &matrix);
        Quaternion(const Vector3 &axis, f32 angle); // axis must be normalized, angle is in radians

        [[nodiscard]] Quaternion operator + (const Quaternion &other) const;
        Quaternion &operator += (const Quaternion &other);

        [[nodiscard]] Quaternion operator - (const Quaternion &other) const;
        Quaternion &operator -= (const Quaternion &other);

        [[nodiscard]] Quaternion operator * (const Quaternion &other) const;
        Quaternion &operator *= (const Quaternion &other);

        [[nodiscard]] Quaternion operator * (f32 scale) const;
        Quaternion &operator *= (f32 scale);

        [[nodiscard]] Quaternion operator / (f32 scale) const;
        Quaternion &operator /= (f32 scale);

        [[nodiscard]] f32 &operator [] (uiw index);
        [[nodiscard]] const f32 &operator [] (uiw index) const;

        [[nodiscard]] f32 *Data();
        [[nodiscard]] const f32 *Data() const;

        [[nodiscard]] Vector3 RotateVector(const Vector3 &source) const;

        void Normalize();
        [[nodiscard]] Quaternion GetNormalized() const;
        [[nodiscard]] bool IsNormalized(f32 epsilon = DefaultF32Epsilon) const;

        void Inverse();
        [[nodiscard]] Quaternion GetInversed() const;

        [[nodiscard]] f32 GetAngle() const;
        [[nodiscard]] Vector3 GetRotationAxis() const;

        [[nodiscard]] Vector3 ToEuler() const;
        [[nodiscard]] std::tuple<Vector3, f32> ToAxisAndAngle() const;
        [[nodiscard]] Matrix3x3 ToMatrix() const;

        [[nodiscard]] bool Equals(const Quaternion &other) const;
        [[nodiscard]] bool EqualsWithEpsilon(const Quaternion &other, f32 epsilon = DefaultF32Epsilon) const;
    };

    template <typename T, bool isTopLessThanBottom = true> struct Rectangle
    {
        T left = std::numeric_limits<T>::min();
        T right = std::numeric_limits<T>::min();
        T top = std::numeric_limits<T>::min();
        T bottom = std::numeric_limits<T>::min();

        static Rectangle FromPoint(T x, T y);

        constexpr Rectangle() = default; // TODO: constexpr constructor with arguments validation
        [[nodiscard]] bool IsIntersected(const Rectangle &other) const; // undefined rectangles are considered instersected
        [[nodiscard]] T Distance(T x, T y) const;
        Rectangle &Expand(const Rectangle &other);
        Rectangle &Expand(T x, T y);
        [[nodiscard]] Rectangle GetExpanded(const Rectangle &other) const;
        [[nodiscard]] Rectangle GetExpanded(T x, T y) const;
        [[nodiscard]] bool IsDefined() const;
        [[nodiscard]] T Width() const;
        [[nodiscard]] T Height() const;

        [[nodiscard]] bool operator == (const Rectangle &other) const;
        [[nodiscard]] bool operator != (const Rectangle &other) const;
    };

    using RectangleF32 = Rectangle<f32>;
    using RectangleI32 = Rectangle<i32>;
    using RectangleUI32 = Rectangle<ui32>;

    /////////////
    // _Vector //
    /////////////

    template<typename Basis>
    template<uiw Rows, uiw Columns>
    inline auto _Vector<Basis>::operator*(const _Matrix<Rows, Columns> &matrix) const -> VectorTypeByDimension<f32, Columns>
    {
        static_assert(dim == Rows, "Cannot multiply vector and matrix, matrix's rows number and vector dimension must be equal");

        VectorType result;

        for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
        {
            result[columnIndex] = Dot(matrix.GetColumn(columnIndex));
        }

        return result;
    }

    template<typename Basis>
    template<uiw Rows, uiw Columns>
    inline auto _Vector<Basis>::operator*=(const _Matrix<Rows, Columns> &matrix) -> VectorType &
    {
        *this = *this * matrix;
        return *this;
    }
}