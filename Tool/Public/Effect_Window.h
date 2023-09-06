#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CComboBox;
class CImageFileDialog;
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
	void MeshEffect(_float _fTimeDelta);

	void Save_FileDialog();
	void Load_FileDialog();

	// 새로운 더미 파티클 생성.
	void Create_Button();

	void MainMoudle_TreeNode();
	void EmissionModule_TreeNode();
	void ShapeModule_TreeNode();
	void RendererModule_TreeNode();

private: // 위젯 관련
	_bool Table_DragFloat(string _strName, string _strTag, _float* _pValue, _float _fDragSpeed = 0.01f, _float _fMin = 0.f, _float _fMax = FLT_MAX);
	_bool Table_DragFloat2Range(string _strName, string _strTag, _float2* _pValue, _float _fDragSpeed = 0.01f, _float _fMin = 0.f, _float _fMax = FLT_MAX);
	_bool Table_DragXYZ(string _strName, string _strTag, _float3* pValue, _float _fDragSpeed = 0.01f, _float _fMin = 0.f, _float _fMax = FLT_MAX, _bool isImplement = true);
	_bool Table_DragInt(string _strName, string _strTag, _int* _pValue, _float _fDragSpeed = 0.1f, _int _iMin = 0.f, _int _iMax = INT_MAX);
	_bool Table_CheckBox(string _strName, string _strTag, _bool* _pValue, _bool isImplement = true);
	_bool Table_ColorEdit4(string _strName, string _strTag, _float4* pValue);
	_bool Table_ImageButton(string _strName, string _strTag, CImageFileDialog* _pImageFileDialog);
	_bool Table_Void();
	_bool Table_DragFloatWithOption(string _strName, string _strTag, _float* _pValue, _float2* _pRangeValue, _bool* _pCheckBox, _float _fDragSpeed = 0.01f, _float _fMin = 0.f, _float _fMax = FLT_MAX);
private:
	_float	   m_fWidgetSize = { 200.f };
	CComboBox* m_pEmitterVelocity_ComboBox = { nullptr };
	CComboBox* m_pShapeCombo = { nullptr };
	CComboBox* m_pMeshModeCombo = { nullptr };
	CComboBox* m_pSpriteTypeCombo = { nullptr };
	CComboBox* m_pArcModeCombo = { nullptr };
	CComboBox* m_pMeshCombo = { nullptr };
	CComboBox* m_pConeEmitFromCombo = { nullptr };
	CComboBox* m_pBoxEmitFromCombo = { nullptr };
	CComboBox* m_pMeshTypeCombo = { nullptr };
	CComboBox* m_pClipChannelCombo = { nullptr };

	CImageFileDialog* m_pMaterialTexture = { nullptr };
	CImageFileDialog* m_pAlphaTexture = { nullptr };
	CImageFileDialog* m_pSpriteTexture = { nullptr };

private:
	class CDummyParticle* m_pDummyParticle = { nullptr };
	CParticleSystem* m_pParticleSystem = { nullptr };
	CModel* m_pModel = { nullptr };

	_bool m_isPrevLooping = { false };

public:
	static CEffect_Window* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize);
	virtual void Free(void) override;
};

END