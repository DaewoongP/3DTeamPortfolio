#pragma once
#include"Potion.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CCoolTime;
END

BEGIN(Client)
class CMagicSlot;

class CThunderBrewPotion final : public CPotion
{
public:
	typedef struct tagCFocusPotionCreateDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CREATE_DESC;

public:
	typedef struct tagCFocusPotionCloneDesc : public tagPotionCloneDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CLONE_DESC;

private:
	CThunderBrewPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThunderBrewPotion(const CThunderBrewPotion& rhs);
	virtual ~CThunderBrewPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CThunderBrewPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;


};
END
