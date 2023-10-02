#pragma once

#include "GameObject.h"
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

enum ITEMTYPE
{
	HAND,
	FACE,
	HEAD,
	NECK,
	BACK,
	OUTFIT,
	RESOURCE,
	POTION,
	ITEMTYPE_END
};

class CItem abstract : public CGameObject
{
protected:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	CTexture* Get_UITexture() { return m_pUITexture; }
	ITEMTYPE Get_Type() { return m_eItemType; }

public:
	HRESULT Initialize_Prototype(_uint iLevel, const _tchar* pUIImagePath);
	virtual HRESULT Initialize(void* pArg) override;
	void Tick(_float fTimeDelta) override;
public:
	virtual void Use(_float3 vPlayPos,void* extraValue = nullptr) PURE;

private:
	HRESULT Add_Components();

protected:
	CTexture* m_pUITexture = { nullptr };
	ITEMTYPE m_eItemType = { ITEMTYPE_END };
	_uint m_iLevel = { 0 };
	wstring m_wstrPrototypeName = { TEXT("") };

public:
	virtual void Free(void) override;
};

END