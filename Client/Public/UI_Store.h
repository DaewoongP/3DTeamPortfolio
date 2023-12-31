#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CUI_Progress;
END

BEGIN(Client)
class CUI_Dynamic_Back;
class CUI_Effect_Back;
class CUI_Back;
class CUI_Font;
class CItem;
class CUI_Group_Cursor;
class CUI_Slot;
END

BEGIN(Client)
class CUI_Store final : public CGameObject
{
private:
	explicit CUI_Store(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Store(const CUI_Store& rhs);
	virtual ~CUI_Store() = default;

public:
	HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

public:
	void	Set_isOpen(_bool isOpen) {
		m_isOpen = isOpen;
	}
	void Open();
	void Close();
private:
	HRESULT Ready_Offset();
	HRESULT Ready_DefaultTexture();
	HRESULT Store_Sell_Read_File(const _tchar* pFilePath);
	HRESULT Store_Buy_Read_File(const _tchar* pFilePath);

	CUI::UIDESC Load_File(const HANDLE hFile, _bool isDDS = true);

	HRESULT	Set_Item();

private:
	_uint m_iLevel = { 0 };
	_bool m_isOpen = { true };
	_bool m_isSetGold = { true };

private:
	CUI_Back* m_pStoreDefault = { nullptr };
	CUI_Group_Cursor* m_pCursor = { nullptr };

	CUI_Effect_Back* m_pSellFrame = { nullptr }; // 0
	CUI_Effect_Back* m_pSellIcon = { nullptr }; // 1
	CUI_Back* m_pSellBack = { nullptr }; // 2

	CUI_Effect_Back* m_pBuyFrame = { nullptr }; // 0
	CUI_Effect_Back* m_pBuyIcon = { nullptr }; // 1
	CUI_Back* m_pBuyBack = { nullptr }; // 2

	CUI_Font* m_pGoldFont = { nullptr };

private:
	HRESULT Add_Compoents();

private:
	_float2					m_fOffset;
	_float					m_fWidth;
	_float					m_fHeight;
	_uint					m_iHorizontal;
	_uint					m_iVertical;
	
private:
	vector<_float2>					m_fPosition;
	vector<CUI_Slot*>				m_pSlots;
	vector<CItem*>                  m_pItems;

	class CInventory* m_pInventory = { nullptr };

public:
	static CUI_Store* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END