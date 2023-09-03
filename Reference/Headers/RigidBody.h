#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
//
/* =============================================== */
#include "Composite.h"

BEGIN(Engine)
class CCharacterController;

#ifdef _DEBUG
class CShader;
class CVIBuffer_Line;
class CVIBuffer_Triangle;
#endif // _DEBUG

class ENGINE_DLL CRigidBody final : public CComposite
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
	const PxMaterial* Get_Material() const { return m_pMaterial; }
	PxRigidBody* Get_RigidBodyActor() const;
	_float3 Get_Position() const;
	_float4 Get_Rotation() const;
	void Set_Position(_float3 vPosition);
	void Set_Material(_float3 vMaterial);
	void Set_Constraint(RigidBodyConstraint eConstraintFlag, _bool _isEnable);
	void Set_Kinematic(_bool _isKinematic);
	void Set_Density(_float _fDensity) const;
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
	virtual void Late_Tick(_float fTimeDelta) override;

#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif // _DEBUG

public:
	HRESULT Create_Actor();
	//void Add_Collider(CColliderCom* collider);

	void Put_To_Sleep() const;
	void Add_Force(const _float3 & _vForce, PxForceMode::Enum _eMode = PxForceMode::eFORCE, _bool _bAutowake = true) const;
	void Add_Torque(const _float3& _vTorque, PxForceMode::Enum _eMode = PxForceMode::eFORCE, _bool _bAutowake = true) const;
	void Clear_Force(PxForceMode::Enum _eMode = PxForceMode::eFORCE) const;
	void Clear_Torque(PxForceMode::Enum _eMode = PxForceMode::eFORCE) const;

	void Translate(_float3 _vPosition) const;
	void Rotate(_float4 _vRotation) const;

private:
	PxRigidDynamic*			m_pActor = { nullptr };
	PxMaterial*				m_pMaterial = { nullptr };
	PxScene*				m_pScene = { nullptr };

private:
	CTransform*				m_pTransform = { nullptr };
	CCharacterController*	m_pController = { nullptr };
	_bool					m_isStatic = { false };
	_bool					m_isKinematic = { false };

#ifdef _DEBUG
private:
	CShader*				m_pShader = { nullptr };
	CVIBuffer_Line*			m_pLine = { nullptr };
	CVIBuffer_Triangle*		m_pTriangle = { nullptr };

private:
	_uint					m_iNumLineBuffer = { 0 };
	_uint					m_iStartLineBufferIndex = { 0 };

	_uint					m_iNumTriangleBuffer = { 0 };
	_uint					m_iStartTriangleBufferIndex = { 0 };
#endif // _DEBUG


#ifdef _DEBUG
private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Make_Buffers();
#endif // _DEBUG

public:
	static CRigidBody* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END