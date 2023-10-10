#pragma once
#include "UI.h"
#include "Client_Defines.h"
#include "UI_Image.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)
class CUI_Effect_Back;
class CUI_Back;

class CUI_Group_Cursor final : public CGameObject
{
private:
	explicit CUI_Group_Cursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_Cursor(const CUI_Group_Cursor& rhs);
	virtual ~CUI_Group_Cursor() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	_bool	Get_Clicked() {
		return  m_pUI_Effect_Back->Get_Clicked();
	}
	_bool	Get_Pressing() {
		return m_pUI_Effect_Back->Get_Pressing();
	}
	void	Set_Image(CTexture* pTexture) {
		m_pUI_Effect_Back->Set_Texture(pTexture);
	}
	void	Set_ImageDesc(CUI_Image::IMAGEDESC imageDesc, _bool isChild) {
	m_pUI_Effect_Back->Set_ImageCom(imageDesc, isChild);
	}
	void Set_CursorPos();

private:
	CUI_Effect_Back*		m_pUI_Effect_Back= { nullptr };
	CUI_Back*				m_pUI_Back		= { nullptr };

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components();
	HRESULT Read_File(const _tchar* pFilePath);
	CUI::UIDESC Load_File(const HANDLE hFile);

public:
	static CUI_Group_Cursor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END