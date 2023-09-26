#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Vault final : public CLevel
{
private:
	explicit CLevel_Vault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Vault() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();
	HRESULT Light_Out();
public:
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);

	



private:
	HRESULT Ready_Lights();


public:
	static CLevel_Vault* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END