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
class CUI_Font;
class CItem;
END

BEGIN(Client)
class CUI_Farming final : public CGameObject
{
private:
	enum STATE { LEFT, LOOP, RIGHT, STATE_END };

private:
	explicit CUI_Farming(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Farming(const CUI_Farming& rhs);
	virtual ~CUI_Farming() = default;

public:
	HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

public:
	void Play(CItem* pItem);

private:
	HRESULT Farming_Read_File(const _tchar* pFilePath);
	HRESULT FontBack_Read_File(const _tchar* pFilePath);

	CUI::UIDESC Load_File(const HANDLE hFile, _bool isDDS = true);

private:
	CUI_Dynamic_Back* m_pUIFarmingBack = { nullptr };
	CUI_Dynamic_Back* m_pUIFarmingFrame = { nullptr };
	CUI_Dynamic_Back* m_pUIFarming_Font_Back = { nullptr };
	CUI_Dynamic_Back* m_pUIFarming_Font_TopFrame = { nullptr };
	CUI_Dynamic_Back* m_pUIFarming_Font_BotFrame = { nullptr };
	CUI_Font* m_pUIFont = { nullptr };

private:
	_float2	m_vFontBackOriginPos = { _float2() };
	STATE	m_eState = { STATE_END };
	_bool	m_isEnable = { false };
	_float	m_fTimeAcc = { 0.f };
	wstring	m_wstrKoreanName = { TEXT("") };

private:
	HRESULT Add_Compoents();

public:
	static CUI_Farming* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END