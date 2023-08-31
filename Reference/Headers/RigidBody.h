#pragma once
#include "Component.h"

BEGIN(Engine)
class CColliderCom;

class ENGINE_DLL CRigidBody final : public CComponent
{
private:
	explicit CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
	PxRigidBody* Get_RigidBodyActor() const;
	_float3 Get_Position() const;
	_float4 Get_Rotation() const;
	void Set_Kinematic(_bool _isKinematic);
	void Set_Density(_float _fDensity) const;
	void Set_CollisionGroup(COLLISIONDESC::COLTYPE eColType);
	void Set_CollisionIgnoreGroups(COLLISIONDESC::COLTYPE eIgnoreColType);
	_bool Is_Static()  const { return m_isStatic; }
	_bool Is_Dynamic()  const { return !m_isStatic; }
	_bool Is_Kinematic() const { return m_isKinematic; }

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	HRESULT Create_Actor();
	void Add_Collider(CColliderCom* collider);

	void Put_To_Sleep() const;
	void Add_Force(const PxVec3 & _vForce, PxForceMode::Enum _eMode = PxForceMode::eFORCE, _bool _bAutowake = true) const;
	void Add_Torque(const PxVec3 & _vTorque, PxForceMode::Enum _eMode = PxForceMode::eFORCE, _bool _bAutowake = true) const;
	void Clear_Force(PxForceMode::Enum _eMode = PxForceMode::eFORCE) const;
	void Clear_Torque(PxForceMode::Enum _eMode = PxForceMode::eFORCE) const;

	//void SetConstraint(RigidBodyConstraintFlag flag, bool enable);
	void Translate(_float3 _vPosition) const;
	void Rotate(_float4 _vRotation) const;

private:
	PxRigidActor*			m_pActor = { nullptr };
	PxD6Joint*				m_pConstraintJoint = { nullptr };
	PxFilterData			m_CollisionGroups;
	_uint					m_InitialConstraints = { 0 };

private:
	vector<CColliderCom*>	m_Colliders;

private:
	_bool					m_isStatic = { false };
	_bool					m_isKinematic = { false };

public:
	static CRigidBody* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END