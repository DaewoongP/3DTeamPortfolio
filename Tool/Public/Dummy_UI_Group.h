#pragma once
#include "UI_Gruop.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CDummy_UI_Group final : public CUI_Gruop
{
protected:
	explicit CDummy_UI_Group(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummy_UI_Group(const CDummy_UI_Group& rhs);
	virtual ~CDummy_UI_Group() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
//	void	Set_isSelected(_bool isSelected = true) { m_isSelected = isSelected; }
	void	Delete(class CDummy_UI* pDeleteUI);
	void	Clear();

public:
	static CDummy_UI_Group* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END