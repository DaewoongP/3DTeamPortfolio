#pragma once
#include "Component.h"

BEGIN(Engine)
class CColliderCom;
class CCharacterController;

class ENGINE_DLL CRigidBody final : public CComponent
{
public:
	enum RigidBodyConstraint
	{
		None = 0,
		RotX = 1 << 0,
		RotY = 1 << 1,
		RotZ = 1 << 2,
		TransX = 1 << 3,
		TransY = 1 << 4,
		TransZ = 1 << 5,

		AllRot = RotX | RotY | RotZ,
		AllTrans = TransX | TransY | TransZ,
		All = AllRot | AllTrans
	};
private:
	explicit CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
	PxRigidBody* Get_RigidBodyActor() const;
	_float3 Get_Position() const;
	_float4 Get_Rotation() const;
	void Set_Constraint(RigidBodyConstraint eConstraintFlag, _bool _isEnable);
	void Set_Kinematic(_bool _isKinematic);
	void Set_Density(_float _fDensity) const;
	void Set_CollisionGroup(COLLISIONDESC::COLTYPE eColType);
	void Set_CollisionIgnoreGroups(COLLISIONDESC::COLTYPE eIgnoreColType);
	void Set_TransformComponent(CTransform* pTransform) {
		m_pTransform = pTransform;
		Safe_AddRef(m_pTransform);
	}
	void Set_ControllerComponent(CCharacterController* pController) {
		m_pController = pController;
		Safe_AddRef(m_pController);
	}

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
	void Add_Force(const _float3 & _vForce, PxForceMode::Enum _eMode = PxForceMode::eFORCE, _bool _bAutowake = true) const;
	void Add_Torque(const _float3& _vTorque, PxForceMode::Enum _eMode = PxForceMode::eFORCE, _bool _bAutowake = true) const;
	void Clear_Force(PxForceMode::Enum _eMode = PxForceMode::eFORCE) const;
	void Clear_Torque(PxForceMode::Enum _eMode = PxForceMode::eFORCE) const;

	void Translate(_float3 _vPosition) const;
	void Rotate(_float4 _vRotation) const;

private:
	PxRigidDynamic*			m_pActor = { nullptr };
	PxD6Joint*				m_pConstraintJoint = { nullptr };
	PxFilterData			m_CollisionGroups;
	_uint					m_InitialConstraints = { 0 };

private:
	vector<CColliderCom*>	m_Colliders;

private:
	CTransform*				m_pTransform = { nullptr };
	CCharacterController*	m_pController = { nullptr };
	_bool					m_isStatic = { false };
	_bool					m_isKinematic = { false };

public:
	static CRigidBody* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END