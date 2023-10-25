#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTrigger;
class CVault_Gate;

class CEvent_Vault_Next_Level final : public CGameObject
{
private:
	explicit CEvent_Vault_Next_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Vault_Next_Level(const CEvent_Vault_Next_Level& rhs);
	virtual ~CEvent_Vault_Next_Level() = default;

public:
	_bool Get_Level_Change() const { return m_isCheck; }

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTrigger* m_pNext_Level = { nullptr };

	CVault_Gate* m_pVault_Gate = { nullptr };

private:
	_bool	m_isCheck = { false };

private:
	HRESULT Add_Components();
	void	Check_Event(_float fTimeDelta);

public:
	static CEvent_Vault_Next_Level* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END