#pragma once
/* =============================================== */
//	[CEvent_Spawn_Dragon_2]
//	: 몬스터 스폰관련 이벤트를 담당하는 클래스.
//	  기존에 사용한 경우를 제외하고 Event_Vault_Spawn은 쓰지마세요.
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CEnemy;
class CTrigger;
END

BEGIN(Client)

class CEvent_Spawn_Dragon_2 final : public CGameObject
{
	enum SPAWN_DRAGON_2_SEQUENCE
	{
		SPAWN_DRAGON_2_SEQUENCE_FADE_OUT,
		SPAWN_DRAGON_2_SEQUENCE_PLAY_CUTSCENE,
		SPAWN_DRAGON_2_SEQUENCE_FADE_IN,
		SPAWN_DRAGON_2_SEQUENCE_END
	};

private:
	explicit CEvent_Spawn_Dragon_2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Spawn_Dragon_2(const CEvent_Spawn_Dragon_2& rhs);
	virtual ~CEvent_Spawn_Dragon_2() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTrigger* m_pTrigger_Spawn = { nullptr };
	CConjuredDragon* m_pDragon = { nullptr };
	_bool m_isSpawned = { false };

private:
	CRenderer* m_pRenderer = { nullptr };
	_bool	m_isEnter = { false };
	SPAWN_DRAGON_2_SEQUENCE m_eSequence = { SPAWN_DRAGON_2_SEQUENCE_END };
private:
	void Check_Event_Spawn_Dragon_2();

private:
	HRESULT Add_Components();

public:
	static CEvent_Spawn_Dragon_2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Spawn_Dragon_2* Clone(void* pArg) override;
	virtual void Free() override;
};

END