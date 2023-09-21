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
		_float4x4				TargetOffsetMatrix = {};
		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		BUFF_TYPE				eBuffType = { BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
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
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;

protected:
	// 충돌을 위한 리지드바디 입니다.
	CRigidBody*		m_pRigidBody = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	//타겟에 대한 트랜스폼임.
	CTransform*		m_pTarget = { nullptr };
	_float4x4		m_TargetOffsetMatrix = {};
	
	//무기의 현재 위치, 오프셋임
	const _float4x4*			m_pWeaponMatrix = { nullptr };
	_float4x4			m_WeaponOffsetMatrix = {};

protected:
	MAGICBALLDESC			m_MagicBallDesc;

	COLLSIONREQUESTDESC		m_CollisionDesc = {};

protected:
	HRESULT Add_Components();
	//기본적인 리지드바디 할당임. 크기 바꾸고싶으면 오버라이드 하면됨.
	virtual HRESULT Add_RigidBody();
	virtual HRESULT Add_Effect() = 0;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END		