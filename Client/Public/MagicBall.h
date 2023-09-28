#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Magic.h"

BEGIN(Engine)
class CRigidBody;
class CRender;
class CTrail;
END

BEGIN(Client)
class CMagicBall : public CGameObject
{
public:
	//�������� ������.		����->Ʈ����->�߻�->���->����
	enum MAGICBALL_STATE { MAGICBALL_STATE_BEGIN, MAGICBALL_STATE_DRAWTRAIL, MAGICBALL_STATE_CASTMAGIC, MAGICBALL_STATE_DYING, MAGICBALL_STATE_END};
public:
	typedef struct MagicBallInitDesc
	{
		const CGameObject*			pTarget = { nullptr };
		const CGameObject*			pWeapon = { nullptr };

		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		BUFF_TYPE				eBuffType = { BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
		COLLISIONFLAG			eCollisionFlag;

		_int					iDamage = { 0 };
		_float					fLifeTime = { 1.0f };
	}MAGICBALLINITDESC;

	typedef struct CollsionRequestDesc
	{
		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		BUFF_TYPE				eBuffType = { BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
		_int					iDamage = { 0 };
		function<void(_float3,_float)>		Action = { nullptr };
	}COLLSIONREQUESTDESC;

protected:
	explicit CMagicBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMagicBall(const CMagicBall& rhs);
	virtual ~CMagicBall() = default;

public:
	// �������� ���¸� ������ �����մϴ�.
	void Set_MagicBallState(MAGICBALL_STATE eState) { m_eMagicBallState = eState; m_isFirstFrameInState = true;}
	// �������� ���¸� �ڿ����� �������� �����մϴ�.
	void Do_MagicBallState_To_Next() { 
		if (m_eMagicBallState != MAGICBALL_STATE_END)
		{
			m_eMagicBallState = static_cast<MAGICBALL_STATE>(m_eMagicBallState + 1);
			m_isFirstFrameInState = true;
		}
	}

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Reset(MAGICBALLINITDESC& InitDesc);

protected:
	virtual void Ready_SpinMove(CTrail* pTrail,_float2 vSpinWeight, _float fSpinSpeed);
	virtual void Ready_SplineMove(CTrail* pTrail);
	virtual void Ready_StraightMove(CTrail* pTrail);
	virtual void Ready_SplineSpinMove(CTrail* pTrail , _float2 vSpinWeight, _float fSpinSpeed);

protected:
	// �浹�� ���� ������ٵ� �Դϴ�.
	CRigidBody*				m_pRigidBody = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	
protected:
	//�Ѿ˿����� �߻� �Ⱓ�̿�. ���忡���� ���ӽð��̴�.
	_float					m_fLifeTime = { 1.0f };

	const CGameObject*		m_pTarget = { nullptr };

	const _float4x4*		m_pWeaponWorldMatrix = { nullptr };
	const _float4x4*		m_pWeaponOffsetMatrix = { nullptr };
	const _float4x4*		m_pTargetWorldMatrix = { nullptr };
	const _float4x4*		m_pTargetOffsetMatrix = { nullptr };

	// �ǽð����� ���ŵ� �ϵ�� Ÿ�� ��ġ
	_float4x4				m_CurrentTargetMatrix = {};
	_float4x4				m_CurrentWeaponMatrix = {};
	// ó������ ��ϵ� �ϵ�� Ÿ�� ��ġ
	_float3					m_vStartPosition = {};
	_float3					m_vEndPosition = {};

	_float					m_fLerpAcc = { 0.f };

	//For. Spin
	_float2					m_vSpinWeight = {};
	_float					m_fSpinSpeed = { 1.f };

	//For. Spline
	_float3					m_vSplineLerp[2] = {};
	_float					m_fTimeScalePerDitance = { 0.f };
	_uint					m_iLevel = { 0 };

protected:
	COLLSIONREQUESTDESC		m_CollisionDesc = {};
	COLLISIONFLAG			m_eCollisionFlag = {};

	//�ൿ ������.
	MAGICBALL_STATE			m_eMagicBallState = { MAGICBALL_STATE_BEGIN };
	_bool					m_isFirstFrameInState = { true };

protected:
	virtual void Ready_Begin() {};
	virtual void Ready_DrawMagic() {};
	virtual void Ready_CastMagic() {};
	virtual void Ready_Dying() {};

	virtual void Tick_Begin(_float fTimeDelta) {};
	virtual void Tick_DrawMagic(_float fTimeDelta) {};
	virtual void Tick_CastMagic(_float fTimeDelta) {};
	virtual void Tick_Dying(_float fTimeDelta) {};

	void Tick_MagicBall_State(_float fTimeDelta);
	void Set_StartPosition();
protected:
	HRESULT Add_Components();
	//�⺻���� ������ٵ� �Ҵ���. ũ�� �ٲٰ������ �������̵� �ϸ��.
	virtual HRESULT Add_RigidBody();
	virtual HRESULT Add_Effect() = 0;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END		