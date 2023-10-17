#pragma once

#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI_Dynamic_Back;
class CUI_Font;

class CUI_Interaction final : public CGameObject
{
public:
	enum INTERACTIONFONT
	{
		NAME,
		FUNC,
		INTERACTIONFONT_END
	};

public:
	struct INTERACTIONDESC
	{
		_tchar			m_wszName[MAX_PATH] = TEXT("");
		_tchar			m_wszFunc[MAX_PATH] = TEXT("");
		const _float4x4*		m_WorldMatrix;
	};

private:
	explicit CUI_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Interaction(const CUI_Interaction& rhs);
	virtual ~CUI_Interaction() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CUI_Dynamic_Back*	m_pUI_Dynamic_Back = { nullptr };
	CUI_Font*			m_pFont[INTERACTIONFONT_END];

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components();
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	_float2			Caculate_Projection(_float3 vPos);

private:
	_tchar			m_wszName[MAX_PATH] = TEXT("");
	_tchar			m_wszFunc[MAX_PATH] = TEXT("");
	const _float4x4*		m_WorldMatrix;

public:
	static CUI_Interaction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END