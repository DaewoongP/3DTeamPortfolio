#pragma once
#include "Component.h"
#include "Client_Defines.h"
#include "GameObject.h"
#include "Item.h"

BEGIN(Client)
class CUI_Potion_Tap;

class CPotionTap final : public CGameObject
{
public:
	enum POTIONTAP
	{
		DEFENSIVE_POWER_UP, 
		ATTACK_POWER_UP,    
		SHOW_TIME,         
		THUNDER_CLOUD,     
		INVISIBILITY_PILL,  
		MANDRAKE,          
		BITE_CABBAGE,      
		TENTACULAR,        
		POTIONTAP_END
	};

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
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	_bool Get_Open() { return m_isOpen; }

private:
	_bool			m_isOpen = { false };

private:
	// UI 컴포넌트
	CUI_Potion_Tap*				m_pUI_Potion_Tap = { nullptr };
	// 실질적인 포션 저장소
	vector<vector<CItem*>>			m_pPotions;

private:
	POTIONTAP						m_eCurPotion = { POTIONTAP_END };

private:
	HRESULT Add_Components();

public:
	void	Add_Potion(CItem* pItem, ITEMTYPE eType);
	void	Delete_Potion(ITEMTYPE eType, _uint iIndex);

public:
	static CPotionTap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END