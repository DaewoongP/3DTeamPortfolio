#pragma once

#include "Item.h"
#include "Tool.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPotion abstract : public CItem, public CTool
{
protected:
	typedef struct tagPotionCreateDesc
	{
		vector<INGREDIENT>	Ingredients;
		_float				fManufacturingTime = { 0.f };
	}POTION_CREATE_DESC;

protected:
	typedef struct tagPotionCloneDesc : public tagItemCloneDesc
	{
		// 아무것도 없어도 만들어놓기.
	}POTION_CLONE_DESC;

protected:
	CPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPotion(const CPotion& rhs);
	virtual ~CPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Set_ShaderResources();

protected:
	POTION_CREATE_DESC			m_PotionCreateDesc = { POTION_CREATE_DESC() };
	_float4x4					m_WorldMatrix;
	
	_float m_fDuration = { 0.f };

private:
	CBone*						m_pTargetBone = { nullptr };

public:
	virtual void Free(void) override;
};
END
