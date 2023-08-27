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
	_float3 Get_Scale() const;
	_float3 Get_Right() const;
	_float3 Get_Up() const;
	_float3 Get_Look() const;
	_float3 Get_Translation() const;
	_float4x4 Get_WorldMatrix() const;
	const _float4x4* Get_WorldMatrixPtr() const;
	_float4x4 Get_WorldMatrix_Inverse() const;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	void Move_Direction(_float3 vDirection, _float fTimeDelta, _float fSpeed = 1.f);

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

public:
	static CTransform* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END