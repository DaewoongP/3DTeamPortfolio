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
		_float					fLifeTime = { 1.0f };
		CWeapon_Player_Wand*	pWeapon = { nullptr };
	}MAGICBALLINITDESC;

	typedef struct tagMagicBallDesc
	{
		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		CMagic::BUFF_TYPE		eBuffType = { CMagic::BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
		_float3					vStartPosition = {};
		_float					fDamage = { 0 };
		_float					fDistance = { 0 };
		_float					fInitLiftTime = { 1.0f };
		_float					fLifeTime = { 1.0f };
	}MAGICBALLDESC;
protected:
	explicit CMagicBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMagicBall(const CMagicBall& rhs);
	virtual ~CMagicBall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;

protected:
	// 충돌을 위한 리지드바디 입니다.
	CRigidBody*		m_pRigidBody = { nullptr };

	//타겟에 대한 트랜스폼임.
	CTransform*		m_pTarget = { nullptr };
	CWeapon_Player_Wand* m_pWeapon = { nullptr };

protected:
	MAGICBALLDESC m_MagicBallDesc;

protected:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect() = 0;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END		