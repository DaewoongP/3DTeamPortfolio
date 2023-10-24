#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Sky final : public CLevel
{
private:
	explicit CLevel_Sky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Sky() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Shader();

private:
	HRESULT Load_MapObject(const _tchar* pObjectFilePath);
	HRESULT Load_MapObject_Ins(const _tchar* pObjectFilePath);

public:
	static CLevel_Sky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END