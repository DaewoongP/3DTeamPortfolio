#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)
class CUI_Effect_Back;
class CUI_Font;
class CUI_Potion_Tap;
class CTool;

class CPotionTap final : public CGameObject
{
private:
	CPotionTap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPotionTap(const CPotionTap& rhs);
	virtual ~CPotionTap() = default;

public:
	void	Set_Open(_bool isOpen) {
		m_isOpen = isOpen;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	_bool Is_Valid(POTIONTAP ePotionTap);

public:
	_bool			Get_Open() { return m_isOpen; }

private:
	_bool			m_isOpen = { false };

private:
	// 메인화면 탭 UI
	CUI_Effect_Back*				m_pUI_Main_Tap = { nullptr };
	CUI_Font*					m_pUI_Main_Count = { nullptr };
	vector<CTexture*>				m_pPotionTextures;

	// UI 컴포넌트
	CUI_Potion_Tap*				m_pUI_Potion_Tap = { nullptr };
	// 실질적인 포션 저장소
	vector<vector<CItem*>>			m_pPotions;

private:
	POTIONTAP					m_eCurPotion = { POTIONTAP_END };

private:
	HRESULT Add_Components();
	HRESULT Ready_Main_Tap();
	HRESULT	Ready_PotionTextures();

private:
	CItem* ToolFactory(POTIONTAP eType);

public:
	void	Add_Potion(POTIONTAP eType);
	void	Delete_Potion(POTIONTAP eType, _uint iIndex);

	void	Set_CurPotion();

	void	Use_Item(_float3 vPlayPos);

public:
	static CPotionTap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END