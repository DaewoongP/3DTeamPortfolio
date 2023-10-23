#pragma once
/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */
// https://github.com/DaewoongP/3DTeamPortfolio/wiki/Rigid-Body
#include "Composite.h"

BEGIN(Engine)

#ifdef _DEBUG
class CShader;
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

	typedef struct tagRigidBodyDesc
	{
		_char				szCollisionTag[MAX_PATH] = "";
		class CGameObject*	pOwnerObject = { nullptr };
		_bool				isStatic = { false };
		_bool				isTrigger = { false };
		_bool				isGravity = { true };
		_float3				vInitPosition = _float3(0.f, 0.f, 0.f);
		_float4				vInitRotation = _float4(0.f, 0.f, 0.f, 1.f);
		_float3				vOffsetPosition = _float3(0.f, 0.f, 0.f);
		_float4				vOffsetRotation = _float4(0.f, 0.f, 0.f, 1.f);
		_float				fStaticFriction = { 0.5f };
		_float				fDynamicFriction = { 0.5f };
		_float				fRestitution = { 0.f };
		PxGeometry*			pGeometry = { nullptr };
		_uint				eConstraintFlag = { None };
		COLLISIONFLAG		eThisCollsion = { COL_NONE };
		PxU32				eCollisionFlag = { COL_NONE };
		_float4				vDebugColor = _float4(0.f, 1.f, 0.f, 1.f);
		void*				pCollisionData = { nullptr };
	}RIGIDBODYDESC;

	typedef struct tagCollisionDataDesc
	{
		class CGameObject*	pOwnerObject = { nullptr };
		void*				pCollisionData = { nullptr };
	}COLLISIONDATADESC;

private:
	explicit CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
	const PxMaterial* Get_Material() const { return m_pMaterial; }
	PxRigidBody* Get_RigidBodyActor() const;
	_float3 Get_Position() const;
	_float4 Get_Rotation() const;
	_float3 Get_Current_Velocity() const;
	void Set_Position(_float3 vPosition);
	void Set_Rotation(_float4 vRotation);
	void Set_Material(_float3 vMaterial);
	void Set_Constraint(_uint eConstraintFlag, _bool _isEnable);
	void Set_Kinematic(_bool _isKinematic);
	void Set_Density(_float _fDensity) const;
	void Set_AngularDamping(_float _fAngualrDamping) const;
	void Set_LinearDamping(_float _fLinearDamping) const;
	void Set_Gravity(_bool _isGravity);
	void Set_Mass(_float fMass);
	void Set_CollisionFlag(const _char* szColliderTag, PxU32 eCollisionFlag);
	void Set_CollisionFlag(const _char* szColliderTag, PxU32 eThisCollisionFlag, PxU32 eOtherCollisionFlag);

#ifdef _DEBUG
	void Set_DebugColor(_uint iColliderIndex, _float4 _vColor) { m_Colors[iColliderIndex] = _vColor; }
#endif // _DEBUG

	_bool Is_Static()  const { return m_isStatic; }
	_bool Is_Dynamic()  const { return !m_isStatic; }
	_bool Is_Kinematic() const { return m_isKinematic; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif // _DEBUG

public:
	HRESULT Create_Collider(RIGIDBODYDESC* pRigidBodyDesc);

	void Put_To_Sleep() const;
	void Add_Force(const _float3 & _vForce, PxForceMode::Enum _eMode = PxForceMode::eFORCE, _bool _bAutowake = true) const;
	void Add_Torque(const _float3& _vTorque, PxForceMode::Enum _eMode = PxForceMode::eFORCE, _bool _bAutowake = true) const;
	void Clear_Force(PxForceMode::Enum _eMode = PxForceMode::eFORCE) const;
	void Clear_Torque(PxForceMode::Enum _eMode = PxForceMode::eFORCE) const;

	void Translate(_float3 _vPosition) const;
	void Rotate(_float4 _vRotation) const;

	void Enable_Collision(const _char* szColliderTag, CGameObject* pThisCollision);
	void Enable_Collision(const _char* szColliderTag, CGameObject* pThisCollision, void* pCollisionData);
	void Disable_Collision(const _char* szColliderTag);

private:
	PxRigidActor*					m_pActor = { nullptr };
	PxMaterial*						m_pMaterial = { nullptr };
	PxGeometry*						m_pGeometry = { nullptr };
	PxScene*						m_pScene = { nullptr };
	_umap<const _char*, PxShape*>	m_Shapes;

private:
	_bool							m_isStatic = { false };
	_bool							m_isKinematic = { false };

#ifdef _DEBUG
private:
	vector<CShader*>				m_Shaders;
	vector<CComponent*>				m_Debug_Renders;
	vector<_float4>					m_Colors;
#endif // _DEBUG

private:
	PxShape* Find_Shape(const _char* szShapeTag);
	_bool IsValid(RIGIDBODYDESC* pRigidBodyDesc);

#ifdef _DEBUG
private:
	HRESULT Add_Components(PxGeometry* pPxValues, PxShape* pShape);
	HRESULT SetUp_ShaderResources(_uint iColliderIndex);
#endif // _DEBUG

public:
	static CRigidBody* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END