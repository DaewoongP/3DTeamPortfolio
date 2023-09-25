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
class CUI_Slot;
class CInventory;

class CUI_Inventory final : public CGameObject
{
public:
	struct INVENDESC
	{
		CUI::UIDESC UIDesc;

		_float2					fOffset;
		_float					fWidth;
		_float					fHeight;
		_uint					iHorizontal;
		_uint					iVertical;
		CInventory::ITEMTYPE	eItemtype;
	};

private:
	explicit CUI_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Inventory(const CUI_Inventory& rhs);
	virtual ~CUI_Inventory() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Prototype();

private:
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	HRESULT Ready_Offset();
	HRESULT	Add_ItemTexture();

public:
	HRESULT	Set_InventoryItem(vector<CGameObject*>& pItems);
	HRESULT	Delete_InventoryItem(_uint iIndex);
	HRESULT	Swap_InventoryItem();

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

private:
	_float2					m_fOffset;
	_float					m_fWidth;
	_float					m_fHeight;
	_uint					m_iHorizontal;
	_uint					m_iVertical;
	CInventory::ITEMTYPE	m_eItemtype;

private:
	vector<_float2>					m_fPosition;
	vector<class CTexture*>			m_ItemTextures;
	vector<CUI_Slot*>				m_pSlots;

private:
	_bool		m_isOpen = { false };

public:
	static CUI_Inventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END