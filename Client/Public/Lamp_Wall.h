#pragma once
#include "MapObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CLight;
END

BEGIN(Client)

class CLamp_Wall final : public CMapObject
{
private:
	explicit CLamp_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLamp_Wall(const CLamp_Wall& rhs);
	virtual ~CLamp_Wall() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CLight* m_pLight = { nullptr };

public:
	static CLamp_Wall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END