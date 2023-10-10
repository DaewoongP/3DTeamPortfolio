#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Sanctum final : public CLevel
{
private:
	explicit CLevel_Sanctum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Sanctum() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monsters(const _tchar* pLayerTag);
	HRESULT Ready_Lights();

private:
	HRESULT Load_MapObject(const _tchar* pObjectFilePath);
	HRESULT Load_MapObject_Ins(const _tchar* pObjectFilePath);

#ifdef _DEBUG
private:
	HRESULT Ready_Debug(const _tchar* pLayerTag);
#endif // _DEBUG

public:
	static CLevel_Sanctum* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END