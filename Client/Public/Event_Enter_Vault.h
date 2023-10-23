#pragma once
/* =============================================== */
//	[CEvent_Enter_Vault]
//  : 볼트 진입 시 카드 피그가 대사를 진행하게 만드는 클래스
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CCard_Fig;
class CTrigger;
END

BEGIN(Client)

class CEvent_Enter_Vault final : public CGameObject
{
public:
	typedef struct tagInitEventEnterVault
	{
		_float3 vTriggerSize;
		_float3 vTriggerWorldPosition;
	}INITEVENTENTERVAULT;

private:
	explicit CEvent_Enter_Vault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Enter_Vault(const CEvent_Enter_Vault& rhs);
	virtual ~CEvent_Enter_Vault() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CCard_Fig* m_pCard_Fig = { nullptr };
	CTrigger* m_pTrigger = { nullptr };
	_bool m_isCallTrigger = { false };

private:
	void Check_Event_Enter_Vault();

private:
	HRESULT Add_Components(const INITEVENTENTERVAULT& InitDesc);

public:
	static CEvent_Enter_Vault* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Enter_Vault* Clone(void* pArg) override;
	virtual void Free() override;
};

END