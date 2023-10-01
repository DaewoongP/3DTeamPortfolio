#pragma once
#include"Item.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CCoolTime;
class CMagic;
END

BEGIN(Client)

class CPowerPotion final : public CItem
{
private:
	CPowerPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPowerPotion(const CPowerPotion& rhs);
	virtual ~CPowerPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel, const _tchar* pUIImagePath);
	virtual HRESULT Initialize(void* pArg) override;

	void Use(_float3 vPlayPos);

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	_bool m_isDrug = { false };
	_float m_fDrugTime = { 0.f };
	_uint m_iLevel = { 0 };

	CCoolTime* m_pCooltime = { nullptr };
	CMagic* m_pMagic = { nullptr };

public:
	static CPowerPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pUIImagePath);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;


};
END
