#pragma once
/* =============================================== */
//	[CGame_Manager]
//	: 
//	정 : 심정환
//	부 :
//  
//	밟으면 횃불이 점화되는 트리거
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CVault_Torch;
class CTrigger;
END

BEGIN(Client)

class CEvent_Vault_Torch final : public CGameObject
{ 
private:
	explicit CEvent_Vault_Torch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Vault_Torch(const CEvent_Vault_Torch& rhs);
	virtual ~CEvent_Vault_Torch() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTrigger* m_pTorch_Stage_1 = { nullptr };
	CTrigger* m_pTorch_Stage_2 = { nullptr };

private: /* 횃불 점등 관련 */
	_umap<wstring, CVault_Torch*> m_pTorchs_1; // 1번 구역
	_umap<wstring, CVault_Torch*> m_pTorchs_2; // 2번 구역

	_bool m_isOn_1 = { false };
	_bool m_isOn_2 = { false };

	_float m_fStage1Time = { 0.f };
	_float m_fStage2Time = { 0.f };

private:
	HRESULT Add_Components();
	void Check_Event_On_1(_float fTimeDelta);
	void Check_Event_On_2(_float fTimeDelta);

public:
	static CEvent_Vault_Torch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Vault_Torch* Clone(void* pArg) override;
	virtual void Free() override;
};

END