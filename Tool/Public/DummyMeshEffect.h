#pragma once

//#include "Composite.h"
#include "MeshEffect.h"
#include "Modules.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CVIBuffer;
class CShader;
class CModel;
class CTransform;
END

BEGIN(Tool)
class CImageFileDialog;
class CComboBox;
END;

BEGIN(Tool)
class CDummyMeshEffect final : public CMeshEffect
{
protected:
	explicit CDummyMeshEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDummyMeshEffect(const CDummyMeshEffect& _rhs);
	virtual ~CDummyMeshEffect();

public:
	HRESULT Initialize(void* pArg) override;

public:
	void Tick_Imgui(_float fTimeDelta);
	
	void ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath);
	void ChangeModel(CModel** _pModel, wstring& _wstrOriginPath, const _tchar* _pDestPath, CModel::TYPE _eAnimType = CModel::TYPE_NONANIM);

public:
	HRESULT Save_FileDialog();
	HRESULT Load_FileDialog();

public:
	_bool m_isHardReset = { false };

private:
	CImageFileDialog* m_pTextureIFD = { nullptr };
	CImageFileDialog* m_pClipTextureIFD = { nullptr };
	CImageFileDialog* m_pEmissionTextureIFD = { nullptr };
	CComboBox* m_pClipChannelCombo = { nullptr };
	CComboBox* m_pEmissionChannelCombo = { nullptr };
	CComboBox* m_pPassComboBox = { nullptr };
	CComboBox* m_pColorEaseCombo = { nullptr };
	CComboBox* m_pPosEaseCombo = { nullptr };
	CComboBox* m_pRotEaseCombo = { nullptr };
	CComboBox* m_pScaleEaseCombo = { nullptr };

	list<_tchar*> m_pTags;
public:
	static CDummyMeshEffect* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const _tchar* pFilePath, _uint _iLevel = 0);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END