#pragma once
#include "ImWindow.h"
#include "CutScene_Camera_Tool.h"

BEGIN(Tool)

class CMain_Camera;

class CCamera_Window final : public CImWindow
{
private:
	explicit CCamera_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCamera_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;







private:
	CCutScene_Camera_Tool* m_pCutSceneCameraTool;

private:
	


private:


public:
	static CCamera_Window* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize);
	virtual void Free(void) override;
};

END