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

class CEvent_Smeade final : public CGameObject
{
private:
	explicit CEvent_Smeade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Smeade(const CEvent_Smeade& rhs);
	virtual ~CEvent_Smeade() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTrigger* m_pSpawn_Troll = { nullptr };

private: /* 몬스터 스폰 관련 */
	_umap<wstring, CEnemy*> m_pMonsters;
	_bool m_isSpawned_Troll = { false };
	void Check_Event_Spawn_Troll();

private:
	HRESULT Add_Components();

public:
	static CEvent_Smeade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Smeade* Clone(void* pArg) override;
	virtual void Free() override;
};

END