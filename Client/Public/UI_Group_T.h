#pragma once
#include "UI_Group.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUI_Group_T final : public CUI_Group
{
private:
	explicit CUI_Group_T(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_T(const CUI_Group_T& rhs);
	virtual ~CUI_Group_T() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	HRESULT Add_ProtoType();

private:
	vector<_tchar*> m_ProtoTypeTags;

public:
	static CUI_Group_T* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END