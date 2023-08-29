#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CEffect_Window final : public CImWindow
{
private:
	explicit CEffect_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEffect_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fDuration;
	_bool m_isLooping;
	_bool m_isPrewarm;
	_float m_fStartDelay;
	_float m_fStartLifeTime;
	_float m_fStartSpeed;

public:
	static CEffect_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END