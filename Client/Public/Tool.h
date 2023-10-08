#pragma once

#include "Item.h"
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
class CTool abstract : public CItem
{
protected:
	typedef struct tagCToolCreateDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CREATE_DESC;

public:
	typedef struct tagToolCloneDesc : public tagItemCloneDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CLONE_DESC;

protected:
	CTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool(const CTool& rhs);
	virtual ~CTool() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	virtual void Use(_float3 vPlayPos) PURE;
	virtual void CreateTool() PURE;

public:
	virtual void Free(void) override;
};
END
