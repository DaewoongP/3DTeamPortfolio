#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CEffect_Window final : public CImWindow
{
private:
	explicit CEffect_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CEffect_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize) override;
	virtual void Tick(_float _fTimeDelta) override;

	void Save_FileDialog();
	void Load_FileDialog();

	// 새로운 더미 파티클 생성.
	void Create_Button();

	void MainMoudle_TreeNode();
	void EmissionModule_TreeNode();

private:
	_bool		m_bMainCheckBox = { true };
	_bool		m_bEmmissionCheckBox = { true };
	//_bool		m_bEmmissionCheckBox = { true };

private:
	class CDummyParticle* m_pDummyParticle = { nullptr };

public:
	static CEffect_Window* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize);
	virtual void Free(void) override;
};

END