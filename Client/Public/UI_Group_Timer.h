#pragma once

#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CHealth;
END

BEGIN(Client)
class CUI_Back;
class CUI_Font;

class CUI_Group_Timer final : public CGameObject
{
public:
	
private:
	explicit CUI_Group_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_Timer(const CUI_Group_Timer& rhs);
	virtual ~CUI_Group_Timer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:	
	CUI_Back*			m_pUI_Alarm = { nullptr };
	CUI_Font*			m_pUI_Time = { nullptr };

	_float*				m_pTime = { nullptr };

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components();

	HRESULT Add_Fonts(void* pArg);

public:
	static CUI_Group_Timer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END