#pragma once
#include "GameObject.h"
#include "MapObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
class CParticleSystem;
END

BEGIN(Client)
class CUI_Group_Brew;
class CUI_Interaction;

class CPotion_Station final : public CMapObject
{
	enum STATE { IDLE, INTERACTABLE, SHOW, STATE_END};
private:
	explicit CPotion_Station(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPotion_Station(const CPotion_Station& rhs);
	virtual ~CPotion_Station() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

#ifdef _DEBUG
	void Tick_Imgui(_float fTimeDelta);
#endif // _DEBUG

private:
	CUI_Group_Brew* m_pCUI_Group_Brew = {nullptr};

private:
	class CPlayer* m_pPlayer = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };
	CUI_Interaction* m_pUI_Interaction = { nullptr };

private:
	STATE m_eState = { IDLE };
	_uint m_iLevel = {};

private: // For.Components
	CParticleSystem* m_pWaterSmoke = { nullptr };
	CParticleSystem* m_pPotFire = { nullptr };

private:
	HRESULT Add_Components();

public:
	static CPotion_Station* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END