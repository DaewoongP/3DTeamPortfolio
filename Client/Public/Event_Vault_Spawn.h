#pragma once
/* =============================================== */
//	[CGame_Manager]
//	: 
//	�� : �ּ�ȯ
//	�� :
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

private: /* ���� ���� ���� */
	_umap<wstring, CEnemy*> m_pMonsters;
	_bool m_isSpawned_1 = { false };
	_bool m_isSpawned_2 = { false };
	void Check_Event_Spawn_1();
	void Check_Event_Spawn_2();

private:
	HRESULT Add_Components();

public:
	static CEvent_Vault_Spawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Vault_Spawn* Clone(void* pArg) override;
	virtual void Free() override;
};

END