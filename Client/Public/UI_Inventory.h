#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_Slot;
class CUI_Back;
class CInventory;
class CItem;

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
		ITEMTYPE				eItemtype;
	};

	struct PLURAL
	{
		_bool	 isPlural = { false };
		_bool	 isCreate = { true };
	};

private:
	explicit CUI_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Inventory(const CUI_Inventory& rhs);
	virtual ~CUI_Inventory() = default;

public:
	ITEMTYPE Get_CurType() { return m_eItemtype; }

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
	HRESULT  Ready_Offset();
	HRESULT	Ready_DefaultTexture(const _tchar* pFilePath);
	HRESULT	Add_ItemTexture();

public:
	HRESULT	Set_GearInventoryItem(vector<CItem*>& pItems);
	HRESULT	Set_ResourceInventoryItem(vector<CItem*>& pItems, vector<_uint>* ResourceCount = nullptr);
	HRESULT	Delete_InventoryItem(_uint iIndex);
	HRESULT	Swap_InventoryItem();

private:
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

private:
	_float2					m_fOffset;
	_float					m_fWidth;
	_float					m_fHeight;
	_uint					m_iHorizontal;
	_uint					m_iVertical;
	ITEMTYPE					m_eItemtype;

private:
	vector<_float2>					m_fPosition;
	vector<class CTexture*>			m_ItemTextures;

	vector<CUI_Slot*>					m_pSlots;

	CUI_Back*							m_pBack;

private:
	_bool		m_isOpen = { false };

public:
	static CUI_Inventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END