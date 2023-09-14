#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI_Group : public CGameObject
{


protected:
	explicit CUI_Group(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group(const CUI_Group& rhs);
	virtual ~CUI_Group() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	
	//	void	Set_isSelected(_bool isSelected = true) { m_isSelected = isSelected; }


	



public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END