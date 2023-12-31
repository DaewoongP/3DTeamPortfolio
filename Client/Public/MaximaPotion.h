#pragma once
#include"Potion.h"
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
class CMagicSlot;

class CMaximaPotion final : public CPotion
{
public:
	typedef struct tagCFocusPotionCreateDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CREATE_DESC;

public:
	typedef struct tagCFocusPotionCloneDesc : public tagPotionCloneDesc
	{
		// 아무것도 없어도 일단 만들어놓기.
	}CLONE_DESC;

private:
	CMaximaPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMaximaPotion(const CMaximaPotion& rhs);
	virtual ~CMaximaPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	void Duration(_float fTimeDelta);

private:
	_bool m_isDrug = { false };
	_float m_fDrugTime = { 0.f };

	CMagicSlot* m_pMagicSlot = { nullptr };

public:
	static CMaximaPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;


};
END
