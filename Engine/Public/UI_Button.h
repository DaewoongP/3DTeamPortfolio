#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CUI_Button final : public CComponent
{
protected:
	explicit CUI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Button(const CUI_Button& rhs);
	virtual ~CUI_Button() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

public:
	_bool	Collision_Rect(HWND hWnd, _float2 vPos, _float2 vSize);
	_bool	Click(HWND hWnd, _float2 vPos, _float2 vSize);

public:
	static CUI_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
