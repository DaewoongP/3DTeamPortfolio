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
END

BEGIN(Client)

class CAccPotion final : public CGameObject
{
private:
	CAccPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAccPotion(const CAccPotion& rhs);
	virtual ~CAccPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;

	void Use();

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	_bool m_isDrug = { false };
	_float m_fDrugTime = { 0.f };


	CCoolTime* m_pCooltime = { nullptr };


public:
	static CAccPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;


};
END
