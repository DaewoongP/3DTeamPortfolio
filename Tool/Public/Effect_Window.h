#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CComboBox;

END
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
	void ShapeModule_TreeNode();

private: // 위젯 관련
	void TableDragFloat(string _strTag, _float* _pValue, _float _fDragSpeed = 0.01f, _float _fMin = 0.f, _float _fMax = FLT_MAX);
	void TableDragFloat2Range(string _strTag, _float2* _pValue, _float _fDragSpeed = 0.01f, _float _fMin = 0.f, _float _fMax = FLT_MAX);
	void TableDragXYZ(string _strTag, _float3* pValue, _float _fDragSpeed = 0.01f, _float _fMin = 0.f, _float _fMax = FLT_MAX);
	void TableCheckBox(string _strTag, _bool* _pValue);
	void TableColorEdit4(string _strTag, _float4* pValue);

private:
	_float		m_fWidgetSize = { 200.f };
	CComboBox* m_pEmitterVelocity_ComboBox = { nullptr };
	
private:
	class CDummyParticle* m_pDummyParticle = { nullptr };
	CParticleSystem* m_pParticleSystem = { nullptr };

public:
	static CEffect_Window* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize);
	virtual void Free(void) override;
};

END