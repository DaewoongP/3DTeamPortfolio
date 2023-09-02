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
	void Table_DragFloat(string _strTag, _float* _pValue, _float _fDragSpeed = 0.01f, _float _fMin = 0.f, _float _fMax = FLT_MAX);
	void Table_DragFloat2Range(string _strTag, _float2* _pValue, _float _fDragSpeed = 0.01f, _float _fMin = 0.f, _float _fMax = FLT_MAX);
	void Table_DragXYZ(string _strTag, _float3* pValue, _float _fDragSpeed = 0.01f, _float _fMin = 0.f, _float _fMax = FLT_MAX);
	void Table_DragInt(string _strTag, _int* _pValue, _float _fDragSpeed = 0.1f, _int _iMin = 0.f, _int _iMax = INT_MAX);
	void Table_CheckBox(string _strTag, _bool* _pValue);
	void Table_ColorEdit4(string _strTag, _float4* pValue);

private:
	_float	   m_fWidgetSize = { 200.f };
	CComboBox* m_pEmitterVelocity_ComboBox = { nullptr };
	CComboBox* m_pShapeCombo = { nullptr };
	CComboBox* m_pModeCombo = { nullptr };
	CComboBox* m_pBurstTypeCombo = { nullptr };
	CComboBox* m_pArcModeCombo = { nullptr };
	CComboBox* m_pMeshCombo = { nullptr };
	CComboBox* m_pEmitFromCombo = { nullptr };
	CComboBox* m_pMeshTypeCombo = { nullptr };
private:
	class CDummyParticle* m_pDummyParticle = { nullptr };
	CParticleSystem* m_pParticleSystem = { nullptr };

public:
	static CEffect_Window* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize);
	virtual void Free(void) override;
};

END