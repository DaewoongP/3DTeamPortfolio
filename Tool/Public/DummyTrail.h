#pragma once

#include "Trail.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
class CMeshEffect;
END

BEGIN(Tool)
class CImageFileDialog;
class CComboBox;
END;

BEGIN(Tool)
class CEffect_Window;

class CDummyTrail : public CTrail
{
	friend class CEffect_Window;

private:
	explicit CDummyTrail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDummyTrail(const CDummyTrail& _rhs);
	virtual ~CDummyTrail() = default;

public:
	// 툴에서만 프로토타입에서 파티클 경로를 받아줌.
	HRESULT Initialize_Prototype(const _tchar* _pDirectoryPath, _uint _iLevel);
	// 클라에서는 이니셜라이즈에서 경로 받아줘서 초기화 하면 됨
	virtual HRESULT Initialize(void* _pArg) override;

private:
	// 파티클 텍스처 변경.
	void ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath);
	void RemakeBuffer();

public:
	void Save_FileDialog();
	void Load_FileDialog();

public:
	void Tick_Imgui(_float _fTimeDelta);

private:
	CImageFileDialog* m_pTextureIFD = { nullptr };
	CComboBox* m_pPassComboBox = { nullptr };
	list<_tchar*> m_pTags;

public:
	// 툴에서는 매개변수 추가. 클라에는 삭제하기
	static CDummyTrail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath, _uint iLevel = 0);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free(void) override;
};
END