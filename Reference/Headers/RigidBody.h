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
		// static(true) : 움직이지 않는 물체 (다른 객체의 충돌에 의해 움직일 수도 있음.)
		// dynamic(false) : 움직이는 물체
		_bool isStatic = { false };
		// 초기 포지션 세팅
		// 지면과 붙어있을 경우 튕겨져 나갈 수 있습니다.
		_float3 vInitPosition;
		// 정지 상태에서의 마찰계수
		// 멈춰있을때 동작할때까지의 힘이 얼마나 많이 드는지에 대한 변수입니다.
		// default : 0.5f
		// 0일수록 힘이 덜들어서 빠르게 치고나가고
		// 1일수록 힘이 많이들어서 느리게 치고나갑니다.
		// 1에 가까운데 maxvelocity가 작을경우 움직이지 않을 수도 있습니다.
		_float fStaticFriction = { 0.f };
		// 움직이는 상태에서의 마찰계수
		// 움직일때 멈출때까지의 힘이 얼마나 드는지에 대한 변수입니다.
		// default = 0.5f
		// 0일수록 미끄러짐이 심하고
		// 1일수록 미끄러짐이 없어 빠르게 정지합니다.
		// 0일경우 멈추지 않고 계속 미끄러질 수도 있습니다.
		_float fDynamicFriction = { 0.f };
		// 탄성계수
		// 무언가 충돌하였을때 튕겨져 나가는 정도에 대한 변수입니다.
		// default = 0.f
		// 0일수록 부딪혀도 튕겨나가지 않고
		// 1일수록 부딪히면 멀리 튕겨나갑니다.
		_float fRestitution = { 0.f };
		// GeometryType과 같은 형태로 대입해줍니다.
		// ex) eSphere, PxSphereGeometry
		// 구조체 형태로써 선언하고 멤버변수를 채워 주시면 됩니다.
		// ex) PxSphereGeometry Sphere;
		// Sphere.radius = 5.f;
		PxGeometry* pGeometry = { nullptr };
		// ******** Dynamic Rigid Body를 위한 옵션입니다 ********
		// 콜라이더를 움직이지 않을 방향 또는 회전을 선택합니다.
		// 어느방향으로도 회전하지 않으려면 아래와같이 처리하면 됩니다.
		// ex) Constraint = RotX | RotY | RotZ;
		RigidBodyConstraint Constraint;
		// 디버그 컬러
		_float4		vDebugColor = _float4(0.f, 1.f, 0.f, 1.f);
	}RIGIDBODYDESC;

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
	void Set_AngularDamping(_float _fAngualrDamping) const;

#ifdef _DEBUG
	void Set_DebugColor(_float4 _vColor) { m_vColor = _vColor; }
#endif // _DEBUG


	_bool Is_Static()  const { return m_isStatic; }
	_bool Is_Dynamic()  const { return !m_isStatic; }
	_bool Is_Kinematic() const { return m_isKinematic; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif // _DEBUG

public:
	HRESULT Create_Actor(RIGIDBODYDESC* pRigidBodyDesc);

	void Put_To_Sleep() const;
	void Add_Force(const _float3 & _vForce, PxForceMode::Enum _eMode = PxForceMode::eFORCE, _bool _bAutowake = true) const;
	void Add_Torque(const _float3& _vTorque, PxForceMode::Enum _eMode = PxForceMode::eFORCE, _bool _bAutowake = true) const;
	void Clear_Force(PxForceMode::Enum _eMode = PxForceMode::eFORCE) const;
	void Clear_Torque(PxForceMode::Enum _eMode = PxForceMode::eFORCE) const;

	void Translate(_float3 _vPosition) const;
	void Rotate(_float4 _vRotation) const;

private:
	PxRigidActor*			m_pActor = { nullptr };
	PxMaterial*				m_pMaterial = { nullptr };
	PxScene*				m_pScene = { nullptr };

private:
	_bool					m_isStatic = { false };
	_bool					m_isKinematic = { false };

#ifdef _DEBUG
private:
	CShader*				m_pShader = { nullptr };
	CComponent*				m_pDebug_Render = { nullptr };
	_float4					m_vColor;

private:
	_uint					m_iNumLineBuffer = { 0 };
	_uint					m_iStartLineBufferIndex = { 0 };

	_uint					m_iNumTriangleBuffer = { 0 };
	_uint					m_iStartTriangleBufferIndex = { 0 };
#endif // _DEBUG


#ifdef _DEBUG
private:
	HRESULT Add_Components(PxGeometry* pPxValues);
	HRESULT SetUp_ShaderResources();
#endif // _DEBUG

public:
	static CRigidBody* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END