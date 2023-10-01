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

class CWiggenweldPotion final : public CItem
{
public:
	typedef struct tagCWiggenweldPotionDesc
	{
		CHealth* pHealthCom;
	}INIT_DESC;

private:
	CWiggenweldPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWiggenweldPotion(const CWiggenweldPotion& rhs);
	virtual ~CWiggenweldPotion() = default;

public:
	void Set_HealthCom(CHealth* pHealthCom);

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel, const _tchar* pUIImagePath);
	virtual HRESULT Initialize(void* pArg) override;
	
public:
	virtual void Use(_float3 vPlayPos) override;

private:
	_float m_fRecoveryAmount = { 0.2f };
	
private:
	CHealth* m_pTargetHealthCom = { nullptr };

private:
	HRESULT Add_Components();

public:
	static CWiggenweldPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pUIImagePath);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;


};
END
