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
	// �������� ������Ÿ�Կ��� ��ƼŬ ��θ� �޾���.
	HRESULT Initialize_Prototype(const _tchar* _pDirectoryPath, _uint _iLevel);
	// Ŭ�󿡼��� �̴ϼȶ������ ��� �޾��༭ �ʱ�ȭ �ϸ� ��
	virtual HRESULT Initialize(void* _pArg) override;

private:
	// ��ƼŬ �ؽ�ó ����.
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
	// �������� �Ű����� �߰�. Ŭ�󿡴� �����ϱ�
	static CDummyTrail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath, _uint iLevel = 0);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free(void) override;
};
END