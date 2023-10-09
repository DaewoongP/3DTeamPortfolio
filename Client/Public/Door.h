#pragma once
#include "MapObject.h"

BEGIN(Client)

class CDoor final : public CMapObject
{
protected:
	explicit CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDoor(const CDoor& rhs);
	virtual ~CDoor() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	void Open_Door(_float fTimeDelta);

public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END