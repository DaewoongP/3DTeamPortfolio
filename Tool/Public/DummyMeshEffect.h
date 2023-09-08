#pragma once

//#include "Composite.h"
#include "MeshEffect.h"
#include "Modules.h"
#include "Shader_Nodes.h"

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
	void ImGui(_float fTimeDelta);
	
	void ChangeTexture(const _tchar* _pTag);
	void ChangeModel(const _tchar* _pTag);

private:
	CImageFileDialog* m_pTextureIFD = { nullptr };
	CImageFileDialog* m_pAlphaClipTextureIFD = { nullptr };
	CComboBox* m_pPassComboBox = { nullptr };

public:
	static CDummyMeshEffect* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const _tchar * _pDirectoryPath);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END