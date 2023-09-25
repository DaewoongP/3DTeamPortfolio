#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Magic.h"

BEGIN(Engine)
class CRigidBody;
class CRender;
END

BEGIN(Client)
class CWeapon_Player_Wand;
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
		CTransform*				pTarget = { nullptr };
		_float4x4				TargetOffsetMatrix = {};
		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		BUFF_TYPE				eBuffType = { BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
		COLLISIONFLAG			eCollisionFlag;
		_float					fDamage = { 0 };
		_float					fDistance = { 0 };
		_float					fLifeTime = { 1.0f };
		const _float4x4*		pWeaponMatrix = {nullptr};
		_float4x4				WeaponOffsetMatrix = {};
	}MAGICBALLINITDESC;

	typedef struct CollsionRequestDesc
	{
		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		BUFF_TYPE				eBuffType = { BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
		_float					fDamage = { 0 };
		function<void(_float3,_float)>		Action = { nullptr };
	}COLLSIONREQUESTDESC;

	typedef struct tagMagicBallDesc
	{
		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		BUFF_TYPE				eBuffType = { BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
		_float3					vStartPosition = {};
		_float					fDamage = { 0 };
		_float					fDistance = { 0 };
		_float					fInitLifeTime = { 1.0f };
		_float					fLifeTime = { 1.0f };
	}MAGICBALLDESC;
	
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
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Reset(MAGICBALLINITDESC& InitDesc);

protected:
	// �浹�� ���� ������ٵ� �Դϴ�.
	CRigidBody*				m_pRigidBody = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	//Ÿ�ٿ� ���� Ʈ��������.
	CTransform*				m_pTarget = { nullptr };
	_float4x4				m_TargetOffsetMatrix = {};
	
	//������ ���� ��ġ, ��������
	const _float4x4*		m_pWeaponMatrix = { nullptr };
	_float4x4				m_WeaponOffsetMatrix = {};

protected:
	MAGICBALLDESC			m_MagicBallDesc;
	COLLSIONREQUESTDESC		m_CollisionDesc = {};
	COLLISIONFLAG			m_eCollisionFlag;

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