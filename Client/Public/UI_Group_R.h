#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUI_Group_R final : public CGameObject
{
private:
	explicit CUI_Group_R(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_R(const CUI_Group_R& rhs);
	virtual ~CUI_Group_R() = default;

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
	static CUI_Group_R* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END