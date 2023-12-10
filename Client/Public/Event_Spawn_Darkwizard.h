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

END

BEGIN(Client)
class CEnemy;
class CTrigger;
END

BEGIN(Client)

class CEvent_Spawn_Darkwizard final : public CGameObject
{
	enum DARKWIZARDSPAWN_SEQUENCE
	{
		DARKWIZARDSPAWN_SEQUENCE_FADE_OUT,
		DARKWIZARDSPAWN_SEQUENCE_PLAYCUTSCENE,
		DARKWIZARDSPAWN_SEQUENCE_FADE_IN,
		DARKWIZARDSPAWN_SEQUENCE_END
	};
private:
	explicit CEvent_Spawn_Darkwizard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Spawn_Darkwizard(const CEvent_Spawn_Darkwizard& rhs);
	virtual ~CEvent_Spawn_Darkwizard() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTrigger* m_pSpawn_Darkwizard = { nullptr };

private:
	CRenderer* m_pRenderer = { nullptr };
private:
	_bool m_isEnter = { false };
	DARKWIZARDSPAWN_SEQUENCE m_eDarkwizard_Spawn_Sequence = { DARKWIZARDSPAWN_SEQUENCE_END };


private: /* 몬스터 스폰 관련 */
	_umap<wstring, CEnemy*> m_pMonsters;
	_bool m_isSpawned_Darkwizard = { false };
	void Check_Event_Spawn_Darkwizard();


private:
	HRESULT Add_Components();

public:
	static CEvent_Spawn_Darkwizard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Spawn_Darkwizard* Clone(void* pArg) override;
	virtual void Free() override;
};

END