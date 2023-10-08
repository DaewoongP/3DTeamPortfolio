#pragma once
#include "Tool.h"
#include "Potion.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"

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

class CFocusPotion_Item final : public CTool
{
private:
	typedef struct tagCFocusPotion_ItemCreateDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CREATE_DESC;

public:
	typedef struct tagCFocusPotion_ItemCloneDesc : public tagToolCloneDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CLONE_DESC;

private:
	CFocusPotion_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFocusPotion_Item(const CFocusPotion_Item& rhs);
	virtual ~CFocusPotion_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Use(_float3 vPlayPos);
	virtual void CreateTool() override;
public:
	static CFocusPotion_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END


