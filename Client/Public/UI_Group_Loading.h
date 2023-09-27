#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI_Back;
class CUI_Logo;

class CUI_Group_Loading final : public CGameObject
{
private:
	explicit CUI_Group_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_Loading(const CUI_Group_Loading& rhs);
	virtual ~CUI_Group_Loading() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CUI_Back* m_pBack = { nullptr };
	CUI_Back* m_pScreen = { nullptr };

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components();
	HRESULT Read_File(const _tchar* pFilePath);
	CUI::UIDESC Load_File(const HANDLE hFile);

public:
	static CUI_Group_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END