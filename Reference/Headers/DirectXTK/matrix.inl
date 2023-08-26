inline _Matrix::_Matrix(const XMFLOAT3X3& M) noexcept
{
    _11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
    _21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
    _31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
    _41 = 0.f;   _42 = 0.f;   _43 = 0.f;   _44 = 1.f;
}

inline _Matrix::_Matrix(const XMFLOAT4X3& M) noexcept
{
    _11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
    _21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
    _31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
    _41 = M._41; _42 = M._42; _43 = M._43; _44 = 1.f;
}

inline _Matrix& _Matrix::operator= (const XMFLOAT3X3& M) noexcept
{
    _11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
    _21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
    _31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
    _41 = 0.f;   _42 = 0.f;   _43 = 0.f;   _44 = 1.f;
    return *this;
}

inline _Matrix& _Matrix::operator= (const XMFLOAT4X3& M) noexcept
{
    _11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
    _21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
    _31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
    _41 = M._41; _42 = M._42; _43 = M._43; _44 = 1.f;
    return *this;
}

inline _Matrix& _Matrix::operator+= (const _Matrix& M) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

    const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
    const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
    const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
    const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

    x1 = XMVectorAdd(x1, y1);
    x2 = XMVectorAdd(x2, y2);
    x3 = XMVectorAdd(x3, y3);
    x4 = XMVectorAdd(x4, y4);

    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
    return *this;
}

inline _Matrix& _Matrix::operator-= (const _Matrix& M) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

    const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
    const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
    const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
    const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

    x1 = XMVectorSubtract(x1, y1);
    x2 = XMVectorSubtract(x2, y2);
    x3 = XMVectorSubtract(x3, y3);
    x4 = XMVectorSubtract(x4, y4);

    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
    return *this;
}

inline _Matrix& _Matrix::operator*= (const _Matrix& M) noexcept
{
    using namespace DirectX;
    const XMMATRIX M1 = XMLoadFloat4x4(this);
    const XMMATRIX M2 = XMLoadFloat4x4(&M);
    const XMMATRIX X = XMMatrixMultiply(M1, M2);
    XMStoreFloat4x4(this, X);
    return *this;
}

inline _Matrix& _Matrix::operator*= (float S) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

    x1 = XMVectorScale(x1, S);
    x2 = XMVectorScale(x2, S);
    x3 = XMVectorScale(x3, S);
    x4 = XMVectorScale(x4, S);

    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
    return *this;
}

inline _Matrix& _Matrix::operator/= (float S) noexcept
{
    using namespace DirectX;
    assert(S != 0.f);
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

    const float rs = 1.f / S;

    x1 = XMVectorScale(x1, rs);
    x2 = XMVectorScale(x2, rs);
    x3 = XMVectorScale(x3, rs);
    x4 = XMVectorScale(x4, rs);

    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
    return *this;
}

inline _Matrix& _Matrix::operator/= (const _Matrix& M) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

    const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
    const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
    const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
    const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

    x1 = XMVectorDivide(x1, y1);
    x2 = XMVectorDivide(x2, y2);
    x3 = XMVectorDivide(x3, y3);
    x4 = XMVectorDivide(x4, y4);

    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
    return *this;
}

//------------------------------------------------------------------------------
// Urnary operators
//------------------------------------------------------------------------------

inline _Matrix _Matrix::operator- () const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
    XMVECTOR v2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
    XMVECTOR v3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
    XMVECTOR v4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

    v1 = XMVectorNegate(v1);
    v2 = XMVectorNegate(v2);
    v3 = XMVectorNegate(v3);
    v4 = XMVectorNegate(v4);

    _Matrix R;
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), v1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), v2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), v3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), v4);
    return R;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline _Matrix operator+ (const _Matrix& M1, const _Matrix& M2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

    const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
    const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
    const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
    const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

    x1 = XMVectorAdd(x1, y1);
    x2 = XMVectorAdd(x2, y2);
    x3 = XMVectorAdd(x3, y3);
    x4 = XMVectorAdd(x4, y4);

    _Matrix R;
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
    return R;
}

inline _Matrix operator- (const _Matrix& M1, const _Matrix& M2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

    const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
    const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
    const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
    const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

    x1 = XMVectorSubtract(x1, y1);
    x2 = XMVectorSubtract(x2, y2);
    x3 = XMVectorSubtract(x3, y3);
    x4 = XMVectorSubtract(x4, y4);

    _Matrix R;
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
    return R;
}

inline _Matrix operator* (const _Matrix& M1, const _Matrix& M2) noexcept
{
    using namespace DirectX;
    const XMMATRIX m1 = XMLoadFloat4x4(&M1);
    const XMMATRIX m2 = XMLoadFloat4x4(&M2);
    const XMMATRIX X = XMMatrixMultiply(m1, m2);

    _Matrix R;
    XMStoreFloat4x4(&R, X);
    return R;
}

inline _Matrix operator* (const _Matrix& M, float S) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

    x1 = XMVectorScale(x1, S);
    x2 = XMVectorScale(x2, S);
    x3 = XMVectorScale(x3, S);
    x4 = XMVectorScale(x4, S);

    _Matrix R;
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
    return R;
}

inline _Matrix operator/ (const _Matrix& M, float S) noexcept
{
    using namespace DirectX;
    assert(S != 0.f);

    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

    const float rs = 1.f / S;

    x1 = XMVectorScale(x1, rs);
    x2 = XMVectorScale(x2, rs);
    x3 = XMVectorScale(x3, rs);
    x4 = XMVectorScale(x4, rs);

    _Matrix R;
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
    return R;
}

inline _Matrix operator/ (const _Matrix& M1, const _Matrix& M2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

    const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
    const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
    const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
    const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

    x1 = XMVectorDivide(x1, y1);
    x2 = XMVectorDivide(x2, y2);
    x3 = XMVectorDivide(x3, y3);
    x4 = XMVectorDivide(x4, y4);

    _Matrix R;
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
    return R;
}

inline _Matrix operator* (float S, const _Matrix& M) noexcept
{
    using namespace DirectX;

    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

    x1 = XMVectorScale(x1, S);
    x2 = XMVectorScale(x2, S);
    x3 = XMVectorScale(x3, S);
    x4 = XMVectorScale(x4, S);

    _Matrix R;
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
    return R;
}

//------------------------------------------------------------------------------
// _Matrix operations
//------------------------------------------------------------------------------

inline bool _Matrix::Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation) noexcept
{
    using namespace DirectX;

    XMVECTOR s, r, t;

    if (!XMMatrixDecompose(&s, &r, &t, *this))
        return false;

    XMStoreFloat3(&scale, s);
    XMStoreFloat4(&rotation, r);
    XMStoreFloat3(&translation, t);

    return true;
}

inline _Matrix _Matrix::Transpose() const noexcept
{
    using namespace DirectX;
    const XMMATRIX M = XMLoadFloat4x4(this);
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixTranspose(M));
    return R;
}

inline void _Matrix::Transpose(_Matrix& result) const noexcept
{
    using namespace DirectX;
    const XMMATRIX M = XMLoadFloat4x4(this);
    XMStoreFloat4x4(&result, XMMatrixTranspose(M));
}

inline _Matrix _Matrix::Inverse() const noexcept
{
    using namespace DirectX;
    const XMMATRIX M = XMLoadFloat4x4(this);
    _Matrix R;
    XMVECTOR det;
    XMStoreFloat4x4(&R, XMMatrixInverse(&det, M));
    return R;
}

inline void _Matrix::Inverse(_Matrix& result) const noexcept
{
    using namespace DirectX;
    const XMMATRIX M = XMLoadFloat4x4(this);
    XMVECTOR det;
    XMStoreFloat4x4(&result, XMMatrixInverse(&det, M));
}

inline float _Matrix::Determinant() const noexcept
{
    using namespace DirectX;
    const XMMATRIX M = XMLoadFloat4x4(this);
    return XMVectorGetX(XMMatrixDeterminant(M));
}

inline Vector3 _Matrix::ToEuler() const noexcept
{
    const float cy = sqrtf(_33 * _33 + _31 * _31);
    const float cx = atan2f(_32, cy); // 부호를 제거
    if (cy > 16.f * FLT_EPSILON)
    {
        return Vector3(cx, atan2f(-_31, _33), atan2f(-_12, _22)); // Y와 Z축의 atan2 인자들의 부호 변경
    }
    else
    {
        return Vector3(cx, 0.f, atan2f(_21, _11)); // Z축의 atan2 인자들의 부호 변경
    }
}


//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

_Use_decl_annotations_
inline _Matrix _Matrix::MatrixBillboard(
    const Vector3& objectPosition,
    const Vector3& cameraPosition,
    const Vector3& cameraUp,
    const Vector3* cameraLook) noexcept
{
    using namespace DirectX;
    const XMVECTOR ObjPos = XMLoadFloat3(&objectPosition);
    const XMVECTOR CamPos = XMLoadFloat3(&cameraPosition);
    XMVECTOR vLook = XMVectorSubtract(ObjPos, CamPos);

    const XMVECTOR N = XMVector3LengthSq(vLook);
    if (XMVector3Less(N, g_XMEpsilon))
    {
        if (cameraLook)
        {
            const XMVECTOR vCamLook = XMLoadFloat3(cameraLook);
            vLook = XMVectorNegate(vCamLook);
        }
        else
            vLook = g_XMNegIdentityR2;
    }
    else
    {
        vLook = XMVector3Normalize(vLook);
    }

    const XMVECTOR vCamUp = XMLoadFloat3(&cameraUp);
    XMVECTOR vRight = XMVector3Cross(vCamUp, vLook);
    vRight = XMVector3Normalize(vRight);

    const XMVECTOR vUp = XMVector3Cross(vLook, vRight);

    XMMATRIX M;
    M.r[0] = vRight;
    M.r[1] = vUp;
    M.r[2] = vLook;
    M.r[3] = XMVectorSetW(ObjPos, 1.f);

    _Matrix ResultMatrix;
    XMStoreFloat4x4(&ResultMatrix, M);
    return ResultMatrix;
}

_Use_decl_annotations_

inline _Matrix _Matrix::MatrixTranslation(const Vector3& position) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixTranslation(position.x, position.y, position.z));
    return R;
}

inline _Matrix _Matrix::MatrixTranslation(float x, float y, float z) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixTranslation(x, y, z));
    return R;
}

inline _Matrix _Matrix::MatrixScale(const Vector3& scales) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixScaling(scales.x, scales.y, scales.z));
    return R;
}

inline _Matrix _Matrix::MatrixScale(float xs, float ys, float zs) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixScaling(xs, ys, zs));
    return R;
}

inline _Matrix _Matrix::MatrixScale(float scale) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixScaling(scale, scale, scale));
    return R;
}

inline _Matrix _Matrix::MatrixRotationX(float radians) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixRotationX(radians));
    return R;
}

inline _Matrix _Matrix::MatrixRotationY(float radians) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixRotationY(radians));
    return R;
}

inline _Matrix _Matrix::MatrixRotationZ(float radians) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixRotationZ(radians));
    return R;
}

inline _Matrix _Matrix::MatrixRotationAxis(const Vector3& axis, float angle) noexcept
{
    using namespace DirectX;
    _Matrix R;
    const XMVECTOR a = XMLoadFloat3(&axis);
    XMStoreFloat4x4(&R, XMMatrixRotationAxis(a, angle));
    return R;
}

inline _Matrix _Matrix::MatrixPerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane));
    return R;
}

inline _Matrix _Matrix::MatrixOrthographic(float width, float height, float zNearPlane, float zFarPlane) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixOrthographicLH(width, height, zNearPlane, zFarPlane));
    return R;
}

inline _Matrix _Matrix::MatrixLookAt(const Vector3& eye, const Vector3& target, const Vector3& up) noexcept
{
    using namespace DirectX;
    _Matrix R;
    const XMVECTOR eyev = XMLoadFloat3(&eye);
    const XMVECTOR targetv = XMLoadFloat3(&target);
    const XMVECTOR upv = XMLoadFloat3(&up);
    XMStoreFloat4x4(&R, XMMatrixLookAtLH(eyev, targetv, upv));
    return R;
}

inline _Matrix _Matrix::MatrixWorld(const Vector3& position, const Vector3& look, const Vector3& up) noexcept
{
    using namespace DirectX;
    const XMVECTOR zaxis = XMVector3Normalize((XMLoadFloat3(&look)));
    XMVECTOR yaxis = XMLoadFloat3(&up);
    const XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(yaxis, zaxis));
    yaxis = XMVector3Cross(zaxis, xaxis);

    _Matrix R;
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), xaxis);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), yaxis);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), zaxis);
    R._14 = R._24 = R._34 = 0.f;
    R._41 = position.x; R._42 = position.y; R._43 = position.z;
    R._44 = 1.f;
    return R;
}

inline _Matrix _Matrix::MatrixFromQuaternion(const Quaternion& rotation) noexcept
{
    using namespace DirectX;
    const XMVECTOR quatv = XMLoadFloat4(&rotation);
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixRotationQuaternion(quatv));
    return R;
}

inline _Matrix _Matrix::MatrixFromYawPitchRoll(float yaw, float pitch, float roll) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
    return R;
}

inline _Matrix _Matrix::MatrixFromYawPitchRoll(const Vector3& angles) noexcept
{
    using namespace DirectX;
    _Matrix R;
    XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYawFromVector(angles));
    return R;
}

inline void _Matrix::Lerp(const _Matrix& M1, const _Matrix& M2, float t, _Matrix& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

    const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
    const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
    const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
    const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

    x1 = XMVectorLerp(x1, y1, t);
    x2 = XMVectorLerp(x2, y2, t);
    x3 = XMVectorLerp(x3, y3, t);
    x4 = XMVectorLerp(x4, y4, t);

    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
}

inline _Matrix _Matrix::Lerp(const _Matrix& M1, const _Matrix& M2, float t) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
    XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
    XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
    XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

    const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
    const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
    const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
    const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

    x1 = XMVectorLerp(x1, y1, t);
    x2 = XMVectorLerp(x2, y2, t);
    x3 = XMVectorLerp(x3, y3, t);
    x4 = XMVectorLerp(x4, y4, t);

    _Matrix result;
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
    return result;
}

inline void _Matrix::Transform(const _Matrix& M, const Quaternion& rotation, _Matrix& result) noexcept
{
    using namespace DirectX;
    const XMVECTOR quatv = XMLoadFloat4(&rotation);

    const XMMATRIX M0 = XMLoadFloat4x4(&M);
    const XMMATRIX M1 = XMMatrixRotationQuaternion(quatv);

    XMStoreFloat4x4(&result, XMMatrixMultiply(M0, M1));
}

inline _Matrix _Matrix::Transform(const _Matrix& M, const Quaternion& rotation) noexcept
{
    using namespace DirectX;
    const XMVECTOR quatv = XMLoadFloat4(&rotation);

    const XMMATRIX M0 = XMLoadFloat4x4(&M);
    const XMMATRIX M1 = XMMatrixRotationQuaternion(quatv);

    _Matrix result;
    XMStoreFloat4x4(&result, XMMatrixMultiply(M0, M1));
    return result;
}