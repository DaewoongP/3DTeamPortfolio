#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CAnimation_Window final : public CImWindow
{
private:
	explicit CAnimation_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAnimation_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// 현재 수정중인 더미 오브젝트
	class CDummy* m_pDummyObject = { nullptr };

public:
	static CAnimation_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END