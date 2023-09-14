#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Magic.h"

BEGIN(Engine)
class CRigidBody;
END

BEGIN(Client)
class CWeapon_Player_Wand;
END

BEGIN(Client)
class CMagicBall : public CGameObject
{
public:
	typedef struct MagicBallInitDesc
	{
		CTransform*				pTarget = { nullptr };
		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		CMagic::BUFF_TYPE		eBuffType = { CMagic::BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
		_float					fDamage = { 0 };
		_float					fDistance = { 0 };
		_float					fLiftTime = { 1.0f };
		CWeapon_Player_Wand*	pWeapon = { nullptr };
	}MAGICBALLINITDESC;

protected:
	explicit CMagicBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMagicBall(const CMagicBall& rhs);
	virtual ~CMagicBall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;

protected:
	// �浹�� ���� ������ٵ� �Դϴ�.
	CRigidBody*		m_pRigidBody = { nullptr };

	//Ÿ�ٿ� ���� Ʈ��������.
	CTransform*		m_pTarget = { nullptr };

protected:
	CMagic::MAGIC_GROUP		m_eMagicGroup = { CMagic::MG_END };
	CMagic::MAGIC_TYPE		m_eMagicType = { CMagic::MT_END };
	CMagic::BUFF_TYPE		m_eBuffType = { CMagic::BUFF_NONE };
	SPELL					m_eMagicTag = { SPELL_END };
	_float3					m_vStartPosition = {};
	_float					m_fDamage = { 0 };
	_float					m_fDistance = { 0 };
	_float					m_fInitLiftTime = { 1.0f };
	_float					m_fLiftTime = { 1.0f };
	CWeapon_Player_Wand*	m_pWeapon = { nullptr };

protected:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect() = 0;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END		