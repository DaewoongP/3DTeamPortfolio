#pragma once
#include "UI.h"
#include "UI_DamageFont.h"
#include "Client_Defines.h"


BEGIN(Client)

class CUI_Damage final : public CGameObject
{
public:
	struct UIDAMAGE
	{
		_tchar m_wszObjectName[MAX_PATH] = TEXT("");
		_tchar m_wszGetType[MAX_PATH] = TEXT("");
	};

private:
	explicit CUI_Damage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Damage(const CUI_Damage& rhs);
	virtual ~CUI_Damage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	vector<CUI_DamageFont*>			m_DamageFonts;

private:
	HRESULT Add_Prototype();
	
public:
	HRESULT Add_Font(_uint iDamage, _float3 vTargetPos);

private:
	CUI_DamageFont::DAMAGEFONTDESC		m_DamageFontDesc;

public:
	static CUI_Damage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END