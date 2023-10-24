#pragma once
#include "MapObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CParticleSystem;
END

BEGIN(Client)

class CFireWood final : public CMapObject
{
private:
	explicit CFireWood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFireWood(const CFireWood& rhs);
	virtual ~CFireWood() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CParticleSystem* m_pEffect = { nullptr };

public:
	static CFireWood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END