#pragma once
/* =============================================== */
// 
//	정 : 박대웅
//	부 :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)
class CNavigation;

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
	// 월드행렬에서 쿼터니언 벡터를 추출하는 함수입니다.
	_float4 Get_Quaternion();
	_float2 Get_Trnaslation_To_UI_fXY(_uint iWinSizeX, _uint iWinSizeY) const;
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
	// 쿼터니언 벡터 로테이션
	void Set_Quaternion(_float4 vQuaternion);
	void Set_WorldMatrix(_float4x4 _WorldMatrix);

	void Set_Speed(_float _fSpeed) { m_fSpeed = _fSpeed; }
	void Set_RotationSpeed(_float _fRotationSpeed) { m_fRotationSpeed = _fRotationSpeed; }

	void Add_Force(_float3 vDir, _float fJumpForce, _float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	void RigidBody(_float fTimeDelta);
	void Use_RigidBody(class CNavigation* pNavigation, _float fLimitXZVelocity = 1.f, _float fMass = 10.f, _float fResistance = 10.f);
	void Check_Move(_float3 vCurrentPosition, _float3 vVelocity);
	void Move_Direction(_float3 vMoveDir, _float fTimeDelta);
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Chase(_float3 vTargetPosition, _float fTimeDelta, _float fMinDistance = 0.1f);
	void Turn(_float3 vAxis, _float fTimeDelta); 
	// 회전속도 직접 설정하여 회전
	void Turn(_float3 vAxis, _float fRadian, _float fTimeDelta);
	// 각도값 고정
	// 축을 기반으로 한 로테이션
	void Rotation(_float3 vAxis, _float fRadian);
	// 각도값 고정
	// 각각의 X,Y,Z 의 각도 값을 받아 처리.
	void Rotation(_float3 vRadians);
	void LookAt(_float3 _vTarget, _bool _isDeleteY = false);

private:
	CNavigation*			m_pNavigation = { nullptr };
	_float4x4				m_WorldMatrix;
	_float					m_fSpeed = { 0.f };
	_float					m_fRotationSpeed = { 0.f };
	// 현재 셀 상태값
	CELLFLAG				m_eCurrentCellFlag;
	CELLFLAG				m_ePreviousCellFlag;

private:
	_bool					m_isRigidBody = { false };
	// 땅에 붙어있는지 아닌지 판단하는 bool변수
	_bool					m_isGrounded = { false };
	// 현재 셀의 Y값을 판단하는 값
	_float					m_fGroundY = { 0.f };
	_float3					m_vGravity = _float3(0.f, -9.81f, 0.f);
	// 속도
	_float3					m_vVelocity;
	// 힘
	_float3					m_vForce;
	// 가속도
	_float3					m_vAccel;
	// 최대속도 제한
	_float					m_fLimitXZVelocity = { 0.f };
	// 질량
	_float					m_fMass = { 0.f };
	// 저항
	_float					m_fResistance = { 0.f };
	// 앞 셀의 Y값보다 살짝 적어도 올라갈 수 있게 처리해주는 변수
	_float					m_fStepOffset = { 0.f };

private:
	_bool Check_CellY(_float3 vXZPosition, _float3 vCurrentPosition);
	void Update_CellFlag(_float3 vCurrentPosition);

public:
	static CTransform* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END