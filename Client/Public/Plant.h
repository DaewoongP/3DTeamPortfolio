#pragma once

#include "Item.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CCoolTime;
class CHealth;
END

BEGIN(Client)

class CPlant abstract : public CItem
{
protected:
	typedef struct tagPlantCreateDesc
	{

	}PLANT_CREATE_DESC;

protected:
	typedef struct tagPlantCloneDesc : public tagItemCloneDesc
	{

	}PLANT_CLONE_DESC;

protected:
	CPlant(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlant(const CPlant& rhs);
	virtual ~CPlant() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

protected:
	PLANT_CREATE_DESC m_PlantDesc = { PLANT_CREATE_DESC() };

public:
	virtual void Free(void) override;
};
END


