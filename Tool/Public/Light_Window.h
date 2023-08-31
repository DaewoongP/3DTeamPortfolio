#pragma once
#include "ImWindow.h"
BEGIN(Tool)

class CLight_Window final : public CImWindow
{

private:
	CLight_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight_Window() = default;


public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;



private:
	static CLight_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free() override;
};

END