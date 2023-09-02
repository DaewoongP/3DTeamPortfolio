#pragma once
/* =============================================== */
// 
//	정 : 박대웅
//	부 :
//
/* =============================================== */

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
	_float3 Get_Position() const { return m_WorldMatrix.Translation(); }
	_float4x4 Get_WorldMatrix() const { return m_WorldMatrix; }
	const _float4x4* Get_WorldMatrixPtr() const { return &m_WorldMatrix; }
	_float4x4 Get_WorldMatrix_Inverse() const { return m_WorldMatrix.Inverse(); }
	_float Get_Speed() const { return m_fSpeed; }
	// 원하는 축을 기준으로 회전시킨 쿼터니언 벡터를 반환합니다.
	_float4 Get_QuaternionVector_From_Axis(_float3 vAxis, _float _fRadian);
	// Roll : X축기준 회전 
	// Pitch : Y축기준 회전
	// Yaw : Z축 기준 회전
	// 값은 회전할 값을 넣어주시면 됩니다!! 
	// ex) vRotation.x = XMConvertToRadian(90.f);
	_float4 Get_QuaternionVector_RollPitchYaw(_float3 vRotation);
	// X 축 기준 회전
	_float4 Get_QuaternionVector_Roll(_float fRadian);
	// Y 축 기준 회전
	_float4 Get_QuaternionVector_Pitch(_float fRadian);
	// Z 축 기준 회전
	_float4 Get_QuaternionVector_Yaw(_float fRadian);

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
	void LookAt(_float3 _vTarget, _bool _isDeleteY = false);

private:
	_float4x4		m_WorldMatrix;
	_float			m_fSpeed = { 0.f };
	_float			m_fRotationSpeed = { 0.f };

public:
	static CTransform* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END