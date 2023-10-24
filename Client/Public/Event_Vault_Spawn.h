#pragma once
/* =============================================== */
//	[CGame_Manager]
//	: 
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

class CEvent_Vault_Spawn final : public CGameObject
{
	enum PENSIVESPAWN_SEQUENCE
	{
		PENSIVESPAWN_SEQUENCE_FADE_OUT,
		PENSIVESPAWN_SEQUENCE_PLAY_CUTSCENE,
		PENSIVESPAWN_SEQUENCE_FADE_IN,
		PENSIVESPAWN_SEQUENCE_END
	};
private:
	explicit CEvent_Vault_Spawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Vault_Spawn(const CEvent_Vault_Spawn& rhs);
	virtual ~CEvent_Vault_Spawn() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTrigger* m_pSpawn_Stage_1 = { nullptr };
	CTrigger* m_pSpawn_Stage_2 = { nullptr };
	CTrigger* m_pSpawn_Stage_3 = { nullptr };

	CRenderer* m_pRenderer = { nullptr };

	_bool m_isEnter = { false };
	PENSIVESPAWN_SEQUENCE m_ePensive_Spawn_Sequence = { PENSIVESPAWN_SEQUENCE_END };

private: /* 몬스터 스폰 관련 */
	_umap<wstring, CEnemy*> m_pMonsters;
	_bool m_isSpawned_1 = { false };
	_bool m_isSpawned_2 = { false };
	_bool m_isSpawned_3 = { false };
	void Check_Event_Spawn_1();
	void Check_Event_Spawn_2();
	void Check_Event_Spawn_3();

private:
	HRESULT Add_Components();

public:
	static CEvent_Vault_Spawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Vault_Spawn* Clone(void* pArg) override;
	virtual void Free() override;
};

END