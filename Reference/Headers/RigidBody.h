#pragma once
/* =============================================== */
// 
//	정 : 박대웅
//	부 :
//
/* =============================================== */
#include "Composite.h"

BEGIN(Engine)

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
	void Set_Rotation(_float4 vRotation);
	void Set_Material(_float3 vMaterial);
	void Set_Constraint(RigidBodyConstraint eConstraintFlag, _bool _isEnable);
	void Set_Kinematic(_bool _isKinematic);
	void Set_Density(_float _fDensity) const;

#ifdef _DEBUG
	void Set_DebugColor(_float4 _vColor) { m_vColor = _vColor; }
#endif // _DEBUG


	_bool Is_Static()  const { return m_isStatic; }
	_bool Is_Dynamic()  const { return !m_isStatic; }
	_bool Is_Kinematic() const { return m_isKinematic; }

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_float fTimeDelta) override;

#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif // _DEBUG

public:
	HRESULT Create_Actor();
	// 액터 설정 세팅하려고 만들어 놨는데 지금일단 Create Actor 함수에서 전부 처리중입니다.
	HRESULT SetUp_Actor(_float3 _vInitPos, 	PxGeometry _ShapeType, _bool _isTrigger, 
		RigidBodyConstraint eConstraintFlag, _float3 _vResistance, PxFilterData FilterData);
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
	_bool					m_isStatic = { false };
	_bool					m_isKinematic = { false };

private:
	_float					m_fAirDrag = { 0.f };

#ifdef _DEBUG
private:
	CShader*				m_pShader = { nullptr };
	CVIBuffer_Line*			m_pLine = { nullptr };
	CVIBuffer_Triangle*		m_pTriangle = { nullptr };
	_float4					m_vColor;

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