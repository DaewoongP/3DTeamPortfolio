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

class CWiggenweldPotion final : public CPotion
{
private:
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
	CWiggenweldPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWiggenweldPotion(const CWiggenweldPotion& rhs);
	virtual ~CWiggenweldPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Use(_float3 vPlayPos);

private:
	CHealth*	m_pPlayerHealthCom = { nullptr };
	CPlayer_Information* m_pPlayerInformation = { nullptr };
	_float		m_fRecoveryAmount = { 0.2f };

public:
	static CWiggenweldPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END
