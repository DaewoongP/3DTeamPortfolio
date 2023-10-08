#pragma once
#include "UI_Button.h"

BEGIN(Engine)

class ENGINE_DLL CUI_ScaleButton final : public CUI_Button
{
protected:
	explicit CUI_ScaleButton(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CUI_ScaleButton(const CUI_Button& rhs);
	virtual ~CUI_ScaleButton() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

public:
	virtual _bool	Collision_Rect(HWND hWnd, _float2 vPos, _float2 vSize) override;

private:
	_float m_fRatio = { 0.f };
	_float2 m_vSize;
	_float2 m_vOriginScale = { 1.f, 1.f };
	_float2	m_vLerpScale = {};
	_float2 m_vTargetScale = { 2.f, 2.f };
	_float m_fLerpSpeed = { 1.f };
	_bool m_isCollsion = { false };

public:
	static CUI_ScaleButton* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
