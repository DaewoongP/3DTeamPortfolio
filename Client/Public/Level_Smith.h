#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Smith final : public CLevel
{
private:
	explicit CLevel_Smith(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Smith() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	static CLevel_Smith* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END