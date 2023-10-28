#pragma once
/* =============================================== */
//	[CEvent_Enter_Sanctum]
//	: ���� �������� �̺�Ʈ�� ����ϴ� Ŭ����.
//	  ������ ����� ��츦 �����ϰ� Event_Vault_Spawn�� ����������.
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CEnemy;
class CTrigger;
class CCard_Fig;
class CLightStand;
class CSanctum_Door;
END

BEGIN(Client)

class CEvent_Enter_Sanctum final : public CGameObject
{
	enum GATECUTSCENE_SEQUENCE
	{
		GATECUTSCENE_SEQUENCE_FADE_OUT,
		GATECUTSCENE_SEQUENCE_PLAY_CUTSCENE,
		GATECUTSCENE_SEQUENCE_FADE_IN,
		GATECUTSCENE_SEQUENCE_END
	};

private:
	explicit CEvent_Enter_Sanctum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Enter_Sanctum(const CEvent_Enter_Sanctum& rhs);
	virtual ~CEvent_Enter_Sanctum() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CCard_Fig* m_pCard_Fig = { nullptr };
	_umap<wstring, CEnemy*> m_pMonsters;
	vector<CLightStand*> m_pLightStands;
	vector<CSanctum_Door*> m_pDoors;
	_bool m_isSpawn = { false };
	_bool m_isDoorOpen = { false };

	_bool m_isCutSceneOn = { true }; // �ƽſ� ����

	_bool m_isGateCutsceneStart = { false };
	_bool	m_isEnter = { false };

	GATECUTSCENE_SEQUENCE m_eSequence = { GATECUTSCENE_SEQUENCE_END };

	CRenderer* m_pRenderer = { nullptr };

private:
	void Check_Event_On_Torch_To_Spawn();
	void Check_Monsters_Are_Death();

	HRESULT Add_Components();

public:
	static CEvent_Enter_Sanctum* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Enter_Sanctum* Clone(void* pArg) override;
	virtual void Free() override;
};

END