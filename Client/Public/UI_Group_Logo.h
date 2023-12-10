#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI_Dissolve;
class CUI_Effect_Back;
class CUI_Back;

class CUI_Group_Logo final : public CGameObject
{
private:
	explicit CUI_Group_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_Logo(const CUI_Group_Logo& rhs);
	virtual ~CUI_Group_Logo() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CUI_Back*		m_pUI = { nullptr };
	CUI_Dissolve*		m_pBack = { nullptr };
	CUI_Dissolve*		m_pLogo = { nullptr };
	CUI_Dissolve*		m_pText = { nullptr };

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components();
	HRESULT Read_File(const _tchar* pFilePath);
	CUI::UIDESC Load_File(const HANDLE hFile);

public:
	static CUI_Group_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END