#pragma once

#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI_Back;
class CUI_Font;
class CUI_Effect_Back;
class CItem;
class CUI_Group_Cursor;

class CUI_Potion_Tap final : public CGameObject
{
private:
	explicit CUI_Potion_Tap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Potion_Tap(const CUI_Potion_Tap& rhs);
	virtual ~CUI_Potion_Tap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT	Add_Prototype();
	HRESULT	Ready_DefaultTexture();

private:
	HRESULT Create_Component(const _tchar* pFIlePath, wstring wszTag, POTIONTAP eType);
	HRESULT Add_Components(wstring wszTag, POTIONTAP eType);
	HRESULT Read_File(const _tchar* pFilePath, POTIONTAP iIndex);
	CUI::UIDESC Load_File(const HANDLE hFile, _bool isDDS = true);

public:
	HRESULT	Set_ItemCount(vector<vector<CItem*>>& pItems);
	_uint	Set_CurTapItem();
	void	Update_PotionCount(const vector<vector<CTool*>>& pPotions);

private:
	void Update_TextCount(const vector<vector<CTool*>>& pPotions, POTIONTAP ePotionTap);

private:
	vector<CUI_Back*>				m_pIcons;
	vector<CUI_Effect_Back*>		m_pFrames;
	vector<CUI_Back*>				m_pCountBacks;
	vector<CUI_Effect_Back*>		m_pItemBacks;
	vector<CUI_Font*>				m_pFonts;

	CUI_Back*					m_pBack;
	CUI_Group_Cursor*				m_pCursor;

private:
	_bool						m_isOpen = { false };

public:
	static CUI_Potion_Tap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END