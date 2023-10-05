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
		wstring				wstrModelPath = { TEXT("") };
		CModel::TYPE		eModelType = { CModel::TYPE_END };
		_float4x4			PivotMatrix = { _float4x4() };
		POTIONTAP			ePotionTap = { POTIONTAP_END };
	}POTION_CREATE_DESC;

protected:
	typedef struct tagPotionCloneDesc : public tagItemCloneDesc
	{
		_float4x4        OffsetMatrix;
		_float4x4        PivotMatrix;
		const _float4x4* pCombindTransformationMatrix;
		const _float4x4* pParentWorldMatrix;
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

	virtual void Use(_float3 vPlayPos) override;

private:
	HRESULT Set_ShaderResources();

protected:
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };
	CShader* m_pShader = { nullptr };
	CCoolTime* m_pLifeTime = { nullptr };
	CCoolTime* m_pAttachedTime = { nullptr };

protected:
	POTION_CREATE_DESC			m_PotionCreateDesc = { POTION_CREATE_DESC() };

private:
	_bool			 m_isAttached = { false };
	_float4x4        m_OffsetMatrix;
	_float4x4        m_PivotMatrix;
	const _float4x4* m_pCombindTransformationMatrix;
	const _float4x4* m_pParentWorldMatrix;

	
private:
	HRESULT Add_Components();

public:
	virtual void Free(void) override;
};
END
