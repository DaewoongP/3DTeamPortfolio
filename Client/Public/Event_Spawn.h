#pragma once
/* =============================================== */
//	[CEvent_Spawn]
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

class CEvent_Spawn final : public CGameObject
{
public:
	typedef struct tagInitEventSpawn
	{
		// 레이어에 등록된 적 객체의 컴포넌트 태그를 참고해서 Spawn을 실행할 것임
		// 즉, 툴에서 미리 몬스터의 컴포넌트 태그를 일률적으로 저장해야 함.
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