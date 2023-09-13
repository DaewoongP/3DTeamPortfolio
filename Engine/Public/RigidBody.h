#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
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
		// this������ �����ϸ� �˴ϴ�.
		// ���������� ���۷��� ī��Ʈ ����
		CGameObject* pOwnerObject = { nullptr };
		// static(true) : �������� �ʴ� ��ü (�ٸ� ��ü�� �浹�� ���� ������ ���� ����.)
		// dynamic(false) : �����̴� ��ü
		_bool isStatic = { false };
		// Trigger �ɼ�
		// ****** Enter�� exit�� ó���˴ϴ�. ******
		_bool isTrigger = { false };
		// �ʱ� ������ ����
		// ����� �پ����� ��� ƨ���� ���� �� �ֽ��ϴ�.
		_float3 vInitPosition;
		// ���� ���¿����� �������
		// ���������� �����Ҷ������� ���� �󸶳� ���� ������� ���� �����Դϴ�.
		// default : 0.5f
		// 0�ϼ��� ���� ���� ������ ġ������
		// 1�ϼ��� ���� ���̵� ������ ġ�����ϴ�.
		// 1�� ���� maxvelocity�� ������� �������� ���� ���� �ֽ��ϴ�.
		_float fStaticFriction = { 0.f };
		// �����̴� ���¿����� �������
		// �����϶� ���⶧������ ���� �󸶳� ������� ���� �����Դϴ�.
		// default = 0.5f
		// 0�ϼ��� �̲������� ���ϰ�
		// 1�ϼ��� �̲������� ���� ������ �����մϴ�.
		// 0�ϰ�� ������ �ʰ� ��� �̲����� ���� �ֽ��ϴ�.
		_float fDynamicFriction = { 0.f };
		// ź�����
		// ���� �浹�Ͽ����� ƨ���� ������ ������ ���� �����Դϴ�.
		// default = 0.f
		// 0�ϼ��� �ε����� ƨ�ܳ����� �ʰ�
		// 1�ϼ��� �ε����� �ָ� ƨ�ܳ����ϴ�.
		_float fRestitution = { 0.f };
		// GeometryType�� ���� ���·� �������ݴϴ�.
		// ex) eSphere, PxSphereGeometry
		// ����ü ���·ν� �����ϰ� ��������� ä�� �ֽø� �˴ϴ�.
		// ex) PxSphereGeometry Sphere;
		// Sphere.radius = 5.f;
		PxGeometry* pGeometry = { nullptr };
		// ******** Dynamic Rigid Body�� ���� �ɼ��Դϴ� ********
		// �ݶ��̴��� �������� ���� ���� �Ǵ� ȸ���� �����մϴ�.
		// ����������ε� ȸ������ �������� �Ʒ��Ͱ��� ó���ϸ� �˴ϴ�.
		// ex) Constraint = RotX | RotY | RotZ;
		RigidBodyConstraint Constraint;
		// ����� �÷�
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
	PxGeometry*				m_pGeometry = { nullptr };
	PxScene*				m_pScene = { nullptr };

private:
	_bool					m_isStatic = { false };
	_bool					m_isKinematic = { false };

#ifdef _DEBUG
private:
	CShader*				m_pShader = { nullptr };
	CComponent*				m_pDebug_Render = { nullptr };
	_float4					m_vColor;
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