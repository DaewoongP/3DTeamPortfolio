
#pragma once

#include "Texture_Filpbook.h"
#include "Engine_Defines.h"
#include "Tool_Defines.h"
BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CCollider;
class CNavigation;
class CModel;


END

BEGIN(Tool)
class CImageFileDialog;
class CComboBox;
END;

BEGIN(Tool)
class CDummyFlipBook final : public CTexture_Flipbook
{
private:
	CDummyFlipBook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummyFlipBook(const CDummyFlipBook& rhs);
	virtual ~CDummyFlipBook() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel, const _tchar* pFilePath, _uint iWidthNum, _uint iHeightNum);
	virtual HRESULT Initialize(void* pArg) override;

private:
	// 파티클 텍스처 변경.
	void ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath);

public:
	void Tick_Imgui(_float _fTimeDelta);

private:
	class CImageFileDialog* m_pTextureIFD = { nullptr };
	class CImageFileDialog* m_pNormalTextureIFD = { nullptr };
	
	class CComboBox* m_pClipChannelCombo = { nullptr };
	class CComboBox* m_pLoopOption = { nullptr };

	list<_tchar*> m_pTags;
public:
	static CDummyFlipBook* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pFilePath, _uint iWidthNum, _uint iHeightNum);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

