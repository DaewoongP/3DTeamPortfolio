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

class CEdurusPotion_Item final : public CTool
{
private:
	typedef struct tagCEdurusPotion_ItemCreateDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CREATE_DESC;

public:
	typedef struct tagCEdurusPotion_ItemCloneDesc : public tagToolCloneDesc
	{
		_bool* pIsStartTiming = { nullptr };
	}CLONE_DESC;

private:
	CEdurusPotion_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdurusPotion_Item(const CEdurusPotion_Item& rhs);
	virtual ~CEdurusPotion_Item() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Use(_float3 vPlayPos) override;
	virtual void CreateTool(void* pArg) override;

private:
	CLONE_DESC m_Clone_Desc = { CLONE_DESC() };

public:
	static CEdurusPotion_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END


