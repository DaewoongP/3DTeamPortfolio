#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Magic.h"

BEGIN(Engine)
class CRigidBody;
END

BEGIN(Client)
class CMagicBall : public CGameObject
{
public:
	// 이동의 타입. 어떤식으로 날라갈지?
	enum MOVINGTYPE { MOVETYPE_STRAIGHT, MOVETYPE_CURVE, MOVETYPE_CURVE_S, MOVETYPE_SPIN, MOVETYPE_END};

	typedef struct MagicBallInitDesc
	{
		CTransform*				pTarget = { nullptr };
		_float3					vStartPos = {};
		CMagic::MAGIC_GROUP		eMagicGroup = { CMagic::MG_END };
		CMagic::MAGIC_TYPE		eMagicType = { CMagic::MT_END };
		CMagic::BUFF_TYPE		eBuffType = { CMagic::BUFF_NONE };
		SPELL					eMagicTag = { SPELL_END };
		_float					fDamage = { 0 };
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
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;

protected:
	// 충돌을 위한 리지드바디 입니다.
	CRigidBody*		m_pRigidBody = { nullptr };
	CTransform*		m_pTarget = { nullptr };

protected:
	CMagic::MAGIC_GROUP		m_eMagicGroup = { CMagic::MG_END };
	CMagic::MAGIC_TYPE		m_eMagicType = { CMagic::MT_END };
	CMagic::BUFF_TYPE		m_eBuffType = { CMagic::BUFF_NONE };
	SPELL					m_eMagicTag = { SPELL_END };
	_float3					m_vStartPosition = {};
	_float					m_fDamage = { 0 };

protected:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect() = 0;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END		