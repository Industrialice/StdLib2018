#pragma once

#include "CoreHeader.hpp"

// TODO: refactor Rectangle and write proper tests
// TODO: Rectangle3D
// TODO: more tests for Quaternion

namespace StdLib
{
	template <typename _ScalarType, uiw Dim> struct _VectorBase;
	template <typename _ScalarType, uiw Dim> struct _VectorArithmeticBase;

    template <typename BaseType> struct Vector2Base;
    template <typename BaseType> struct Vector3Base;
    template <typename BaseType> struct Vector4Base;

    struct Vector2;
    struct Vector3;
    struct Vector4;

    struct i32Vector2;
    struct i32Vector3;
    struct i32Vector4;

    struct ui32Vector2;
    struct ui32Vector3;
    struct ui32Vector4;

	struct boolVector2;
	struct boolVector3;
	struct boolVector4;

    template <uiw Rows, uiw Columns> struct _Matrix;

	struct Matrix2x2;
	struct Matrix2x3;
    struct Matrix3x2;
    struct Matrix3x3;
	struct Matrix4x3;
	struct Matrix3x4;
	struct Matrix4x4;

    struct Quaternion;

	template <typename ScalarType, uiw Dim> struct _ChooseVectorBase { using Type = _VectorArithmeticBase<ScalarType, Dim>; };
	template <uiw Dim> struct _ChooseVectorBase<bool, Dim> { using Type = _VectorBase<bool, Dim>; };

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

	template <> struct _ChooseVectorType<bool, 2> { using Type = boolVector2; };
	template <> struct _ChooseVectorType<bool, 3> { using Type = boolVector3; };
	template <> struct _ChooseVectorType<bool, 4> { using Type = boolVector4; };

    template <typename ScalarType> struct _ChooseVectorType<ScalarType, 2> { using Type = Vector2Base<typename _ChooseVectorBase<ScalarType, 2>::Type>; };
    template <typename ScalarType> struct _ChooseVectorType<ScalarType, 3> { using Type = Vector3Base<typename _ChooseVectorBase<ScalarType, 3>::Type>; };
    template <typename ScalarType> struct _ChooseVectorType<ScalarType, 4> { using Type = Vector4Base<typename _ChooseVectorBase<ScalarType, 4>::Type>; };

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

    template <typename _ScalarType, uiw Dim> struct _VectorElements;
    
    template <typename _ScalarType> struct _VectorElements<_ScalarType, 2>
    {
        _ScalarType x = 0, y = 0;
    };

    template <typename _ScalarType> struct _VectorElements<_ScalarType, 3>
    {
        _ScalarType x = 0, y = 0, z = 0;
    };

    template <typename _ScalarType> struct _VectorElements<_ScalarType, 4>
    {
        _ScalarType x = 0, y = 0, z = 0, w = 1;
    };

    template <typename _ScalarType, uiw Dim> struct _VectorBase : _VectorElements<_ScalarType, Dim>
    {
        static_assert(Dim > 1);

        static constexpr uiw dim = Dim;

        using ScalarType = _ScalarType;
        using VectorType = VectorTypeByDimension<ScalarType, dim>;

        using _VectorElements<_ScalarType, Dim>::x;
        using _VectorElements<_ScalarType, Dim>::y;

        constexpr _VectorBase() = default;

        [[nodiscard]] bool operator == (const _VectorBase &other) const;
        [[nodiscard]] bool operator != (const _VectorBase &other) const;

        [[nodiscard]] std::array<ScalarType, Dim> &Data();
        [[nodiscard]] const std::array<ScalarType, Dim> &Data() const;

        [[nodiscard]] ScalarType &operator [] (uiw index);
        [[nodiscard]] const ScalarType &operator [] (uiw index) const;

		[[nodiscard]] uiw Hash() const;

    protected:
        template <typename... Args> constexpr _VectorBase(Args... args);
    };

	template <typename _ScalarType, uiw Dim> struct _VectorArithmeticBase : _VectorBase<_ScalarType, Dim>
	{
		using BaseType = typename _VectorBase<_ScalarType, Dim>;

		using BaseType::_VectorBase;
		using BaseType::operator ==;
		using BaseType::operator !=;
		using BaseType::Data;
		using BaseType::operator [];

		static constexpr uiw dim = Dim;

		using ScalarType = _ScalarType;
		using VectorType = VectorTypeByDimension<ScalarType, dim>;

		constexpr _VectorArithmeticBase() = default;

		[[nodiscard]] VectorType operator + (const _VectorArithmeticBase &other) const;
		[[nodiscard]] VectorType operator + (ScalarType scalar) const;

		VectorType &operator += (const _VectorArithmeticBase &other);
		VectorType &operator += (ScalarType scalar);

		[[nodiscard]] VectorType operator - (const _VectorArithmeticBase &other) const;
		[[nodiscard]] VectorType operator - (ScalarType scalar) const;

		VectorType &operator -= (const _VectorArithmeticBase &other);
		VectorType &operator -= (ScalarType scalar);

		[[nodiscard]] VectorType operator * (const _VectorArithmeticBase &other) const;
		[[nodiscard]] VectorType operator * (ScalarType scalar) const;

		VectorType &operator *= (const _VectorArithmeticBase &other);
		VectorType &operator *= (ScalarType scalar);

		[[nodiscard]] VectorType operator / (const _VectorArithmeticBase &other) const;
		[[nodiscard]] VectorType operator / (ScalarType scalar) const;

		VectorType &operator /= (const _VectorArithmeticBase &other);
		VectorType &operator /= (ScalarType scalar);

		VectorType operator - () const; // only valid for signed scalar types

		VectorType &ForEach(_ScalarType func(_ScalarType));
		template <typename ReturnType> VectorType &ForEach(ReturnType func(_ScalarType &));
		template <typename Func> VectorType &ForEach(Func func);
		template <typename ReturnType> const VectorType &ForEach(ReturnType func(_ScalarType)) const;
		template <typename Func> const VectorType &ForEach(Func func) const;
	};

    template <typename BaseType> struct Vector2Base : BaseType
    {
		using typename BaseType::ScalarType;
        using BaseType::x;
        using BaseType::y;

        constexpr Vector2Base() = default;
        constexpr Vector2Base(ScalarType x, ScalarType y);
		[[nodiscard]] VectorTypeByDimension<ScalarType, 2> ToVector2() const;
    };

    template <typename BaseType> struct Vector3Base : BaseType
    {
		using typename BaseType::ScalarType;
		using BaseType::x;
        using BaseType::y;

        constexpr Vector3Base() = default;
        constexpr Vector3Base(ScalarType x, ScalarType y, ScalarType z);
        constexpr Vector3Base(const VectorTypeByDimension<ScalarType, 2> &vec, ScalarType z);
		[[nodiscard]] VectorTypeByDimension<ScalarType, 2> ToVector2() const;
		[[nodiscard]] VectorTypeByDimension<ScalarType, 3> ToVector3() const;
    };

    template <typename BaseType> struct Vector4Base : BaseType
    {
		using typename BaseType::ScalarType;
		using BaseType::x;
        using BaseType::y;

        constexpr Vector4Base() = default;
        constexpr Vector4Base(ScalarType x, ScalarType y, ScalarType z, ScalarType w);
        constexpr Vector4Base(const VectorTypeByDimension<ScalarType, 2> &vec, ScalarType z, ScalarType w);
        constexpr Vector4Base(const VectorTypeByDimension<ScalarType, 3> &vec, ScalarType w);
        constexpr Vector4Base(const VectorTypeByDimension<ScalarType, 2> &v0, const VectorTypeByDimension<ScalarType, 2> &v1);
        [[nodiscard]] VectorTypeByDimension<ScalarType, 2> ToVector2() const;
		[[nodiscard]] VectorTypeByDimension<ScalarType, 3> ToVector3() const;
		[[nodiscard]] VectorTypeByDimension<ScalarType, 4> ToVector4() const;
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
        using Basis::x;
        using Basis::y;

        [[nodiscard]] f32 Length() const;
        [[nodiscard]] f32 LengthSquare() const;

        [[nodiscard]] f32 Distance(const _VectorFP &other) const;
        [[nodiscard]] f32 DistanceSquare(const _VectorFP &other) const;

        [[nodiscard]] f32 Dot(const _VectorFP &other) const;

        VectorType &Normalize();
		[[nodiscard]] VectorType GetNormalized() const;
		[[nodiscard]] bool IsNormalized() const;

        [[nodiscard]] bool EqualsWithEpsilon(const _VectorFP &other, f32 epsilon = DefaultF32Epsilon) const;

        VectorType &Lerp(const _VectorFP &other, f32 interpolant);
        [[nodiscard]] VectorType GetLerped(const _VectorFP &other, f32 interpolant) const;

        template <uiw Rows, uiw Columns>[[nodiscard]] VectorTypeByDimension<f32, Columns> operator * (const _Matrix<Rows, Columns> &matrix) const;
        template <uiw Rows, uiw Columns> VectorType &operator *= (const _Matrix<Rows, Columns> &matrix);
    };

    struct Vector2 : _VectorFP<Vector2Base<_VectorArithmeticBase<f32, 2>>>
    {
        using _VectorFP::_VectorFP;

        [[nodiscard]] Vector2 GetLeftNormal() const; // no normalization
        [[nodiscard]] Vector2 GetRightNormal() const; // no normalization
    };

    struct Vector3 : _VectorFP<Vector3Base<_VectorArithmeticBase<f32, 3>>>
    {
        using _VectorFP::_VectorFP;

        Vector3 &Cross(const Vector3 &other);
        [[nodiscard]] Vector3 GetCrossed(const Vector3 &other) const;
    };

    struct Vector4 : _VectorFP<Vector4Base<_VectorArithmeticBase<f32, 4>>>
    {
        using _VectorFP::_VectorFP;
    };

    struct i32Vector2 : Vector2Base<_VectorArithmeticBase<i32, 2>>
    {
        using Vector2Base::Vector2Base;
    };

    struct i32Vector3 : Vector3Base<_VectorArithmeticBase<i32, 3>>
    {
        using Vector3Base::Vector3Base;
    };

    struct i32Vector4 : Vector4Base<_VectorArithmeticBase<i32, 4>>
    {
        using Vector4Base::Vector4Base;
    };

    struct ui32Vector2 : Vector2Base<_VectorArithmeticBase<ui32, 2>>
    {
        using Vector2Base::Vector2Base;
    };

    struct ui32Vector3 : Vector3Base<_VectorArithmeticBase<ui32, 3>>
    {
        using Vector3Base::Vector3Base;
    };

    struct ui32Vector4 : Vector4Base<_VectorArithmeticBase<ui32, 4>>
    {
        using Vector4Base::Vector4Base;
    };

	struct boolVector2 : Vector2Base<_VectorBase<bool, 2>>
	{
		using Vector2Base::Vector2Base;
	};

	struct boolVector3 : Vector3Base<_VectorBase<bool, 3>>
	{
		using Vector3Base::Vector3Base;
	};

	struct boolVector4 : Vector4Base<_VectorBase<bool, 4>>
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

		std::array<std::array<f32, Columns>, Rows> elements{};

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

        [[nodiscard]] std::array<f32, Rows * Columns> &Data();
        [[nodiscard]] const std::array<f32, Rows * Columns> &Data() const;

        [[nodiscard]] MatrixTypeByDimensions<Columns, Rows> GetTransposed() const;

        [[nodiscard]] VectorTypeByDimension<f32, Columns> GetRow(uiw rowIndex) const;
        MatrixType &SetRow(uiw rowIndex, const VectorTypeByDimension<f32, Columns> &row);

        [[nodiscard]] VectorTypeByDimension<f32, Rows> GetColumn(uiw columnIndex) const;
        MatrixType &SetColumn(uiw columnIndex, const VectorTypeByDimension<f32, Rows> &column);

        [[nodiscard]] f32 FetchValueBoundless(uiw rowIndex, uiw columnIndex) const;
        MatrixType &StoreValueBoundless(uiw rowIndex, uiw columnIndex, f32 value);

        [[nodiscard]] bool EqualsWithEpsilon(const _Matrix &other, f32 epsilon = DefaultF32Epsilon) const;

        template <typename OtherMatrix> OtherMatrix As() const;

        MatrixType &Inverse(); // assertion fails if failed to inverse, the matrix isn't modified

		[[nodiscard]] uiw Hash() const;

    protected:
        constexpr _Matrix(); // will create identity matrix
        template <typename... Args> constexpr _Matrix(Args &&... args);
    };

    enum class ProjectionTarget
    {
        D3DAndMetal,
        Vulkan,
        OGL
    };

    struct Matrix4x3 : _Matrix<4, 3>
    {
        constexpr Matrix4x3() = default;

        constexpr Matrix4x3(f32 e00, f32 e01, f32 e02,
            f32 e10, f32 e11, f32 e12,
            f32 e20, f32 e21, f32 e22,
            f32 e30, f32 e31, f32 e32);

        constexpr Matrix4x3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2, const Vector3 &row3);

        [[nodiscard]] std::optional<Matrix4x3> GetInversed() const;
		[[nodiscard]] f32 Determinant() const;
		void Decompose(Matrix3x3 *rotation, Vector3 *translation, Vector3 *scale) const;
		void Decompose(Vector3 *rotation, Vector3 *translation, Vector3 *scale) const;
		void Decompose(Quaternion *rotation, Vector3 *translation, Vector3 *scale) const;

        [[nodiscard]] Matrix4x4 operator * (const Matrix4x4 &other) const;

        [[nodiscard]] static Matrix4x3 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix4x3 CreateRTS(const std::optional<Vector3> &rotation, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale = std::nullopt);
        [[nodiscard]] static Matrix4x3 CreateRTS(const std::optional<Quaternion> &rotation, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale = std::nullopt);
        [[nodiscard]] static Matrix4x3 CreateOrthographicProjection(const Vector3 &min, const Vector3 &max, ProjectionTarget target);
    };

    struct Matrix3x4 : _Matrix<3, 4>
    {
        constexpr Matrix3x4() = default;

        constexpr Matrix3x4(f32 e00, f32 e01, f32 e02, f32 e03,
            f32 e10, f32 e11, f32 e12, f32 e13,
            f32 e20, f32 e21, f32 e22, f32 e23);

        constexpr Matrix3x4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2);

		[[nodiscard]] std::optional<Matrix3x4> GetInversed() const;
		[[nodiscard]] f32 Determinant() const;

        [[nodiscard]] static Matrix3x4 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix3x4 CreateRS(const std::optional<Vector3> &rotation, const std::optional<Vector3> &scale = std::nullopt);
        [[nodiscard]] static Matrix3x4 CreateRS(const std::optional<Quaternion> &rotation, const std::optional<Vector3> &scale = std::nullopt);
    };

    struct Matrix4x4 : _Matrix<4, 4>
    {
        constexpr Matrix4x4() = default;

        constexpr Matrix4x4(f32 e00, f32 e01, f32 e02, f32 e03,
            f32 e10, f32 e11, f32 e12, f32 e13,
            f32 e20, f32 e21, f32 e22, f32 e23,
            f32 e30, f32 e31, f32 e32, f32 e33);

        constexpr Matrix4x4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2, const Vector4 &row3);

        Matrix4x4 &Transpose();

        std::optional<Matrix4x4> GetInversed() const;
		[[nodiscard]] f32 Determinant() const;
		void Decompose(Matrix3x3 *rotation, Vector3 *translation, Vector3 *scale) const;
		void Decompose(Vector3 *rotation, Vector3 *translation, Vector3 *scale) const;
		void Decompose(Quaternion *rotation, Vector3 *translation, Vector3 *scale) const;

        [[nodiscard]] static Matrix4x4 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix4x4 CreateRTS(const std::optional<Vector3> &rotation, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale = std::nullopt);
        [[nodiscard]] static Matrix4x4 CreateRTS(const std::optional<Quaternion> &rotation, const std::optional<Vector3> &translation, const std::optional<Vector3> &scale = std::nullopt);
        [[nodiscard]] static Matrix4x4 CreatePerspectiveProjection(f32 horizontalFOV, f32 aspectRatio, f32 nearPlane, f32 farPlane, ProjectionTarget target);
        [[nodiscard]] static Matrix4x4 CreateOrthographicProjection(const Vector3 &min, const Vector3 &max, ProjectionTarget target);
    };

    struct Matrix3x2 : _Matrix<3, 2>
    {
        constexpr Matrix3x2() = default;

        constexpr Matrix3x2(f32 e00, f32 e01,
            f32 e10, f32 e11,
            f32 e20, f32 e21);

        constexpr Matrix3x2(const Vector2 &row0, const Vector2 &row1, const Vector2 &row2);

        [[nodiscard]] std::optional<Matrix3x2> GetInversed() const;
		[[nodiscard]] f32 Determinant() const;
		void Decompose(f32 *rotation, Vector2 *translation, Vector2 *scale) const;

        [[nodiscard]] static Matrix3x2 CreateRTS(const std::optional<f32> &rotation, const std::optional<Vector2> &translation, const std::optional<Vector2> &scale = std::nullopt); // clockwise rotation around Z axis
    };

    struct Matrix2x3 : _Matrix<2, 3>
    {
        constexpr Matrix2x3() = default;

        constexpr Matrix2x3(f32 e00, f32 e01, f32 e02,
            f32 e10, f32 e11, f32 e12);

        constexpr Matrix2x3(const Vector3 &row0, const Vector3 &row1);

		[[nodiscard]] std::optional<Matrix2x3> GetInversed() const;
		[[nodiscard]] f32 Determinant() const;
	};

    struct Matrix2x2 : _Matrix<2, 2>
    {
        constexpr Matrix2x2() = default;

        constexpr Matrix2x2(f32 e00, f32 e01,
            f32 e10, f32 e11);

        constexpr Matrix2x2(const Vector2 &row0, const Vector2 &row1);

        Matrix2x2 &Transpose();
		void Decompose(f32 *rotation, Vector2 *scale) const;

        std::optional<Matrix2x2> GetInversed() const;
		[[nodiscard]] f32 Determinant() const;
		
		[[nodiscard]] static Matrix2x2 CreateRS(const std::optional<f32> &rotation, const std::optional<Vector2> &scale = std::nullopt); // clockwise rotation around Z axis
	};

    struct Matrix3x3 : _Matrix<3, 3>
    {
        constexpr Matrix3x3() = default;

        constexpr Matrix3x3(f32 e00, f32 e01, f32 e02,
            f32 e10, f32 e11, f32 e12,
            f32 e20, f32 e21, f32 e22);

        constexpr Matrix3x3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2);

        Matrix3x3 &Transpose();

		std::optional<Matrix3x3> GetInversed() const;
		[[nodiscard]] f32 Determinant() const;
		void Decompose(Matrix3x3 *rotation, Vector3 *scale) const; // decomposes as 3D
		void Decompose(Vector3 *rotation, Vector3 *scale) const; // decomposes as 3D
		void Decompose(Quaternion *rotation, Vector3 *scale) const; // decomposes as 3D
		void Decompose(f32 *rotation, Vector2 *translation, Vector2 *scale) const; // decomposes as 2D

        [[nodiscard]] static Matrix3x3 CreateRotationAroundAxis(const Vector3 &axis, f32 angle);
        [[nodiscard]] static Matrix3x3 CreateRS(const std::optional<Vector3> &rotation, const std::optional<Vector3> &scale = std::nullopt);
        [[nodiscard]] static Matrix3x3 CreateRS(const std::optional<Quaternion> &rotation, const std::optional<Vector3> &scale = std::nullopt);
		[[nodiscard]] static Matrix3x3 CreateRTS(const std::optional<f32> &rotation, const std::optional<Vector2> &translation, const std::optional<Vector2> &scale = std::nullopt); // clockwise rotation around Z axis
	};

    // Addition and subtraction are a component-wise operation; composing quaternions should be done via multiplication.
    // Order matters when composing quaternions : C = A * B will yield a quaternion C that logically
    // first applies A then B to any subsequent transformation(left first, then right).
    struct Quaternion
    {
		static constexpr uiw dim = 4;
		using ScalarType = f32;

        f32 x = 0, y = 0, z = 0, w = 1;

        static Quaternion FromEuler(const Vector3 &source);
		static Quaternion FromMatrix(const Matrix3x3 &matrix);
		static Quaternion FromAxis(const Vector3 &axis, f32 angleRad); // axis must be normalized

        constexpr Quaternion() = default;
        Quaternion(f32 x, f32 y, f32 z, f32 w);

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

        [[nodiscard]] std::array<f32, 4> &Data();
        [[nodiscard]] const std::array<f32, 4> &Data() const;

        [[nodiscard]] Vector3 RotateVector(const Vector3 &source) const;

        Quaternion &Normalize();
        [[nodiscard]] Quaternion GetNormalized() const;
		[[nodiscard]] bool IsNormalized() const;

		[[nodiscard]] f32 Length() const;
		[[nodiscard]] f32 LengthSquare() const;

        Quaternion &Inverse();
        [[nodiscard]] Quaternion GetInversed() const;

        [[nodiscard]] Vector3 ToEuler() const; // Must quaternion be normalized?
        [[nodiscard]] std::tuple<Vector3, f32> ToAxisAndAngle() const; // Quaternion must be normalized
        [[nodiscard]] Matrix3x3 ToMatrix() const; // Must quaternion be normalized?

        [[nodiscard]] bool EqualsWithEpsilon(const Quaternion &other, f32 epsilon = DefaultF32Epsilon) const;

		[[nodiscard]] uiw Hash() const;
    };

    template <typename T, bool IsTopLessThanBottom = true> struct Rectangle
    {
		static constexpr T undefinedValue = std::is_signed_v<T> ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max();
		using type = T;
		static constexpr bool isTopLessThanBottom = IsTopLessThanBottom;

		T left = undefinedValue;
		T right = undefinedValue;
		T top = undefinedValue;
		T bottom = undefinedValue;

        static constexpr Rectangle FromPoint(T x, T y);

        [[nodiscard]] bool IsIntersected(const Rectangle &other) const; // undefined rectangles are considered instersected
		[[nodiscard]] bool Contains(T x, T y) const;
        //[[nodiscard]] T SquareDistance(T x, T y) const;

		// expands the rectangle to include the specified rectangle or point
        Rectangle &Expand(const Rectangle &other);
        Rectangle &Expand(T x, T y);
        [[nodiscard]] Rectangle GetExpanded(const Rectangle &other) const;
        [[nodiscard]] Rectangle GetExpanded(T x, T y) const;

        [[nodiscard]] bool IsDefined() const;

        [[nodiscard]] T Width() const;
        [[nodiscard]] T Height() const;

        [[nodiscard]] bool operator == (const Rectangle &other) const;
        [[nodiscard]] bool operator != (const Rectangle &other) const;

		[[nodiscard]] uiw Hash() const;
    };

    using RectangleF32 = Rectangle<f32>;
    using RectangleI32 = Rectangle<i32>;
    using RectangleUI32 = Rectangle<ui32>;
}

#include "_MathValidation.hpp"

namespace StdLib
{
    /////////////
    // _Vector //
    /////////////

    template <typename Basis>
    template <uiw Rows, uiw Columns>
    inline auto _VectorFP<Basis>::operator * (const _Matrix<Rows, Columns> &matrix) const -> VectorTypeByDimension<f32, Columns>
    {
        static_assert(dim == Rows, "Cannot multiply vector and matrix, matrix's rows number and vector dimension must be equal");

        VectorTypeByDimension<f32, Columns> result;

        for (uiw columnIndex = 0; columnIndex < Columns; ++columnIndex)
        {
            result[columnIndex] = Dot(matrix.GetColumn(columnIndex));
        }

        _ValidateValues(*this, matrix, result);

        return result;
    }

    template <typename Basis>
    template <uiw Rows, uiw Columns>
    inline auto _VectorFP<Basis>::operator *= (const _Matrix<Rows, Columns> &matrix) -> VectorType &
    {
        *this = *this * matrix;
        _ValidateValues(*this, matrix);
		return *static_cast<VectorType*>(this);
    }

    // constepxr is implicitly inline, so these definitions must be in the header

    /////////////////
    // _VectorBase //
    /////////////////

	template <typename _ScalarType, uiw Dim>
	template <typename... Args>
	inline constexpr _VectorBase<_ScalarType, Dim>::_VectorBase(Args... args)
	{
		uiw index = 0;
		auto writeValue = [&index, this](_ScalarType value) mutable constexpr
		{
			if (index == 0) x = value;
			else if (index == 1) y = value;

			if constexpr (Dim > 2)
			{
				if (index == 2) this->z = value;

				if constexpr (Dim > 3)
				{
					if (index == 3) this->w = value;
				}
			}

			++index;
		};
		(writeValue(args), ...);
		_ValidateValues(*this);
	}

	///////////////////////////
	// _VectorArithmeticBase //
	///////////////////////////

    template <typename _ScalarType, uiw Dim>
    auto _VectorArithmeticBase<_ScalarType, Dim>::ForEach(_ScalarType func(_ScalarType)) -> VectorType &
    {
        _ValidateValues(*this);
        for (auto &e : Data())
        {
            e = func(e);
        }
        _ValidateValues(*this);
        return *static_cast<VectorType *>(this);
    }

    template <typename _ScalarType, uiw Dim>
    template <typename ReturnType>
    auto _VectorArithmeticBase<_ScalarType, Dim>::ForEach(ReturnType func(_ScalarType &)) -> VectorType &
    {
        _ValidateValues(*this);
        for (auto &e : Data())
        {
            if constexpr (std::is_same_v<ReturnType, void>)
            {
                func(e);
            }
            else
            {
                e = func(e);
            }
        }
        _ValidateValues(*this);
		return *static_cast<VectorType*>(this);
    }

    template <typename _ScalarType, uiw Dim>
    template <typename Func>
    auto _VectorArithmeticBase<_ScalarType, Dim>::ForEach(Func func) -> VectorType &
    {
        _ValidateValues(*this);
        for (auto &e : Data())
        {
            if constexpr (std::is_same_v<std::invoke_result_t<Func, _ScalarType>, void>)
            {
                func(e);
            }
            else
            {
                e = func(e);
            }
        }
        _ValidateValues(*this);
		return *static_cast<VectorType*>(this);
    }

    template <typename _ScalarType, uiw Dim>
    template <typename ReturnType>
    auto _VectorArithmeticBase<_ScalarType, Dim>::ForEach(ReturnType func(_ScalarType)) const -> const VectorType &
    {
        _ValidateValues(*this);
        for (const auto &e : Data())
        {
            if constexpr (std::is_same_v<ReturnType, void>)
            {
                func(e);
            }
            else
            {
                e = func(e);
            }
        }
        _ValidateValues(*this);
		return *static_cast<VectorType*>(this);
    }

    template <typename _ScalarType, uiw Dim>
    template <typename Func>
    auto _VectorArithmeticBase<_ScalarType, Dim>::ForEach(Func func) const -> const VectorType &
    {
        _ValidateValues(*this);
        for (const auto &e : Data())
        {
            if constexpr (std::is_same_v<std::invoke_result_t<Func, _ScalarType>, void>)
            {
                func(e);
            }
            else
            {
                e = func(e);
            }
        }
        _ValidateValues(*this);
		return *static_cast<VectorType*>(this);
    }

    /////////////////
    // Vector2Base //
    /////////////////

    template <typename BaseType> inline constexpr Vector2Base<BaseType>::Vector2Base(ScalarType x, ScalarType y) : BaseType(x, y) {}

    /////////////////
    // Vector3Base //
    /////////////////

    template <typename BaseType> inline constexpr Vector3Base<BaseType>::Vector3Base(ScalarType x, ScalarType y, ScalarType z) : BaseType(x, y, z) {}

    template <typename BaseType> inline constexpr Vector3Base<BaseType>::Vector3Base(const VectorTypeByDimension<ScalarType, 2> &vec, ScalarType z) : Vector3Base(vec.x, vec.y, z) {}

    /////////////////
    // Vector4Base //
    /////////////////

    template <typename BaseType> inline constexpr Vector4Base<BaseType>::Vector4Base(ScalarType x, ScalarType y, ScalarType z, ScalarType w) : BaseType(x, y, z, w) {}

    template <typename BaseType> inline constexpr Vector4Base<BaseType>::Vector4Base(const VectorTypeByDimension<ScalarType, 2> &vec, ScalarType z, ScalarType w) : Vector4Base(vec.x, vec.y, z, w) {}
    template <typename BaseType> inline constexpr Vector4Base<BaseType>::Vector4Base(const VectorTypeByDimension<ScalarType, 3> &vec, ScalarType w) : Vector4Base(vec.x, vec.y, vec.z, w) {}
    template <typename BaseType> inline constexpr Vector4Base<BaseType>::Vector4Base(const VectorTypeByDimension<ScalarType, 2> &v0, const VectorTypeByDimension<ScalarType, 2> &v1) : Vector4Base(v0.x, v0.y, v1.x, v1.y) {}

    /////////////
    // _Matrix //
    /////////////

    template <uiw Rows, uiw Columns>
    template <typename OtherMatrix> 
    inline OtherMatrix _Matrix<Rows, Columns>::As() const
    {
        _ValidateValues(*this);
        OtherMatrix r;
        for (uiw row = 0; row < Rows; ++row)
        {
            for (uiw column = 0; column < Columns; ++column)
            {
                r.StoreValueBoundless(row, column, elements[row][column]);
            }
        }
        return r;
    }

    template <uiw Rows, uiw Columns> inline constexpr _Matrix<Rows, Columns>::_Matrix()
    {
        for (uiw row = 0; row < Rows; ++row)
        {
            for (uiw column = 0; column < Columns; ++column)
            {
                if (row == column)
                {
                    elements[row][column] = 1.0f;
                }
                else
                {
                    elements[row][column] = 0.0f;
                }
            }
        }
    }

    template <uiw Rows, uiw Columns>
    template <typename... Args>
    inline constexpr _Matrix<Rows, Columns>::_Matrix(Args &&... args) : elements{args...}
    {
        _ValidateValues(*this);
    }

    ///////////////
    // Matrix2x2 //
    ///////////////

    inline constexpr Matrix2x2::Matrix2x2(f32 e00, f32 e01, f32 e10, f32 e11) : _Matrix(e00, e01, e10, e11) {}
    inline constexpr Matrix2x2::Matrix2x2(const Vector2 &row0, const Vector2 &row1) : _Matrix(row0.x, row0.y, row1.x, row1.y) {}

    ///////////////
    // Matrix3x2 //
    ///////////////

    inline constexpr Matrix3x2::Matrix3x2(f32 e00, f32 e01, f32 e10, f32 e11, f32 e20, f32 e21) : _Matrix(e00, e01, e10, e11, e20, e21) {}
    inline constexpr Matrix3x2::Matrix3x2(const Vector2 &row0, const Vector2 &row1, const Vector2 &row2) : _Matrix(row0.x, row0.y, row1.x, row1.y, row2.x, row2.y) {}

    ///////////////
    // Matrix2x3 //
    ///////////////

    inline constexpr Matrix2x3::Matrix2x3(f32 e00, f32 e01, f32 e02, f32 e10, f32 e11, f32 e12) : _Matrix(e00, e01, e02, e10, e11, e12) {}
    inline constexpr Matrix2x3::Matrix2x3(const Vector3 &row0, const Vector3 &row1) : _Matrix(row0.x, row0.y, row0.z, row1.x, row1.y, row1.z) {}

    ///////////////
    // Matrix4x3 //
    ///////////////

    inline constexpr Matrix4x3::Matrix4x3(f32 e00, f32 e01, f32 e02, f32 e10, f32 e11, f32 e12, f32 e20, f32 e21, f32 e22, f32 e30, f32 e31, f32 e32) : _Matrix(e00, e01, e02, e10, e11, e12, e20, e21, e22, e30, e31, e32) {}
    inline constexpr Matrix4x3::Matrix4x3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2, const Vector3 &row3) : _Matrix(row0.x, row0.y, row0.z, row1.x, row1.y, row1.z, row2.x, row2.y, row2.z, row3.x, row3.y, row3.z) {}

    ///////////////
    // Matrix3x4 //
    ///////////////

    inline constexpr Matrix3x4::Matrix3x4(f32 e00, f32 e01, f32 e02, f32 e03, f32 e10, f32 e11, f32 e12, f32 e13, f32 e20, f32 e21, f32 e22, f32 e23) : _Matrix(e00, e01, e02, e03, e10, e11, e12, e13, e20, e21, e22, e23) {}
    inline constexpr Matrix3x4::Matrix3x4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2) : _Matrix(row0.x, row0.y, row0.z, row0.w, row1.x, row1.y, row1.z, row1.w, row2.x, row2.y, row2.z, row2.w) {}

    ///////////////
    // Matrix4x4 //
    ///////////////

    inline constexpr Matrix4x4::Matrix4x4(f32 e00, f32 e01, f32 e02, f32 e03, f32 e10, f32 e11, f32 e12, f32 e13, f32 e20, f32 e21, f32 e22, f32 e23, f32 e30, f32 e31, f32 e32, f32 e33) : _Matrix(e00, e01, e02, e03, e10, e11, e12, e13, e20, e21, e22, e23, e30, e31, e32, e33) {}
    inline constexpr Matrix4x4::Matrix4x4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2, const Vector4 &row3) : _Matrix(row0.x, row0.y, row0.z, row0.w, row1.x, row1.y, row1.z, row1.w, row2.x, row2.y, row2.z, row2.w, row3.x, row3.y, row3.z, row3.w) {}

    ///////////////
    // Matrix3x3 //
    ///////////////

    inline constexpr Matrix3x3::Matrix3x3(f32 e00, f32 e01, f32 e02, f32 e10, f32 e11, f32 e12, f32 e20, f32 e21, f32 e22) : _Matrix(e00, e01, e02, e10, e11, e12, e20, e21, e22) {}
    inline constexpr Matrix3x3::Matrix3x3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2) : _Matrix(row0.x, row0.y, row0.z, row1.x, row1.y, row1.z, row2.x, row2.y, row2.z) {}

    ///////////////
    // Rectangle //
    ///////////////

    template <typename T, bool isTopLessThanBottom> inline constexpr auto Rectangle<T, isTopLessThanBottom>::FromPoint(T x, T y) -> Rectangle
    {
        return {x, x, y, y};
    }
}

namespace std
{
	template <typename T> struct _HashBase
	{
		[[nodiscard]] size_t operator()(const T &value) const
		{
			return value.Hash();
		}
	};

	template <> struct hash<StdLib::Vector2> : _HashBase<StdLib::Vector2> {};
	template <> struct hash<StdLib::Vector3> : _HashBase<StdLib::Vector3> {};
	template <> struct hash<StdLib::Vector4> : _HashBase<StdLib::Vector4> {};
	template <> struct hash<StdLib::i32Vector2> : _HashBase<StdLib::i32Vector2> {};
	template <> struct hash<StdLib::i32Vector3> : _HashBase<StdLib::i32Vector3> {};
	template <> struct hash<StdLib::i32Vector4> : _HashBase<StdLib::i32Vector4> {};
	template <> struct hash<StdLib::ui32Vector2> : _HashBase<StdLib::ui32Vector2> {};
	template <> struct hash<StdLib::ui32Vector3> : _HashBase<StdLib::ui32Vector3> {};
	template <> struct hash<StdLib::ui32Vector4> : _HashBase<StdLib::ui32Vector4> {};
	template <> struct hash<StdLib::boolVector2> : _HashBase<StdLib::boolVector2> {};
	template <> struct hash<StdLib::boolVector3> : _HashBase<StdLib::boolVector3> {};
	template <> struct hash<StdLib::boolVector4> : _HashBase<StdLib::boolVector4> {};
	template <> struct hash<StdLib::Matrix2x2> : _HashBase<StdLib::Matrix2x2> {};
	template <> struct hash<StdLib::Matrix3x2> : _HashBase<StdLib::Matrix3x2> {};
	template <> struct hash<StdLib::Matrix2x3> : _HashBase<StdLib::Matrix2x3> {};
	template <> struct hash<StdLib::Matrix3x3> : _HashBase<StdLib::Matrix3x3> {};
	template <> struct hash<StdLib::Matrix4x3> : _HashBase<StdLib::Matrix4x3> {};
	template <> struct hash<StdLib::Matrix3x4> : _HashBase<StdLib::Matrix3x4> {};
	template <> struct hash<StdLib::Matrix4x4> : _HashBase<StdLib::Matrix4x4> {};
	template <> struct hash<StdLib::Quaternion> : _HashBase<StdLib::Quaternion> {};
	template <> struct hash<StdLib::RectangleF32> : _HashBase<StdLib::RectangleF32> {};
	template <> struct hash<StdLib::RectangleI32> : _HashBase<StdLib::RectangleI32> {};
	template <> struct hash<StdLib::RectangleUI32> : _HashBase<StdLib::RectangleUI32> {};
}