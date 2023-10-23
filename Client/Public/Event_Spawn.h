#pragma once
/* =============================================== */
//	[CEvent_Spawn]
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
END

BEGIN(Client)

class CEvent_Spawn final : public CGameObject
{
public:
	typedef struct tagInitEventSpawn
	{
		// ���̾ ��ϵ� �� ��ü�� ������Ʈ �±׸� �����ؼ� Spawn�� ������ ����
		// ��, ������ �̸� ������ ������Ʈ �±׸� �Ϸ������� �����ؾ� ��.
		wstring wstrSpawnEnemyComponentTag = { TEXT("") };
		LEVELID eMonsterLevel = LEVEL_END;
		_float3 vTriggerSize;
		_float3 vTriggerWorldPosition;
	}INITEVENTSPAWM;

private:
	explicit CEvent_Spawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Spawn(const CEvent_Spawn& rhs);
	virtual ~CEvent_Spawn() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTrigger* m_pTrigger_Spawn = { nullptr };
	_umap<wstring, CEnemy*> m_pMonsters;
	_bool m_isSpawned = { false };

private:
	void Check_Event_Spawn();

private:
	HRESULT Add_Components(const INITEVENTSPAWM& InitDesc);

public:
	static CEvent_Spawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Spawn* Clone(void* pArg) override;
	virtual void Free() override;
};

END