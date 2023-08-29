#pragma once
#include "ImWindow.h"
#include "MainModule.h"

BEGIN(Tool)

class CEffect_Window final : public CImWindow
{
private:
	explicit CEffect_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CEffect_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	void SaveFileDialog();
	void LoadFileDialog();
	void CreateFileDialog();

private:
	_bool		m_bMainCheckBox;
	MAIN_MODULE m_MainModuleDesc;

	CParticleSystem* m_pDummyParticle = { nullptr };
public:
	static CEffect_Window* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize);
	virtual void Free(void) override;
};

END