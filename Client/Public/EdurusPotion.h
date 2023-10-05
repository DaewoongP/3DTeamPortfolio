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
class CDefence;
END

BEGIN(Client)

class CEdurusPotion final : public CPotion
{
public:
	typedef struct tagCWiggenweldPotionCreateDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CREATE_DESC;

public:
	typedef struct tagCWiggenweldPotionCloneDesc : public tagPotionCloneDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CLONE_DESC;


private:
	CEdurusPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdurusPotion(const CEdurusPotion& rhs);
	virtual ~CEdurusPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
//	void Tick(_float fTimeDelta) override;

	void Duration(_float fTimeDelta);
	void Use(_float3 vPlayPos);

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	_bool m_isDrug = { false };
	_float m_fDrugTime = { 0.f };
	_uint m_iLevel = { 0 };

	CCoolTime* m_pCooltime = { nullptr };
	CDefence* m_pDefence = { nullptr };
	_int m_iDefValue = { 0 };
	
public:
	static CEdurusPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;


};
END
