#pragma once

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

class CInvisibilityPotion final : public CPotion
{
private:
	CInvisibilityPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvisibilityPotion(const CInvisibilityPotion& rhs);
	virtual ~CInvisibilityPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Duration(_float fTimeDelta);

public:
	static CInvisibilityPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END
