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
END

BEGIN(Client)

class CPotion_Station final : public CMapObject
{
private:
	explicit CPotion_Station(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPotion_Station(const CPotion_Station& rhs);
	virtual ~CPotion_Station() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	class CPlayer* m_pPlayer = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };

	_bool m_isInteractable = { false };

public:
	static CPotion_Station* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END