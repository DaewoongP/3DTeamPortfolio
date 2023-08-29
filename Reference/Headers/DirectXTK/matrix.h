#pragma once
#include <DirectXMath.h>
#include "DirectXTK/SimpleMath.h" 

using namespace DirectX::SimpleMath;

struct _Matrix : public XMFLOAT4X4
{
    _Matrix() noexcept
        : XMFLOAT4X4(1.f, 0, 0, 0,
            0, 1.f, 0, 0,
            0, 0, 1.f, 0,
            0, 0, 0, 1.f)
    {
    }
    constexpr _Matrix(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33) noexcept
        : XMFLOAT4X4(m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33)
    {
    }
    explicit _Matrix(const Vector3& r0, const Vector3& r1, const Vector3& r2) noexcept
        : XMFLOAT4X4(r0.x, r0.y, r0.z, 0,
            r1.x, r1.y, r1.z, 0,
            r2.x, r2.y, r2.z, 0,
            0, 0, 0, 1.f)
    {
    }
    explicit _Matrix(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3) noexcept
        : XMFLOAT4X4(r0.x, r0.y, r0.z, r0.w,
            r1.x, r1.y, r1.z, r1.w,
            r2.x, r2.y, r2.z, r2.w,
            r3.x, r3.y, r3.z, r3.w)
    {
    }
    _Matrix(const XMFLOAT4X4& M) noexcept { memcpy(this, &M, sizeof(XMFLOAT4X4)); }
    _Matrix(const XMFLOAT3X3& M) noexcept;
    _Matrix(const XMFLOAT4X3& M) noexcept;

    explicit _Matrix(_In_reads_(16) const float* pArray) noexcept : XMFLOAT4X4(pArray) {}
    _Matrix(CXMMATRIX M) noexcept { XMStoreFloat4x4(this, M); }

    _Matrix(const _Matrix&) = default;
    _Matrix& operator=(const _Matrix&) = default;

    _Matrix(_Matrix&&) = default;
    _Matrix& operator=(_Matrix&&) = default;

    operator XMMATRIX() const noexcept { return XMLoadFloat4x4(this); }

    // Comparison operators
    bool operator == (const _Matrix& M) const noexcept;
    bool operator != (const _Matrix& M) const noexcept;

    // Assignment operators
    _Matrix& operator= (const XMFLOAT3X3& M) noexcept;
    _Matrix& operator= (const XMFLOAT4X3& M) noexcept;
    _Matrix& operator+= (const _Matrix& M) noexcept;
    _Matrix& operator-= (const _Matrix& M) noexcept;
    _Matrix& operator*= (const _Matrix& M) noexcept;
    _Matrix& operator*= (float S) noexcept;
    _Matrix& operator/= (float S) noexcept;

    _Matrix& operator/= (const _Matrix& M) noexcept;
    // Element-wise divide

// Unary operators
    _Matrix operator+ () const noexcept { return *this; }
    _Matrix operator- () const noexcept;

    // Properties
    Vector3 Up() const noexcept { return Vector3(_21, _22, _23); }
    void Up(const Vector3& v) noexcept { _21 = v.x; _22 = v.y; _23 = v.z; }

    Vector3 Down() const  noexcept { return Vector3(-_21, -_22, -_23); }
    void Down(const Vector3& v) noexcept { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

    Vector3 Right() const noexcept { return Vector3(_11, _12, _13); }
    void Right(const Vector3& v) noexcept { _11 = v.x; _12 = v.y; _13 = v.z; }

    Vector3 Left() const noexcept { return Vector3(-_11, -_12, -_13); }
    void Left(const Vector3& v) noexcept { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

    Vector3 Look() const noexcept { return Vector3(_31, _32, _33); }
    void Look(const Vector3& v) noexcept { _31 = v.x; _32 = v.y; _33 = v.z; }

    Vector3 Back() const noexcept { return Vector3(-_31, -_32, -_33); }
    void Back(const Vector3& v) noexcept { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

    Vector3 Translation() const  noexcept { return Vector3(_41, _42, _43); }
    void Translation(const Vector3& v) noexcept { _41 = v.x; _42 = v.y; _43 = v.z; }

    // _Matrix operations
    bool Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation) noexcept;

    _Matrix Transpose() const noexcept;
    void Transpose(_Matrix& result) const noexcept;

    _Matrix Inverse() const noexcept;
    void Inverse(_Matrix& result) const noexcept;

    float Determinant() const noexcept;

    // Computes rotation about y-axis (y), then x-axis (x), then z-axis (z)
    Vector3 ToEuler() const noexcept;

    // Static functions
    static _Matrix MatrixBillboard(
        const Vector3& object, const Vector3& cameraPosition, const Vector3& cameraUp, _In_opt_ const Vector3* cameraLook = nullptr) noexcept;

    static _Matrix MatrixTranslation(const Vector3& position) noexcept;
    static _Matrix MatrixTranslation(float x, float y, float z) noexcept;

    static _Matrix MatrixScale(const Vector3& scales) noexcept;
    static _Matrix MatrixScale(float xs, float ys, float zs) noexcept;
    static _Matrix MatrixScale(float scale) noexcept;

    static _Matrix MatrixRotationX(float radians) noexcept;
    static _Matrix MatrixRotationY(float radians) noexcept;
    static _Matrix MatrixRotationZ(float radians) noexcept;

    static _Matrix MatrixRotationAxis(const Vector3& axis, float angle) noexcept;

    static _Matrix MatrixPerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane) noexcept;
    static _Matrix MatrixOrthographic(float width, float height, float zNearPlane, float zFarPlane) noexcept;

    static _Matrix MatrixLookAt(const Vector3& position, const Vector3& target, const Vector3& up) noexcept;
    static _Matrix MatrixWorld(const Vector3& position, const Vector3& look, const Vector3& up) noexcept;

    static _Matrix MatrixFromQuaternion(const Quaternion& quat) noexcept;

    // Rotates about y-axis (yaw), then x-axis (pitch), then z-axis (roll)
    static _Matrix MatrixFromYawPitchRoll(float yaw, float pitch, float roll) noexcept;

    // Rotates about y-axis (angles.y), then x-axis (angles.x), then z-axis (angles.z)
    static _Matrix MatrixFromYawPitchRoll(const Vector3& angles) noexcept;

    static void Lerp(const _Matrix& M1, const _Matrix& M2, float t, _Matrix& result) noexcept;
    static _Matrix Lerp(const _Matrix& M1, const _Matrix& M2, float t) noexcept;

    static void Transform(const _Matrix& M, const Quaternion& rotation, _Matrix& result) noexcept;
    static _Matrix Transform(const _Matrix& M, const Quaternion& rotation) noexcept;

    // Constants
    static const _Matrix Identity;
};

// Binary operators
_Matrix operator+ (const _Matrix& M1, const _Matrix& M2) noexcept;
_Matrix operator- (const _Matrix& M1, const _Matrix& M2) noexcept;
_Matrix operator* (const _Matrix& M1, const _Matrix& M2) noexcept;
_Matrix operator* (const _Matrix& M, float S) noexcept;
_Matrix operator/ (const _Matrix& M, float S) noexcept;
_Matrix operator/ (const _Matrix& M1, const _Matrix& M2) noexcept;
// Element-wise divide
_Matrix operator* (float S, const _Matrix& M) noexcept;

#include "matrix.inl"
