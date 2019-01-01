#pragma once

#include "CoreHeader.hpp"

// TODO: try to make vectors decomposable, so you would be able to perform auto [x, y, z] = Vector3{};
// TODO: inverse matrix
// TODO: determinant
// TODO: matrix conversions (3x3 to 4x4, 4x4 to 3x3 etc.)
// TODO: Rectangle3D
 // TODO: constexpr constructors with arguments validation

namespace StdLib
{
    template <typename ScalarType> struct Vector2Base;
    template <typename ScalarType> struct Vector3Base;
    template <typename ScalarType> struct Vector4Base;

    struct Vector2;
    struct Vector3;
    struct Vector4;

    struct i32Vector2;
    struct i32Vector3;
    struct i32Vector4;

    struct ui32Vector2;
    struct ui32Vector3;
    struct ui32Vector4;

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

    template <> struct _ChooseVectorType<i32, 2> { using Type = i32Vector2; };
    template <> struct _ChooseVectorType<i32, 3> { using Type = i32Vector3; };
    template <> struct _ChooseVectorType<i32, 4> { using Type = i32Vector4; };

    template <> struct _ChooseVectorType<ui32, 2> { using Type = ui32Vector2; };
    template <> struct _ChooseVectorType<ui32, 3> { using Type = ui32Vector3; };
    template <> struct _ChooseVectorType<ui32, 4> { using Type = ui32Vector4; };

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
        static_assert(Dim > 1);

        static constexpr uiw dim = Dim;

        using ScalarType = _ScalarType;
        using VectorType = VectorTypeByDimension<ScalarType, dim>;

        ScalarType x = 0, y = 0;

        constexpr _VectorBase() = default;
        constexpr _VectorBase(ScalarType x, ScalarType y);

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
        [[nodiscard]] friend VectorType operator * (ScalarType scalar, const _VectorBase &vector) { return vector * scalar; } // TODO: do we need this?

        VectorType &operator *= (const _VectorBase &other);
        VectorType &operator *= (ScalarType scalar);

        [[nodiscard]] VectorType operator / (const _VectorBase &other) const;
        [[nodiscard]] VectorType operator / (ScalarType scalar) const;
        [[nodiscard]] friend VectorType operator / (ScalarType scalar, const _VectorBase &vector) { return vector / scalar; } // TODO: do we need this?

        VectorType &operator /= (const _VectorBase &other);
        VectorType &operator /= (ScalarType scalar);

		[[nodiscard]] bool operator == (const _VectorBase &other) const;
		[[nodiscard]] bool operator != (const _VectorBase &other) const;

        [[nodiscard]] ScalarType *Data();
        [[nodiscard]] const ScalarType *Data() const;
        
        [[nodiscard]] ScalarType Accumulate() const;
        [[nodiscard]] ScalarType Max() const;
        [[nodiscard]] ScalarType Min() const;
    };

    template <typename ScalarType> struct Vector2Base : _VectorBase<ScalarType, 2>
    {
        using _VectorBase<ScalarType, 2>::x;
        using _VectorBase<ScalarType, 2>::y;

        constexpr Vector2Base() = default;
        constexpr Vector2Base(ScalarType x, ScalarType y);
        [[nodiscard]] ScalarType &operator [] (uiw index);
        [[nodiscard]] const ScalarType &operator [] (uiw index) const;
    };

    template <typename ScalarType> struct Vector3Base : _VectorBase<ScalarType, 3>
    {
        using _VectorBase<ScalarType, 3>::x;
        using _VectorBase<ScalarType, 3>::y;
        ScalarType z = 0;

        constexpr Vector3Base() = default;
        constexpr Vector3Base(ScalarType x, ScalarType y, ScalarType z);
        constexpr Vector3Base(const VectorTypeByDimension<ScalarType, 2> &vec, ScalarType z);
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

        constexpr Vector4Base() = default;
        constexpr Vector4Base(ScalarType x, ScalarType y, ScalarType z, ScalarType w);
        constexpr Vector4Base(const VectorTypeByDimension<ScalarType, 2> &vec, ScalarType z, ScalarType w);
        constexpr Vector4Base(const VectorTypeByDimension<ScalarType, 3> &vec, ScalarType w);
        constexpr Vector4Base(const VectorTypeByDimension<ScalarType, 2> &v0, const VectorTypeByDimension<ScalarType, 2> &v1);
        [[nodiscard]] ScalarType &operator [] (uiw index);
        [[nodiscard]] const ScalarType &operator [] (uiw index) const;
        [[nodiscard]] VectorTypeByDimension<ScalarType, 2> ToVector2() const;
        [[nodiscard]] VectorTypeByDimension<ScalarType, 3> ToVector3() const;
    };

    template <typename Basis> struct _VectorFP : Basis
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

        [[nodiscard]] f32 Distance(const _VectorFP &other) const;
        [[nodiscard]] f32 DistanceSquare(const _VectorFP &other) const;

        [[nodiscard]] f32 Dot(const _VectorFP &other) const;

        [[nodiscard]] f32 Average() const;

        void Normalize();
        [[nodiscard]] VectorType GetNormalized() const;
        [[nodiscard]] bool IsNormalized(f32 epsilon = DefaultF32Epsilon) const;

        [[nodiscard]] bool EqualsWithEpsilon(const _VectorFP &other, f32 epsilon = DefaultF32Epsilon) const;

        void Lerp(const _VectorFP &other, f32 interpolant);
        [[nodiscard]] VectorType GetLerped(const _VectorFP &other, f32 interpolant) const;

        template <uiw Rows, uiw Columns> [[nodiscard]] VectorTypeByDimension<f32, Columns> operator * (const _Matrix<Rows, Columns> &matrix) const;
        template <uiw Rows, uiw Columns> VectorType &operator *= (const _Matrix<Rows, Columns> &matrix);
    };

    struct Vector2 : _VectorFP<Vector2Base<f32>>
    {
        using _VectorFP::_VectorFP;

        [[nodiscard]] Vector2 GetLeftNormal() const; // no normalization
        [[nodiscard]] Vector2 GetRightNormal() const; // no normalization
    };

    struct Vector3 : _VectorFP<Vector3Base<f32>>
    {
        using _VectorFP::_VectorFP;

        void Cross(const Vector3 &other);
        [[nodiscard]] Vector3 GetCrossed(const Vector3 &other) const;
    };

    struct Vector4 : _VectorFP<Vector4Base<f32>>
    {
        using _VectorFP::_VectorFP;
    };

    struct i32Vector2 : Vector2Base<i32>
    {
        using Vector2Base::Vector2Base;
    };

    struct i32Vector3 : Vector3Base<i32>
    {
        using Vector3Base::Vector3Base;
    };

    struct i32Vector4 : Vector4Base<i32>
    {
        using Vector4Base::Vector4Base;
    };

    struct ui32Vector2 : Vector2Base<ui32>
    {
        using Vector2Base::Vector2Base;
    };

    struct ui32Vector3 : Vector3Base<ui32>
    {
        using Vector3Base::Vector3Base;
    };

    struct ui32Vector4 : Vector4Base<ui32>
    {
        using Vector4Base::Vector4Base;
    };

    template <uiw Rows, uiw Columns> struct _Matrix
    {
        static_assert(Rows > 1);
        static_assert(Columns > 1);

        using MatrixType = MatrixTypeByDimensions<Rows, Columns>;

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

        [[nodiscard]] MatrixTypeByDimensions<Columns, Rows> GetTransposed() const;

        [[nodiscard]] VectorTypeByDimension<f32, Columns> GetRow(uiw rowIndex) const;
        void SetRow(uiw rowIndex, const VectorTypeByDimension<f32, Columns> &row);

        [[nodiscard]] VectorTypeByDimension<f32, Rows> GetColumn(uiw columnIndex) const;
        void SetColumn(uiw columnIndex, const VectorTypeByDimension<f32, Rows> &column);

        [[nodiscard]] f32 FetchValueBoundless(uiw rowIndex, uiw columnIndex) const;
        void StoreValueBoundless(uiw rowIndex, uiw columnIndex, f32 value);

        [[nodiscard]] bool EqualsWithEpsilon(const _Matrix &other, f32 epsilon = DefaultF32Epsilon) const;

    protected:
        constexpr _Matrix(); // will create identity matrix
        template <typename... Args> constexpr _Matrix(Args &&... args) : elements{args...} {}
    };

    struct Matrix4x3 : _Matrix<4, 3>
    {
        constexpr Matrix4x3() = default;

        constexpr Matrix4x3(f32 e00, f32 e01, f32 e02,
            f32 e10, f32 e11, f32 e12,
            f32 e20, f32 e21, f32 e22,
            f32 e30, f32 e31, f32 e32);

        constexpr Matrix4x3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2, const Vector3 &row3);

        [[nodiscard]] Matrix4x4 operator * (const Matrix4x4 &other) const;

        [[nodiscard]] static Matrix4x3 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix4x3 CreateRTS(const optional<Vector3> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale = nullopt);
        [[nodiscard]] static Matrix4x3 CreateRTS(const optional<Quaternion> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale = nullopt);
        [[nodiscard]] static Matrix4x3 CreateOrthographicProjection(const Vector3 &min, const Vector3 &max);
    };

    struct Matrix3x4 : _Matrix<3, 4>
    {
        constexpr Matrix3x4() = default;

        constexpr Matrix3x4(f32 e00, f32 e01, f32 e02, f32 e03,
            f32 e10, f32 e11, f32 e12, f32 e13,
            f32 e20, f32 e21, f32 e22, f32 e23);

        constexpr Matrix3x4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2);

        [[nodiscard]] static Matrix3x4 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix3x4 CreateRS(const optional<Vector3> &rotation, const optional<Vector3> &scale = nullopt);
        [[nodiscard]] static Matrix3x4 CreateRS(const optional<Quaternion> &rotation, const optional<Vector3> &scale = nullopt);
    };

    struct Matrix4x4 : _Matrix<4, 4>
    {
        constexpr Matrix4x4() = default;

        constexpr Matrix4x4(f32 e00, f32 e01, f32 e02, f32 e03,
            f32 e10, f32 e11, f32 e12, f32 e13,
            f32 e20, f32 e21, f32 e22, f32 e23,
            f32 e30, f32 e31, f32 e32, f32 e33);

        constexpr Matrix4x4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2, const Vector4 &row3);
		
		void Transpose();

        [[nodiscard]] static Matrix4x4 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix4x4 CreateRTS(const optional<Vector3> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale = nullopt);
        [[nodiscard]] static Matrix4x4 CreateRTS(const optional<Quaternion> &rotation, const optional<Vector3> &translation, const optional<Vector3> &scale = nullopt);
        [[nodiscard]] static Matrix4x4 CreatePerspectiveProjection(f32 horizontalFOV, f32 aspectRatio, f32 nearPlane, f32 farPlane);
        [[nodiscard]] static Matrix4x4 CreateOrthographicProjection(const Vector3 &min, const Vector3 &max);
    };

    struct Matrix3x2 : _Matrix<3, 2>
    {
        constexpr Matrix3x2() = default;

        constexpr Matrix3x2(f32 e00, f32 e01,
            f32 e10, f32 e11,
            f32 e20, f32 e21);

        constexpr Matrix3x2(const Vector2 &row0, const Vector2 &row1, const Vector2 &row2);

        [[nodiscard]] Matrix3x3 GetInversed() const;
        [[nodiscard]] Matrix3x2 GetInversedClipped() const;

        // clockwise rotation around Z axis
        [[nodiscard]] static Matrix3x2 CreateRTS(const optional<f32> &rotation = nullopt, const optional<Vector2> &translation = nullopt, const optional<Vector2> &scale = nullopt);
    };

    struct Matrix2x3 : _Matrix<2, 3>
    {
        constexpr Matrix2x3() = default;

        constexpr Matrix2x3(f32 e00, f32 e01, f32 e02,
            f32 e10, f32 e11, f32 e12);

        constexpr Matrix2x3(const Vector3 &row0, const Vector3 &row1);
    };

    struct Matrix2x2 : _Matrix<2, 2>
    {
        constexpr Matrix2x2() = default;

        constexpr Matrix2x2(f32 e00, f32 e01,
            f32 e10, f32 e11);

        constexpr Matrix2x2(const Vector2 &row0, const Vector2 &row1);
		
		void Transpose();
	};

    struct Matrix3x3 : _Matrix<3, 3>
    {
        constexpr Matrix3x3() = default;

        constexpr Matrix3x3(f32 e00, f32 e01, f32 e02,
            f32 e10, f32 e11, f32 e12,
            f32 e20, f32 e21, f32 e22);

        constexpr Matrix3x3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2);
		
		void Transpose();

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

        constexpr Quaternion() = default;
        Quaternion(f32 x, f32 y, f32 z, f32 w);
        explicit Quaternion(const Matrix3x3 &matrix);
        Quaternion(const Vector3 &axis, f32 angleRad); // axis must be normalized

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

        [[nodiscard]] bool EqualsWithEpsilon(const Quaternion &other, f32 epsilon = DefaultF32Epsilon) const;
    };

    template <typename T, bool isTopLessThanBottom = true> struct Rectangle
    {
        T left = std::numeric_limits<T>::min();
        T right = std::numeric_limits<T>::min();
        T top = std::numeric_limits<T>::min();
        T bottom = std::numeric_limits<T>::min();

        static constexpr Rectangle FromPoint(T x, T y);

        constexpr Rectangle() = default;
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
    inline auto _VectorFP<Basis>::operator * (const _Matrix<Rows, Columns> &matrix) const -> VectorTypeByDimension<f32, Columns>
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
    inline auto _VectorFP<Basis>::operator *= (const _Matrix<Rows, Columns> &matrix) -> VectorType &
    {
        *this = *this * matrix;
        return (VectorType &)*this;
    }

    // constepxr is implicitly inline, so these definitions must be in the header

    /////////////////
    // _VectorBase //
    /////////////////

    template<typename _ScalarType, uiw Dim> inline constexpr _VectorBase<_ScalarType, Dim>::_VectorBase(ScalarType x, ScalarType y) : x(x), y(y)
    {}

    /////////////////
    // Vector2Base //
    /////////////////

    template<typename ScalarType> inline constexpr Vector2Base<ScalarType>::Vector2Base(ScalarType x, ScalarType y) : _VectorBase<ScalarType, 2>(x, y)
    {}

    /////////////////
    // Vector3Base //
    /////////////////

    template<typename ScalarType> inline constexpr Vector3Base<ScalarType>::Vector3Base(ScalarType x, ScalarType y, ScalarType z) : _VectorBase<ScalarType, 3>(x, y), z(z)
    {}

    template<typename ScalarType> inline constexpr Vector3Base<ScalarType>::Vector3Base(const VectorTypeByDimension<ScalarType, 2> &vec, ScalarType z) : _VectorBase<ScalarType, 3>(vec.x, vec.y), z(z)
    {}

    /////////////////
    // Vector4Base //
    /////////////////

    template<typename ScalarType> inline constexpr Vector4Base<ScalarType>::Vector4Base(ScalarType x, ScalarType y, ScalarType z, ScalarType w) : _VectorBase<ScalarType, 4>(x, y), z(z), w(w)
    {}

    template<typename ScalarType> inline constexpr Vector4Base<ScalarType>::Vector4Base(const VectorTypeByDimension<ScalarType, 2> &vec, ScalarType z, ScalarType w) : _VectorBase<ScalarType, 4>(vec.x, vec.y), z(z), w(w)
    {}

    template<typename ScalarType> inline constexpr Vector4Base<ScalarType>::Vector4Base(const VectorTypeByDimension<ScalarType, 3> &vec, ScalarType w) : _VectorBase<ScalarType, 4>(vec.x, vec.y), z(vec.z), w(w)
    {}

    template<typename ScalarType> inline constexpr Vector4Base<ScalarType>::Vector4Base(const VectorTypeByDimension<ScalarType, 2> &v0, const VectorTypeByDimension<ScalarType, 2> &v1) : _VectorBase<ScalarType, 4>(v0.x, v0.y), z(v1.x), w(v1.y)
    {}

    /////////////
    // _Matrix //
    /////////////

    template<uiw Rows, uiw Columns> inline constexpr _Matrix<Rows, Columns>::_Matrix()
    {
        for (uiw rowIndex = 0; rowIndex < Rows; ++rowIndex)
        {
            for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
            {
                if (rowIndex == columnIndex)
                {
                    elements[rowIndex][columnIndex] = 1.0f;
                }
                else
                {
                    elements[rowIndex][columnIndex] = 0.0f;
                }
            }
        }
    }

    ///////////////
    // Matrix2x2 //
    ///////////////

    constexpr Matrix2x2::Matrix2x2(f32 e00, f32 e01, f32 e10, f32 e11) : _Matrix(e00, e01, e10, e11)
    {}

    constexpr Matrix2x2::Matrix2x2(const Vector2 &row0, const Vector2 &row1) : _Matrix(row0.x, row0.y, row1.x, row1.y)
    {}

    ///////////////
    // Matrix3x2 //
    ///////////////

    constexpr Matrix3x2::Matrix3x2(f32 e00, f32 e01, f32 e10, f32 e11, f32 e20, f32 e21) : _Matrix(e00, e01, e10, e11, e20, e21)
    {}

    constexpr Matrix3x2::Matrix3x2(const Vector2 &row0, const Vector2 &row1, const Vector2 &row2) : _Matrix(row0.x, row0.y, row1.x, row1.y, row2.x, row2.y)
    {}

    ///////////////
    // Matrix2x3 //
    ///////////////

    constexpr Matrix2x3::Matrix2x3(f32 e00, f32 e01, f32 e02, f32 e10, f32 e11, f32 e12) : _Matrix(e00, e01, e02, e10, e11, e12)
    {}

    constexpr Matrix2x3::Matrix2x3(const Vector3 &row0, const Vector3 &row1) : _Matrix(row0.x, row0.y, row0.z, row1.x, row1.y, row1.z)
    {}

    ///////////////
    // Matrix4x3 //
    ///////////////

    constexpr Matrix4x3::Matrix4x3(f32 e00, f32 e01, f32 e02, f32 e10, f32 e11, f32 e12, f32 e20, f32 e21, f32 e22, f32 e30, f32 e31, f32 e32) : _Matrix(e00, e01, e02, e10, e11, e12, e20, e21, e22, e30, e31, e32)
    {}

    constexpr Matrix4x3::Matrix4x3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2, const Vector3 &row3) : _Matrix(row0.x, row0.y, row0.z, row1.x, row1.y, row1.z, row2.x, row2.y, row2.z, row3.x, row3.y, row3.z)
    {}

    ///////////////
    // Matrix3x4 //
    ///////////////

    constexpr Matrix3x4::Matrix3x4(f32 e00, f32 e01, f32 e02, f32 e03, f32 e10, f32 e11, f32 e12, f32 e13, f32 e20, f32 e21, f32 e22, f32 e23) : _Matrix(e00, e01, e02, e03, e10, e11, e12, e13, e20, e21, e22, e23)
    {}

    constexpr Matrix3x4::Matrix3x4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2) : _Matrix(row0.x, row0.y, row0.z, row0.w, row1.x, row1.y, row1.z, row1.w, row2.x, row2.y, row2.z, row2.w)
    {}

    ///////////////
    // Matrix4x4 //
    ///////////////

    constexpr Matrix4x4::Matrix4x4(f32 e00, f32 e01, f32 e02, f32 e03, f32 e10, f32 e11, f32 e12, f32 e13, f32 e20, f32 e21, f32 e22, f32 e23, f32 e30, f32 e31, f32 e32, f32 e33) : _Matrix(e00, e01, e02, e03, e10, e11, e12, e13, e20, e21, e22, e23, e30, e31, e32, e33)
    {}

    constexpr Matrix4x4::Matrix4x4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2, const Vector4 &row3) : _Matrix(row0.x, row0.y, row0.z, row0.w, row1.x, row1.y, row1.z, row1.w, row2.x, row2.y, row2.z, row2.w, row3.x, row3.y, row3.z, row3.w)
    {}

    ///////////////
    // Matrix3x3 //
    ///////////////

    constexpr Matrix3x3::Matrix3x3(f32 e00, f32 e01, f32 e02, f32 e10, f32 e11, f32 e12, f32 e20, f32 e21, f32 e22) : _Matrix(e00, e01, e02, e10, e11, e12, e20, e21, e22)
    {}

    constexpr Matrix3x3::Matrix3x3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2) : _Matrix(row0.x, row0.y, row0.z, row1.x, row1.y, row1.z, row2.x, row2.y, row2.z)
    {}

    ///////////////
    // Rectangle //
    ///////////////

    template <typename T, bool isTopLessThanBottom> inline constexpr auto Rectangle<T, isTopLessThanBottom>::FromPoint(T x, T y) -> Rectangle
    {
        return {x, x, y, y};
    }
}