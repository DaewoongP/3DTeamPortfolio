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
class CUI_Effect_Back;
class CUI_Back;

class CMenu_Gear final : public CGameObject
{
public:
	enum GEARSLOT
	{
		HAND,
		FACE,
		HEAD,
		NECK,
		BACK,
		OUTFIT,
		GEARSLOT_END
	};
	
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



	HRESULT Ready_Gear_Frame();
	HRESULT Add_Components_Frame();
	HRESULT Read_File_Frame(const _tchar* pFilePath);

	HRESULT Ready_Gear_Slot(const _tchar* pFilePath, wstring wszTag, GEARSLOT eType);
	HRESULT Add_Components_Slot(wstring wszTag);
	HRESULT Read_File_Slot(const _tchar* pFilePath, wstring wszTag, GEARSLOT eType);

	HRESULT Ready_Gear_Status();
	HRESULT Add_Components_Status();
	HRESULT Read_File_Statust(const _tchar* pFilePath);

private:
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	vector<CUI_Effect_Back*> pSlotFrames;
	vector<CUI_Effect_Back*> pSlotIcons;

	vector<CUI_Back*> pStatuses;
	vector<CUI*> pComponents;

private:
	_bool		m_isOpen = { false };

public:
	static CMenu_Gear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END