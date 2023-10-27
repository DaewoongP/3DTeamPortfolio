#pragma once
/* =============================================== */
//	[CEvent_Spawn_Dragon_2]
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

class CEvent_Spawn_Dragon_2 final : public CGameObject
{
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
	void Check_Event_Spawn_Dragon_2();

private:
	HRESULT Add_Components();

public:
	static CEvent_Spawn_Dragon_2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Spawn_Dragon_2* Clone(void* pArg) override;
	virtual void Free() override;
};

END