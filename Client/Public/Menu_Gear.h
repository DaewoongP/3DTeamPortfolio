#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CMenu_Gear final : public CGameObject
{
private:
	explicit CMenu_Gear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMenu_Gear(const CMenu_Gear& rhs);
	virtual ~CMenu_Gear() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components();
	HRESULT Add_TextComponent(wstring wszTag);

private:
	HRESULT Read_File(const _tchar* pFilePath);
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTexture = { nullptr };

public:
	static CMenu_Gear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END