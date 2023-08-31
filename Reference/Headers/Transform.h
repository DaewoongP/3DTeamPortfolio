#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	explicit CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_float3 Get_Scale() const { return _float3(Get_Right().Length(), Get_Up().Length(), Get_Look().Length()); }
	_float3 Get_Right() const { return m_WorldMatrix.Right(); }
	_float3 Get_Up() const { return m_WorldMatrix.Up(); }
	_float3 Get_Look() const { return m_WorldMatrix.Look(); }
	_float3 Get_Translation() const { return m_WorldMatrix.Translation(); }
	_float4x4 Get_WorldMatrix() const { return m_WorldMatrix; }
	const _float4x4* Get_WorldMatrixPtr() const { return &m_WorldMatrix; }
	_float4x4 Get_WorldMatrix_Inverse() const { return m_WorldMatrix.Inverse(); }

	void Set_Scale(_float3 _vScale);
	void Set_Right(_float3 _vRight);
	void Set_Up(_float3 _vUp);
	void Set_Look(_float3 _vLook);
	void Set_Position(_float3 _vPosition);
	void Set_WorldMatrix(_float4x4 _WorldMatrix);

	void Set_Speed(_float _fSpeed) { m_fSpeed = _fSpeed; }
	void Set_RotationSpeed(_float _fRotationSpeed) { m_fRotationSpeed = _fRotationSpeed; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	void Move_Direction(_float3 vDirection, _float fTimeDelta);
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Turn(_float3 vAxis, _float fTimeDelta);

private: // 공용적인 변수값
	_float4x4		m_WorldMatrix;
	_bool			m_isRigidBody = { false };
	_float3			m_vGravity = _float3(0.f, -5.f, 0.f);
	// 속도
	_float3			m_vVelocity;
	// 힘
	_float3			m_vForce;
	// 가속도
	_float3			m_vAccel;

private: // 객체 변수값.
	// 질량
	_float			m_fMass = { 0.f };
	// 저항
	_float			m_fResistance = { 0.f };
	_float			m_fSpeed = { 0.f };
	_float			m_fRotationSpeed = { 0.f };

public:
	static CTransform* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

/*_float3 vPos1, vPos2;
	_float3 vResult = vPos1 + vPos2;
	_float3 vRe = vPos1 * vPos2;

	_float fDot = vPos1.Dot(vPos2);
	_float3 vNorm;
	vPos1.Normalize(vNorm);

	_float4 vTrans = vPos1.TransCoord();
	_float4 vTran = vPos1.TransNorm();
	_float3 vRes = vTrans.xyz();

	_float4 vRight = m_WorldMatrix.Right().TransNorm();*/