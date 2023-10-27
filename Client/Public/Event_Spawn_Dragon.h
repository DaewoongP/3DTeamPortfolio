#pragma once
/* =============================================== */
//
//	정 : 장현우
//	부 :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CRenderer;
class CParticleSystem;
END

BEGIN(Client)
class CEnemy;
class CTrigger;
END

BEGIN(Client)

class CEvent_Spawn_Dragon final : public CGameObject
{
	enum DRAGONSPAWN_SEQUENCE
	{
		DRAGONSPAWN_SEQUENCE_EGG_FADE_OUT,
		DRAGONSPAWN_SEQUENCE_EGG_PLAY_CUTSCENE,
		DRAGONSPAWN_SEQUENCE_DRAGON_ENTER_FADE_OUT,
		DRAGONSPAWN_SEQUENCE_DRAGON_ENTER_PLAY_CUTSCENE,
		DRAGONSPAWN_SEQUENCE_DRAGON_FADE_IN,
		DRAGONSPAWN_SEQUENCE_END
	};
private:
	explicit CEvent_Spawn_Dragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Spawn_Dragon(const CEvent_Spawn_Dragon& rhs);
	virtual ~CEvent_Spawn_Dragon() = default;

public:
	HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	_uint m_iLevel = { 0 };
	CTrigger* m_pSpawn_Dragon = { nullptr };
	CParticleSystem* m_pParticle_Dragon_Egg = { nullptr };
	CParticleSystem* m_pParticle_Dragon_Egg_Distortion = { nullptr };

private:
	CRenderer* m_pRenderer = { nullptr };
private:
	_bool m_isEnter = { false };
	DRAGONSPAWN_SEQUENCE m_eDragon_Spawn_Sequence = { DRAGONSPAWN_SEQUENCE_END };

	_bool m_isScream = { false };

	_bool m_isEgg[4] = { false, false, false, false };


private: /* 몬스터 스폰 관련 */
	_umap<wstring, CEnemy*> m_pMonsters;
	_bool m_isSpawned_Dragon = { false };
	void Check_Event_Spawn_Dragon();

private:
	HRESULT Add_Components();

public:
	static CEvent_Spawn_Dragon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CEvent_Spawn_Dragon* Clone(void* pArg) override;
	virtual void Free() override;
};

END