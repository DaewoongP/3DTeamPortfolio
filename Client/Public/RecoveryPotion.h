#pragma once

#include "GameObject.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CCollider;
class CNavigation;
class CModel;

END

BEGIN(Client)

class CRecoveryPotion final : public CGameObject
{
private:
	CRecoveryPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRecoveryPotion(const CRecoveryPotion& rhs);
	virtual ~CRecoveryPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

	void Use();

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CRecoveryPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;


};
END
