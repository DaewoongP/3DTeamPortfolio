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

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monsters(const _tchar* pLayerTag);
	HRESULT Ready_Layer_NPC(const _tchar* pLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_UI(const _tchar* pLayerTag);
	HRESULT Ready_Event(const _tchar* pLayerTag);

private:
	HRESULT Load_Dummy_NPC(const _tchar* pLayerTag);
	HRESULT Load_MapObject(const _tchar* pObjectFilePath);
	HRESULT Load_MapObject_Ins(const _tchar* pObjectFilePath);

public:
	static CLevel_Smith* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END